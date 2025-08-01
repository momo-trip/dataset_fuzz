/* $Id$ */

/*
 *   Copyright (c) 2001-2010 Aaron Turner <aturner at synfin dot net>
 *   Copyright (c) 2013-2024 Fred Klassen <tcpreplay at appneta dot com> - AppNeta
 *
 *   The Tcpreplay Suite of tools is free software: you can redistribute it
 *   and/or modify it under the terms of the GNU General Public License as
 *   published by the Free Software Foundation, either version 3 of the
 *   License, or with the authors permission any later version.
 *
 *   The Tcpreplay Suite is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details.
 *
 *   You should have received a copy of the GNU General Public License
 *   along with the Tcpreplay Suite.  If not, see <http://www.gnu.org/licenses/>.
 */

/*
 * Purpose: Modify packets in a pcap file based on rules provided by the
 * user to offload work from tcpreplay and provide an easier means of
 * reproducing traffic for testing purposes.
 */

#include "tcprewrite.h"
#include "config.h"
#include "common.h"
#include "tcpedit/fuzzing.h"
#include "tcpedit/tcpedit.h"
#include "tcprewrite_opts.h"
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>

#ifdef DEBUG
int debug;
#endif

#ifdef ENABLE_VERBOSE
/* tcpdump handle */
tcpdump_t tcpdump;
#endif

tcprewrite_opt_t options;
tcpedit_t *tcpedit;

/* local functions */
void tcprewrite_init(void);
void post_args(int argc, char *argv[]);
int rewrite_packets(tcpedit_t *tcpedit_ctx, pcap_t *pin, pcap_dumper_t *pout);

int
original_main(int argc, char *argv[])
{
    int optct, rcode;
    pcap_t *dlt_pcap;
#ifdef ENABLE_FRAGROUTE
    char ebuf[FRAGROUTE_ERRBUF_LEN];
#endif
    tcprewrite_init();

    /* call autoopts to process arguments */
    optct = optionProcess(&tcprewriteOptions, argc, argv);
    argc -= optct;
    argv += optct;

    /* parse the tcprewrite args */
    post_args(argc, argv);

    /* init tcpedit context */
    if (tcpedit_init(&tcpedit, pcap_datalink(options.pin)) < 0) {
        err_no_exitx("Error initializing tcpedit: %s", tcpedit_geterr(tcpedit));
        tcpedit_close(&tcpedit);
        exit(-1);
    }

    /* parse the tcpedit args */
    rcode = tcpedit_post_args(tcpedit);
    if (rcode < 0) {
        err_no_exitx("Unable to parse args: %s", tcpedit_geterr(tcpedit));
        tcpedit_close(&tcpedit);
        exit(-1);
    } else if (rcode == 1) {
        warnx("%s", tcpedit_geterr(tcpedit));
    }

    if (tcpedit_validate(tcpedit) < 0) {
        err_no_exitx("Unable to edit packets given options:\n%s", tcpedit_geterr(tcpedit));
        tcpedit_close(&tcpedit);
        exit(-1);
    }

    /* fuzzing init */
    fuzzing_init(tcpedit->fuzz_seed, tcpedit->fuzz_factor);

    /* open up the output file */
    options.outfile = safe_strdup(OPT_ARG(OUTFILE));
    dbgx(1, "Rewriting DLT to %s", pcap_datalink_val_to_name(tcpedit_get_output_dlt(tcpedit)));
    if ((dlt_pcap = pcap_open_dead(tcpedit_get_output_dlt(tcpedit), 65535)) == NULL) {
        tcpedit_close(&tcpedit);
        err(-1, "Unable to open dead pcap handle.");
    }

    dbgx(1, "DLT of dlt_pcap is %s", pcap_datalink_val_to_name(pcap_datalink(dlt_pcap)));

#ifdef ENABLE_FRAGROUTE
    if (options.fragroute_args) {
        if ((options.frag_ctx = fragroute_init(65535, pcap_datalink(dlt_pcap), options.fragroute_args, ebuf)) == NULL) {
            err_no_exitx("%s", ebuf);
            tcpedit_close(&tcpedit);
            exit(-1);
        }
    }
#endif

#ifdef ENABLE_VERBOSE
    if (options.verbose) {
        tcpdump_open(&tcpdump, dlt_pcap);
    }
#endif

    if ((options.pout = pcap_dump_open(dlt_pcap, options.outfile)) == NULL) {
        err_no_exitx("Unable to open output pcap file: %s", pcap_geterr(dlt_pcap));
        tcpedit_close(&tcpedit);
        exit(-1);
    }

    pcap_close(dlt_pcap);

    /* rewrite packets */
    if (rewrite_packets(tcpedit, options.pin, options.pout) == TCPEDIT_ERROR) {
        err_no_exitx("Error rewriting packets: %s", tcpedit_geterr(tcpedit));
        tcpedit_close(&tcpedit);
        exit(-1);
    }

    /* clean up after ourselves */
    pcap_dump_close(options.pout);
    pcap_close(options.pin);
    tcpedit_close(&tcpedit);

#ifdef ENABLE_VERBOSE
    tcpdump_close(&tcpdump);
#endif

#ifdef ENABLE_FRAGROUTE
    if (options.frag_ctx) {
        fragroute_close(options.frag_ctx);
    }
#endif

#ifdef ENABLE_DMALLOC
    dmalloc_shutdown();
#endif

    restore_stdin();
    return 0;
}

void
tcprewrite_init(void)
{
    memset(&options, 0, sizeof(options));

#ifdef ENABLE_VERBOSE
    /* clear out tcpdump struct */
    memset(&tcpdump, '\0', sizeof(tcpdump_t));
#endif

    if (fcntl(STDERR_FILENO, F_SETFL, O_NONBLOCK) < 0)
        warnx("Unable to set STDERR to non-blocking: %s", strerror(errno));
}

/**
 * post AutoGen argument processing
 */
void
post_args(_U_ int argc, _U_ char *argv[])
{
    char ebuf[PCAP_ERRBUF_SIZE];

#ifdef DEBUG
    if (HAVE_OPT(DBUG))
        debug = OPT_VALUE_DBUG;
#else
    if (HAVE_OPT(DBUG))
        warn("not configured with --enable-debug.  Debugging disabled.");
#endif

    if (HAVE_OPT(SUPPRESS_WARNINGS))
        print_warnings = 0;

#ifdef ENABLE_VERBOSE
    if (HAVE_OPT(VERBOSE))
        options.verbose = 1;

    if (HAVE_OPT(DECODE))
        tcpdump.args = safe_strdup(OPT_ARG(DECODE));
#endif

#ifdef ENABLE_FRAGROUTE
    if (HAVE_OPT(FRAGROUTE))
        options.fragroute_args = safe_strdup(OPT_ARG(FRAGROUTE));

    options.fragroute_dir = FRAGROUTE_DIR_BOTH;
    if (HAVE_OPT(FRAGDIR)) {
        if (strcmp(OPT_ARG(FRAGDIR), "c2s") == 0) {
            options.fragroute_dir = FRAGROUTE_DIR_C2S;
        } else if (strcmp(OPT_ARG(FRAGDIR), "s2c") == 0) {
            options.fragroute_dir = FRAGROUTE_DIR_S2C;
        } else if (strcmp(OPT_ARG(FRAGDIR), "both") == 0) {
            options.fragroute_dir = FRAGROUTE_DIR_BOTH;
        } else {
            errx(-1, "Unknown --fragdir value: %s", OPT_ARG(FRAGDIR));
        }
    }
#endif

    /* open up the input file */
    options.infile = safe_strdup(OPT_ARG(INFILE));
    if ((options.pin = pcap_open_offline(options.infile, ebuf)) == NULL)
        errx(-1, "Unable to open input pcap file: %s", ebuf);

#ifdef HAVE_PCAP_SNAPSHOT
    if (pcap_snapshot(options.pin) < 65535)
        warnx("%s was captured using a snaplen of %d bytes.  This may mean you have truncated packets.",
              options.infile,
              pcap_snapshot(options.pin));
#endif
}

/**
 * Main loop to rewrite packets
 */
int
rewrite_packets(tcpedit_t *tcpedit_ctx, pcap_t *pin, pcap_dumper_t *pout)
{
    tcpr_dir_t cache_result = TCPR_DIR_C2S; /* default to primary */
    struct pcap_pkthdr pkthdr, *pkthdr_ptr; /* packet header */
    const u_char *pktconst = NULL;          /* packet from libpcap */
    u_char **pktdata = NULL;
    static u_char *pktdata_buff;
    static char *frag = NULL;
    COUNTER packetnum = 0;
    int rcode;
#ifdef ENABLE_FRAGROUTE
    int frag_len, proto;
#endif

    pkthdr_ptr = &pkthdr;

    if (pktdata_buff == NULL)
        pktdata_buff = (u_char *)safe_malloc(MAXPACKET);

    pktdata = &pktdata_buff;

    if (frag == NULL)
        frag = (char *)safe_malloc(MAXPACKET);

    /* MAIN LOOP
     * Keep sending while we have packets or until
     * we've sent enough packets
     */
    while ((pktconst = safe_pcap_next(pin, pkthdr_ptr)) != NULL) {
        packetnum++;
        dbgx(2, "packet " COUNTER_SPEC " caplen %d", packetnum, pkthdr.caplen);

        if (pkthdr.caplen > MAX_SNAPLEN)
            errx(-1, "Frame too big, caplen %d exceeds %d", pkthdr.caplen, MAX_SNAPLEN);
        /*
         * copy over the packet so we can pad it out if necessary and
         * because pcap_next() returns a const ptr
         */
        memcpy(*pktdata, pktconst, pkthdr.caplen);

        /* Dual nic processing? */
        if (options.cachedata != NULL) {
            cache_result = check_cache(options.cachedata, packetnum);
        }

        /* sometimes we should not send the packet, in such cases
         * no point in editing this packet at all, just write it to the
         * output file (note, we can't just remove it, or the tcpprep cache
         * file will lose it's indexing
         */

        if (cache_result == TCPR_DIR_NOSEND)
            goto WRITE_PACKET; /* still need to write it so cache stays in sync */

        if ((rcode = tcpedit_packet(tcpedit_ctx, &pkthdr_ptr, pktdata, cache_result)) == TCPEDIT_ERROR) {
            return rcode;
        } else if ((rcode == TCPEDIT_SOFT_ERROR) && HAVE_OPT(SKIP_SOFT_ERRORS)) {
            /* don't write packet */
            dbgx(1, "Packet " COUNTER_SPEC " is suppressed from being written due to soft errors", packetnum);
            continue;
        }

#ifdef ENABLE_VERBOSE
        if (options.verbose)
            tcpdump_print(&tcpdump, pkthdr_ptr, *pktdata);
#endif

WRITE_PACKET:
#ifdef ENABLE_FRAGROUTE
        if (options.frag_ctx == NULL) {
            /* write the packet when there's no fragrouting to be done */
            if (pkthdr_ptr->caplen)
                pcap_dump((u_char *)pout, pkthdr_ptr, *pktdata);
        } else {
            /* get the L3 protocol of the packet */
            proto = tcpedit_l3proto(tcpedit_ctx, AFTER_PROCESS, *pktdata, pkthdr_ptr->caplen);

            /* packet is IPv4/IPv6 AND needs to be fragmented */
            if ((proto == ETHERTYPE_IP || proto == ETHERTYPE_IP6) &&
                ((options.fragroute_dir == FRAGROUTE_DIR_BOTH) ||
                 (cache_result == TCPR_DIR_C2S && options.fragroute_dir == FRAGROUTE_DIR_C2S) ||
                 (cache_result == TCPR_DIR_S2C && options.fragroute_dir == FRAGROUTE_DIR_S2C))) {
#ifdef DEBUG
                int i = 0;
#endif
                if (fragroute_process(options.frag_ctx, *pktdata, pkthdr_ptr->caplen) < 0)
                    errx(-1, "Error processing packet via fragroute: %s", options.frag_ctx->errbuf);

                while ((frag_len = fragroute_getfragment(options.frag_ctx, &frag)) > 0) {
                    /* frags get the same timestamp as the original packet */
                    dbgx(1, "processing packet " COUNTER_SPEC " frag: %u (%d)", packetnum, i++, frag_len);
                    pkthdr_ptr->caplen = frag_len;
                    pkthdr_ptr->len = frag_len;
                    if (pkthdr_ptr->caplen)
                        pcap_dump((u_char *)pout, pkthdr_ptr, (u_char *)frag);
                }
            } else {
                /* write the packet without fragroute */
                if (pkthdr_ptr->caplen)
                    pcap_dump((u_char *)pout, pkthdr_ptr, *pktdata);
            }
        }
#else
        /* write the packet when there's no fragrouting to be done */
        if (pkthdr_ptr->caplen)
            pcap_dump((u_char *)pout, pkthdr_ptr, *pktdata);

#endif
    } /* while() */
    return 0;
}


#include <time.h>    /* for time(), localtime(), strftime() */
#include <errno.h>   /* for errno */
#include <sys/stat.h> /* for struct stat, stat() */
#include <unistd.h>  /* for other functions */

// Custom string copy function
char* my_string_copy(const char* src) {
  if (!src) return NULL;
  size_t len = strlen(src) + 1;
  char* copy = malloc(len);
  if (copy) {
      memcpy(copy, src, len);
  }
  return copy;
}

// Version without using strdup
int parse_command_line_from_file(const char* filename, char*** new_argv, int* new_argc, const char* program_name) {
  FILE *input_file;
  char buffer[1024];
  size_t bytes_read;
  char *token_start, *token_end;
  int arg_count = 1; // For argv[0] (program name)
  int i;
  
  // Check if debug logging is enabled
  int enable_logging = (getenv("SHELLGEN_LOG") != NULL);
  
  if (enable_logging) {
      fprintf(stderr, "[PARSE_DEBUG] Opening file: %s\n", filename);
  }
  
  // Open the file
  input_file = fopen(filename, "r");
  if (!input_file) {
      if (enable_logging) {
          fprintf(stderr, "Error opening input file: %s\n", filename);
      }
      return -1;
  }
  
  // Read the file contents
  bytes_read = fread(buffer, 1, sizeof(buffer) - 1, input_file);
  fclose(input_file);
  
  if (enable_logging) {
      fprintf(stderr, "[PARSE_DEBUG] Read %zu bytes\n", bytes_read);
  }
  
  if (bytes_read == 0) {
      if (enable_logging) {
          fprintf(stderr, "Input file is empty or could not be read\n");
      }
      return -1;
  }
  
  buffer[bytes_read] = '\0';  // Null-terminate
  
  if (enable_logging) {
      fprintf(stderr, "[PARSE_DEBUG] File content: '%s'\n", buffer);
  }
  
  // Remove newline characters
  char* newline = strchr(buffer, '\n');
  if (newline) *newline = '\0';
  char* carriage = strchr(buffer, '\r');
  if (carriage) *carriage = '\0';
  
  if (enable_logging) {
      fprintf(stderr, "[PARSE_DEBUG] After newline removal: '%s'\n", buffer);
  }
  
  // Count arguments manually (without strtok)
  char* p = buffer;
  while (*p) {
      // Skip whitespace
      while (*p == ' ' || *p == '\t') p++;
      if (*p == '\0') break;
      
      // Found start of token
      arg_count++;
      
      // Skip to end of token
      while (*p && *p != ' ' && *p != '\t') p++;
  }
  
  if (enable_logging) {
      fprintf(stderr, "[PARSE_DEBUG] Total arguments: %d\n", arg_count);
  }
  
  // Allocate argv array
  *new_argv = (char**)malloc(sizeof(char*) * (arg_count + 1));
  if (*new_argv == NULL) {
      if (enable_logging) {
          fprintf(stderr, "Memory allocation failed\n");
      }
      return -1;
  }
  
  // argv[0] is the actual program name
  (*new_argv)[0] = my_string_copy(program_name);
  if (!(*new_argv)[0]) {
      if (enable_logging) {
          fprintf(stderr, "[PARSE_DEBUG] string copy failed for program_name\n");
      }
      free(*new_argv);
      return -1;
  }
  
  // Parse arguments manually
  p = buffer;
  i = 1;
  while (*p && i < arg_count) {
      // Skip whitespace
      while (*p == ' ' || *p == '\t') p++;
      if (*p == '\0') break;
      
      // Find start and end of token
      token_start = p;
      while (*p && *p != ' ' && *p != '\t') p++;
      token_end = p;
      
      // Calculate token length
      size_t token_len = token_end - token_start;
      
      // Allocate and copy token
      (*new_argv)[i] = malloc(token_len + 1);
      if (!(*new_argv)[i]) {
          if (enable_logging) {
              fprintf(stderr, "[PARSE_DEBUG] malloc failed for token %d\n", i);
          }
          // Cleanup
          for (int j = 0; j < i; j++) {
              free((*new_argv)[j]);
          }
          free(*new_argv);
          return -1;
      }
      
      memcpy((*new_argv)[i], token_start, token_len);
      (*new_argv)[i][token_len] = '\0';
      
      if (enable_logging) {
          fprintf(stderr, "[PARSE_DEBUG] new_argv[%d] = '%s'\n", i, (*new_argv)[i]);
      }
      i++;
  }
  
  (*new_argv)[arg_count] = NULL; // End with NULL
  *new_argc = arg_count;
  
  if (enable_logging) {
      fprintf(stderr, "[PARSE_DEBUG] Successfully parsed %d arguments\n", arg_count);
  }
  
  return 0;
}

int main(int argc, char* argv[]) {
  // Check if debug logging is enabled
  int enable_logging = (getenv("SHELLGEN_LOG") != NULL);
  
  if (enable_logging) {
      fprintf(stderr, "[DEBUG] main() started\n");
      fprintf(stderr, "[DEBUG] argc = %d\n", argc);
      for (int i = 0; i < argc; i++) {
          fprintf(stderr, "[DEBUG] argv[%d] = '%s'\n", i, argv[i]);
      }
  }
  
  // For AFL++ @@ mode when arguments are provided
  if (argc >= 2) {
      if (enable_logging) {
          fprintf(stderr, "[DEBUG] AFL++ mode: reading from file %s\n", argv[1]);
      }
      
      // Read and process arguments from file
      char **new_argv = NULL;
      int new_argc = 0;
      
      if (parse_command_line_from_file(argv[1], &new_argv, &new_argc, argv[0]) == 0) {
          if (enable_logging) {
              fprintf(stderr, "[DEBUG] Successfully parsed %d arguments from file\n", new_argc);
              for (int i = 0; i < new_argc; i++) {
                  fprintf(stderr, "[DEBUG] new_argv[%d] = '%s'\n", i, new_argv[i]);
              }
              fprintf(stderr, "[DEBUG] Calling original_main with %d arguments\n", new_argc);

              // Also record to log file
              FILE *log_file = fopen("fuzzing_log.txt", "a");
              if (log_file) {
                  time_t current_time = time(NULL);
                  char time_str[64];
                  strftime(time_str, sizeof(time_str), "%Y-%m-%d %H:%M:%S", localtime(&current_time));
                  
                  fprintf(log_file, "[%s] --- Parsed command line arguments from file: %s ---\n", time_str, argv[1]);
                  fprintf(log_file, "[%s] argc = %d\n", time_str, new_argc);
                  for (int i = 0; i < new_argc; i++) {
                      fprintf(log_file, "[%s] argv[%d] = '%s'\n", time_str, i, new_argv[i]);
                  }
                  fprintf(log_file, "[%s] --- End of command line arguments ---\n\n", time_str);
                  fclose(log_file);
              }
              
          }
          
          // Call original_main directly
          //return original_main(new_argc, new_argv);
          
          int result = original_main(new_argc, new_argv);
          if (new_argv) {
            for (int i = 0; i < new_argc; i++) {
                if (new_argv[i]) {
                    free(new_argv[i]);
                }
            }
            free(new_argv);
          }
          return result;
        
      } else {
          if (enable_logging) {
              fprintf(stderr, "[DEBUG] Failed to parse arguments from file\n");
          }
          return 1;
      }
  }
  
  // For normal processing when no arguments are provided
  if (enable_logging) {
      fprintf(stderr, "[DEBUG] Normal mode: calling original_main directly\n");
  }
  
  return original_main(argc, argv);
}

