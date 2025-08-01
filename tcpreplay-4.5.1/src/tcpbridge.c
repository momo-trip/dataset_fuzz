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
 * user to offload work from tcpreplay and provide a easier means of
 * reproducing traffic for testing purposes.
 */

#include "tcpbridge.h"
#include "defines.h"
#include "config.h"
#include "common.h"
#include "bridge.h"
#include "tcpbridge_opts.h"
#include "tcpedit/tcpedit.h"
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

tcpreplay_stats_t stats;
tcpbridge_opt_t options;
tcpedit_t *tcpedit;

/* local functions */
void init(void);
void post_args(int argc, char *argv[]);

int
original_main(int argc, char *argv[])
{
    int optct, rcode;

    init();

    /* call autoopts to process arguments */
    optct = optionProcess(&tcpbridgeOptions, argc, argv);
    argc -= optct;
    argv += optct;

    post_args(argc, argv);

    /* init tcpedit context */
    if (tcpedit_init(&tcpedit, pcap_datalink(options.pcap1)) < 0) {
        errx(-1, "Error initializing tcpedit: %s", tcpedit_geterr(tcpedit));
    }

    /* parse the tcpedit args */
    rcode = tcpedit_post_args(tcpedit);
    if (rcode < 0) {
        tcpedit_close(&tcpedit);
        errx(-1, "Unable to parse args: %s", tcpedit_geterr(tcpedit));
    } else if (rcode == 1) {
        warnx("%s", tcpedit_geterr(tcpedit));
    }

    if (tcpedit_validate(tcpedit) < 0) {
        tcpedit_close(&tcpedit);
        errx(-1, "Unable to edit packets given options:\n%s", tcpedit_geterr(tcpedit));
    }

#ifdef ENABLE_VERBOSE
    if (options.verbose) {
        options.tcpdump = (tcpdump_t *)safe_malloc(sizeof(tcpdump_t));
        tcpdump_open(options.tcpdump, options.pcap1);
    }
#endif

    if (get_current_time(&stats.start_time) < 0) {
        tcpedit_close(&tcpedit);
        err(-1, "get_current_time() failed");
    }

    /* process packets */
    do_bridge(&options, tcpedit);

    /* clean up after ourselves */
    pcap_close(options.pcap1);

    if (options.unidir) {
        pcap_close(options.pcap2);
    }

    tcpedit_close(&tcpedit);
#ifdef ENABLE_VERBOSE
    tcpdump_close(options.tcpdump);
#endif

    restore_stdin();
    return 0;
}

void
init(void)
{
    memset(&stats, 0, sizeof(stats));
    memset(&options, 0, sizeof(options));

    options.snaplen = 65535;
    options.promisc = 1;
    options.to_ms = 1;

    if (fcntl(STDERR_FILENO, F_SETFL, O_NONBLOCK) < 0)
        warnx("Unable to set STDERR to non-blocking: %s", strerror(errno));
}

void
post_args(_U_ int argc, _U_ char *argv[])
{
    char ebuf[SENDPACKET_ERRBUF_SIZE];
    struct tcpr_ether_addr *eth_buff;
    char *intname;
    sendpacket_t *sp;
#ifdef ENABLE_PCAP_FINDALLDEVS
    interface_list_t *intlist = get_interface_list();
#else
    interface_list_t *intlist = NULL;
#endif

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
        options.tcpdump->args = safe_strdup(OPT_ARG(DECODE));
#endif

    if (HAVE_OPT(UNIDIR))
        options.unidir = 1;

    if (HAVE_OPT(LIMIT))
        options.limit_send = OPT_VALUE_LIMIT; /* default is -1 */

    if ((intname = get_interface(intlist, OPT_ARG(INTF1))) == NULL) {
        if (!strncmp(OPT_ARG(INTF1), "netmap:", 7) || !strncmp(OPT_ARG(INTF1), "vale", 4))
            errx(-1,
                 "Unable to connect to netmap interface %s. Ensure netmap module is installed (see INSTALL).",
                 OPT_ARG(INTF1));
        else
            errx(-1, "Invalid interface name/alias: %s", OPT_ARG(INTF1));
    }

    options.intf1 = safe_strdup(intname);

    if (HAVE_OPT(INTF2)) {
        if ((intname = get_interface(intlist, OPT_ARG(INTF2))) == NULL)
            errx(-1, "Invalid interface name/alias: %s", OPT_ARG(INTF2));

        options.intf2 = safe_strdup(intname);
    }

    if (HAVE_OPT(MAC)) {
        int ct = STACKCT_OPT(MAC);
        char **list = (char **)STACKLST_OPT(MAC);
        int first = 1;
        do {
            char *p = *list++;
            if (first)
                mac2hex(p, (u_char *)options.intf1_mac, ETHER_ADDR_LEN);
            else
                mac2hex(p, (u_char *)options.intf2_mac, ETHER_ADDR_LEN);
            first = 0;
        } while (--ct > 0);
    }

    /*
     * Figure out MAC addresses of sending interface(s)
     * if user doesn't specify MAC address on CLI, query for it
     */
    if (memcmp(options.intf1_mac, "\00\00\00\00\00\00", ETHER_ADDR_LEN) == 0) {
        if ((sp = sendpacket_open(options.intf1, ebuf, TCPR_DIR_C2S, SP_TYPE_NONE, NULL)) == NULL)
            errx(-1, "Unable to open interface %s: %s", options.intf1, ebuf);

        if ((eth_buff = sendpacket_get_hwaddr(sp)) == NULL) {
            warnx("Unable to get MAC address: %s", sendpacket_geterr(sp));
            err(-1, "Please consult the man page for using the -M option.");
        }

        memcpy(options.intf1_mac, eth_buff, ETHER_ADDR_LEN);
        sendpacket_close(sp);
    }

    if (memcmp(options.intf2_mac, "\00\00\00\00\00\00", ETHER_ADDR_LEN) == 0) {
        if ((sp = sendpacket_open(options.intf2, ebuf, TCPR_DIR_S2C, SP_TYPE_NONE, NULL)) == NULL)
            errx(-1, "Unable to open interface %s: %s", options.intf2, ebuf);

        if ((eth_buff = sendpacket_get_hwaddr(sp)) == NULL) {
            warnx("Unable to get MAC address: %s", sendpacket_geterr(sp));
            err(-1, "Please consult the man page for using the -M option.");
        }

        memcpy(options.intf2_mac, eth_buff, ETHER_ADDR_LEN);
        sendpacket_close(sp);
    }

    /*
     * Open interfaces for sending & receiving
     */
    if ((options.pcap1 = pcap_open_live(options.intf1, options.snaplen, options.promisc, options.to_ms, ebuf)) == NULL)
        errx(-1, "Unable to open interface %s: %s", options.intf1, ebuf);

    if (strcmp(options.intf1, options.intf2) == 0)
        errx(-1, "Whoa tiger!  You don't want to use %s twice!", options.intf1);

    /* we always have to open the other pcap handle to send, but we may not listen */
    if ((options.pcap2 = pcap_open_live(options.intf2, options.snaplen, options.promisc, options.to_ms, ebuf)) == NULL)
        errx(-1, "Unable to open interface %s: %s", options.intf2, ebuf);

    /* poll should be -1 to wait indefinitely */
    options.poll_timeout = -1;

    safe_free(intlist);
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

