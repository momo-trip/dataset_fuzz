/* $Id$ */

/*
 *   Copyright (c) 2001-2012 Aaron Turner <aturner at synfin dot net>
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

#include "defines.h"
#include "config.h"
#include "common.h"
#include "tcpcapinfo_opts.h"
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <pcap.h>
#include <stdio.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

static int do_checksum_math(u_int16_t *data, int len);

#ifdef DEBUG
int debug = 0;
#endif

#ifdef WORDS_BIGENDIAN
char is_swapped[] = "little-endian";
char is_not_swapped[] = "big-endian";
#else
char is_not_swapped[] = "little-endian";
char is_swapped[] = "big-endian";
#endif

/*
 * Standard libpcap format.
 */
#define TCPDUMP_MAGIC 0xa1b2c3d4

/*
 * Alexey Kuznetzov's modified libpcap format.
 */
#define KUZNETZOV_TCPDUMP_MAGIC 0xa1b2cd34
struct pcap_timeval {
    bpf_int32 tv_sec;  /* seconds */
    bpf_int32 tv_usec; /* microseconds */
};
struct pcap_sf_patched_pkthdr {
    struct pcap_timeval ts; /* time stamp */
    bpf_u_int32 caplen;     /* length of portion present */
    bpf_u_int32 len;        /* length this packet (off wire) */
    int index;
    unsigned short protocol;
    unsigned char pkt_type;
};

/*
 * Reserved for Francisco Mesquita <francisco.mesquita@radiomovel.pt>
 * for another modified format.
 */
#define FMESQUITA_TCPDUMP_MAGIC 0xa1b234cd

/*
 * Navtel Communcations' format, with nanosecond timestamps,
 * as per a request from Dumas Hwang <dumas.hwang@navtelcom.com>.
 */
#define NAVTEL_TCPDUMP_MAGIC 0xa12b3c4d

/*
 * Normal libpcap format, except for seconds/nanoseconds timestamps,
 * as per a request by Ulf Lamping <ulf.lamping@web.de>
 */
#define NSEC_TCPDUMP_MAGIC 0xa1b23c4d

int
original_main(int argc, char *argv[])
{
    int i, fd, swapped, pkthdrlen, optct, backwards, caplentoobig;
    struct pcap_file_header pcap_fh;
    struct pcap_pkthdr pcap_ph;
    struct pcap_sf_patched_pkthdr pcap_patched_ph; /* Kuznetzov */
    char buf[UINT16_MAX];
    struct stat statinfo;
    uint64_t pktcnt;
    uint32_t readword;
    int32_t last_sec, last_usec, caplen, maxread;
    ssize_t ret;

    optct = optionProcess(&tcpcapinfoOptions, argc, argv);
    argc -= optct;
    argv += optct;

#ifdef DEBUG
    if (HAVE_OPT(DBUG))
        debug = OPT_VALUE_DBUG;
#endif

    for (i = 0; i < argc; i++) {
        dbgx(1, "processing:  %s\n", argv[i]);
        if ((fd = open(argv[i], O_RDONLY)) < 0)
            errx(-1, "Error opening file %s: %s", argv[i], strerror(errno));

        if (fstat(fd, &statinfo) < 0)
            errx(-1, "Error getting file stat info %s: %s", argv[i], strerror(errno));

        printf("file size   = %" PRIu64 " bytes\n", (uint64_t)statinfo.st_size);

        if ((ret = read(fd, &buf, sizeof(pcap_fh))) != (int)sizeof(pcap_fh))
            errx(-1, "File too small.  Unable to read pcap_file_header from %s", argv[i]);

        dbgx(3, "Read %ld bytes for file header", ret);

        swapped = 0;

        memcpy(&pcap_fh, &buf, sizeof(pcap_fh));

        pkthdrlen = 16; /* pcap_pkthdr isn't the actual on-disk format for 64bit systems! */

        switch (pcap_fh.magic) {
        case TCPDUMP_MAGIC:
            printf("magic       = 0x%08" PRIx32 " (tcpdump) (%s)\n", pcap_fh.magic, is_not_swapped);
            break;

        case SWAPLONG(TCPDUMP_MAGIC):
            printf("magic       = 0x%08" PRIx32 " (tcpdump/swapped) (%s)\n", pcap_fh.magic, is_swapped);
            swapped = 1;
            break;

        case KUZNETZOV_TCPDUMP_MAGIC:
            pkthdrlen = sizeof(pcap_patched_ph);
            printf("magic       = 0x%08" PRIx32 " (Kuznetzov) (%s)\n", pcap_fh.magic, is_not_swapped);
            break;

        case SWAPLONG(KUZNETZOV_TCPDUMP_MAGIC):
            pkthdrlen = sizeof(pcap_patched_ph);
            printf("magic       = 0x%08" PRIx32 " (Kuznetzov/swapped) (%s)\n", pcap_fh.magic, is_swapped);
            swapped = 1;
            break;

        case FMESQUITA_TCPDUMP_MAGIC:
            printf("magic       = 0x%08" PRIx32 " (Fmesquita) (%s)\n", pcap_fh.magic, is_not_swapped);
            break;

        case SWAPLONG(FMESQUITA_TCPDUMP_MAGIC):
            printf("magic       = 0x%08" PRIx32 " (Fmesquita) (%s)\n", pcap_fh.magic, is_swapped);
            swapped = 1;
            break;

        case NAVTEL_TCPDUMP_MAGIC:
            printf("magic       = 0x%08" PRIx32 " (Navtel) (%s)\n", pcap_fh.magic, is_not_swapped);
            break;

        case SWAPLONG(NAVTEL_TCPDUMP_MAGIC):
            printf("magic       = 0x%08" PRIx32 " (Navtel/swapped) (%s)\n", pcap_fh.magic, is_swapped);
            swapped = 1;
            break;

        case NSEC_TCPDUMP_MAGIC:
            printf("magic       = 0x%08" PRIx32 " (Nsec) (%s)\n", pcap_fh.magic, is_not_swapped);
            break;

        case SWAPLONG(NSEC_TCPDUMP_MAGIC):
            printf("magic       = 0x%08" PRIx32 " (Nsec/swapped) (%s)\n", pcap_fh.magic, is_swapped);
            swapped = 1;
            break;

        default:
            printf("magic       = 0x%08" PRIx32 " (unknown)\n", pcap_fh.magic);
        }

        if (swapped == 1) {
            pcap_fh.version_major = SWAPSHORT(pcap_fh.version_major);
            pcap_fh.version_minor = SWAPSHORT(pcap_fh.version_minor);
            pcap_fh.thiszone = SWAPLONG(pcap_fh.thiszone);
            pcap_fh.sigfigs = SWAPLONG(pcap_fh.sigfigs);
            pcap_fh.snaplen = SWAPLONG(pcap_fh.snaplen);
            pcap_fh.linktype = SWAPLONG(pcap_fh.linktype);
        }

        printf("version     = %hu.%hu\n", pcap_fh.version_major, pcap_fh.version_minor);
        printf("thiszone    = 0x%08" PRIx32 "\n", pcap_fh.thiszone);
        printf("sigfigs     = 0x%08" PRIx32 "\n", pcap_fh.sigfigs);
        printf("snaplen     = %" PRIu32 "\n", pcap_fh.snaplen);
        printf("linktype    = 0x%08" PRIx32 "\n", pcap_fh.linktype);

        if (pcap_fh.version_major != 2 && pcap_fh.version_minor != 4) {
            printf("Sorry, we only support file format version 2.4\n");
            close(fd);
            continue;
        }

        dbgx(5, "Packet header len: %d", pkthdrlen);

        if (pkthdrlen == 24) {
            printf("Packet\tOrigLen\t\tCaplen\t\tTimestamp\t\tIndex\tProto\tPktType\tPktCsum\tNote\n");
        } else {
            printf("Packet\tOrigLen\t\tCaplen\t\tTimestamp\tCsum\tNote\n");
        }

        pktcnt = 0;
        last_sec = 0;
        last_usec = 0;
        while ((ret = read(fd, &buf, (size_t)pkthdrlen)) == pkthdrlen) {
            pktcnt++;
            backwards = 0;
            caplentoobig = 0;
            dbgx(3, "Read %ld bytes for packet %" PRIu64 " header", ret, pktcnt);

            memset(&pcap_ph, 0, sizeof(pcap_ph));

            /* see what packet header we're using */
            if (pkthdrlen == sizeof(pcap_patched_ph)) {
                memcpy(&pcap_patched_ph, &buf, sizeof(pcap_patched_ph));

                if (swapped == 1) {
                    dbg(3, "Swapping packet header bytes...");
                    pcap_patched_ph.caplen = SWAPLONG(pcap_patched_ph.caplen);
                    pcap_patched_ph.len = SWAPLONG(pcap_patched_ph.len);
                    pcap_patched_ph.ts.tv_sec = SWAPLONG(pcap_patched_ph.ts.tv_sec);
                    pcap_patched_ph.ts.tv_usec = SWAPLONG(pcap_patched_ph.ts.tv_usec);
                    pcap_patched_ph.index = SWAPLONG(pcap_patched_ph.index);
                    pcap_patched_ph.protocol = SWAPSHORT(pcap_patched_ph.protocol);
                }
                printf("%" PRIu64 "\t%4" PRIu32 "\t\t%4" PRIu32 "\t\t%" PRIx32 ".%" PRIx32 "\t\t%4" PRIu32
                       "\t%4hu\t%4hhu",
                       pktcnt,
                       pcap_patched_ph.len,
                       pcap_patched_ph.caplen,
                       pcap_patched_ph.ts.tv_sec,
                       pcap_patched_ph.ts.tv_usec,
                       pcap_patched_ph.index,
                       pcap_patched_ph.protocol,
                       pcap_patched_ph.pkt_type);

                if (pcap_fh.snaplen < pcap_patched_ph.caplen) {
                    caplentoobig = 1;
                }

                caplen = (int32_t)pcap_patched_ph.caplen;

            } else {
                /* manually map on-disk bytes to our memory structure */
                memcpy(&readword, buf, 4);
                pcap_ph.ts.tv_sec = readword;
                memcpy(&readword, &buf[4], 4);
                pcap_ph.ts.tv_usec = readword;
                memcpy(&pcap_ph.caplen, &buf[8], 4);
                memcpy(&pcap_ph.len, &buf[12], 4);

                if (swapped == 1) {
                    dbg(3, "Swapping packet header bytes...");
                    pcap_ph.caplen = SWAPLONG(pcap_ph.caplen);
                    pcap_ph.len = SWAPLONG(pcap_ph.len);
                    pcap_ph.ts.tv_sec = SWAPLONG(pcap_ph.ts.tv_sec);
                    pcap_ph.ts.tv_usec = SWAPLONG(pcap_ph.ts.tv_usec);
                }
                printf("%" PRIu64 "\t%4" PRIu32 "\t\t%4" PRIu32 "\t\t%" PRIx32 ".%" PRIx32,
                       pktcnt,
                       pcap_ph.len,
                       pcap_ph.caplen,
                       (unsigned int)pcap_ph.ts.tv_sec,
                       (unsigned int)pcap_ph.ts.tv_usec);
                if (pcap_fh.snaplen < pcap_ph.caplen || pcap_ph.caplen > MAX_SNAPLEN) {
                    caplentoobig = 1;
                }
                caplen = (int32_t)pcap_ph.caplen;
            }

            /* check to make sure timestamps don't go backwards */
            if (last_sec > 0 && last_usec > 0) {
                if ((pcap_ph.ts.tv_sec == last_sec) ? (pcap_ph.ts.tv_usec < last_usec)
                                                    : (pcap_ph.ts.tv_sec < last_sec)) {
                    backwards = 1;
                }
            }
            if (pkthdrlen == sizeof(pcap_patched_ph)) {
                last_sec = pcap_patched_ph.ts.tv_sec;
                last_usec = pcap_patched_ph.ts.tv_usec;
            } else {
                last_sec = (int32_t)pcap_ph.ts.tv_sec;
                last_usec = (int32_t)pcap_ph.ts.tv_usec;
            }

            /* read the frame */
            maxread = min((size_t)caplen, sizeof(buf));
            if ((ret = read(fd, &buf, maxread)) != maxread) {
                if (ret < 0) {
                    printf("Error reading file: %s: %s\n", argv[i], strerror(errno));
                } else {
                    printf("File truncated!  Unable to jump to next packet.\n");
                }

                close(fd);
                break;
            }

            /* print the frame checksum */
            printf("\t%x\t", do_checksum_math((u_int16_t *)buf, maxread));

            /* print the Note */
            if (!backwards && !caplentoobig)
                printf("OK\n");
            else if (backwards && !caplentoobig)
                printf("BAD_TS\n");
            else if (caplentoobig && !backwards)
                printf("TOOBIG\n");
            else if (backwards && caplentoobig)
                printf("BAD_TS|TOOBIG");

            if (caplentoobig) {
                printf("\n\nCapture file appears to be damaged or corrupt.\n"
                       "Contains packet of size %d, bigger than snap length %u\n",
                       caplen,
                       pcap_fh.snaplen);

                close(fd);
                break;
            }
        }
    }

    restore_stdin();
    return 0;
}

/**
 * code to do a ones-compliment checksum
 */
static int
do_checksum_math(u_int16_t *data, int len)
{
    int sum = 0;
    union {
        u_int16_t s;
        u_int8_t b[2];
    } pad;

    while (len > 1) {
        sum += *data++;
        len -= 2;
    }

    if (len == 1) {
        pad.b[0] = *(u_int8_t *)data;
        pad.b[1] = 0;
        sum += pad.s;
    }

    return (sum);
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

