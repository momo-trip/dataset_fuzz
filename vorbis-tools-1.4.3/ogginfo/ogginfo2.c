/* Ogginfo
 *
 * A tool to describe ogg file contents and metadata.
 *
 * Copyright 2002-2005 Michael Smith <msmith@xiph.org>
 * Copyright 2020-2021 Philipp Schafft <lion@lion.leolix.org>
 * Licensed under the GNU GPL, distributed with this program.
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include <getopt.h>
#include <math.h>
#include <inttypes.h>

#include <ogg/ogg.h>

#include <locale.h>
#include "utf8.h"
#include "i18n.h"

#include "private.h"

#define CHUNK 4500

/* TODO:
 *
 * - detect violations of muxing constraints
 * - detect granulepos 'gaps' (possibly vorbis-specific). (seperate from
 *   serial-number gaps)
 */

typedef struct {
    stream_processor *streams;
    int allocated;
    int used;

    int in_headers;
} stream_set;

int printlots = 0;
static int printinfo = 1;
static int printwarn = 1;
static int verbose = 1;

static int flawed;

#define CONSTRAINT_PAGE_AFTER_EOS   1
#define CONSTRAINT_MUXING_VIOLATED  2

static stream_set *create_stream_set(void) {
    stream_set *set = calloc(1, sizeof(stream_set));

    set->streams = calloc(5, sizeof(stream_processor));
    set->allocated = 5;
    set->used = 0;

    return set;
}

void info(const char *format, ...)
{
    va_list ap;

    if (!printinfo)
        return;

    va_start(ap, format);
    vfprintf(stdout, format, ap);
    va_end(ap);
}

void warn(const char *format, ...)
{
    va_list ap;

    flawed = 1;
    if (!printwarn)
        return;

    va_start(ap, format);
    vfprintf(stdout, format, ap);
    va_end(ap);
}

void error(const char *format, ...)
{
    va_list ap;

    flawed = 1;

    va_start(ap, format);
    vfprintf(stdout, format, ap);
    va_end(ap);
}

void print_summary(stream_processor *stream, size_t bytes, double time)
{
    long minutes, seconds, milliseconds;
    double bitrate;

    minutes = (long)time / 60;
    seconds = (long)time - minutes*60;
    milliseconds = (long)((time - minutes*60 - seconds)*1000);
    bitrate = bytes*8 / time / 1000.0;

    info(_("%s stream %d:\n"
           "\tTotal data length: %" PRId64 " bytes\n"
           "\tPlayback length: %ldm:%02ld.%03lds\n"
           "\tAverage bitrate: %f kb/s\n"),
            stream->type, stream->num, bytes, minutes, seconds, milliseconds, bitrate);
}

static void free_stream_set(stream_set *set)
{
    int i;
    for (i=0; i < set->used; i++) {
        if (!set->streams[i].end) {
            warn(_("WARNING: EOS not set on stream %d\n"),
                    set->streams[i].num);
            if (set->streams[i].process_end)
                set->streams[i].process_end(&set->streams[i]);
        }
        ogg_stream_clear(&set->streams[i].os);
    }

    free(set->streams);
    free(set);
}

static int streams_open(stream_set *set)
{
    int i;
    int res=0;
    for (i=0; i < set->used; i++) {
        if (!set->streams[i].end)
            res++;
    }

    return res;
}

static stream_processor *find_stream_processor(stream_set *set, ogg_page *page)
{
    ogg_uint32_t serial = ogg_page_serialno(page);
    int i;
    int invalid = 0;
    int constraint = 0;
    stream_processor *stream;

    for (i=0; i < set->used; i++) {
        if (serial == set->streams[i].serial) {
            /* We have a match! */
            stream = &(set->streams[i]);

            set->in_headers = 0;
            /* if we have detected EOS, then this can't occur here. */
            if (stream->end) {
                stream->isillegal = 1;
                stream->constraint_violated = CONSTRAINT_PAGE_AFTER_EOS;
                return stream;
            }

            stream->isnew = 0;
            stream->start = ogg_page_bos(page);
            stream->end = ogg_page_eos(page);
            stream->serial = serial;
            return stream;
        }
    }

    /* If there are streams open, and we've reached the end of the
     * headers, then we can't be starting a new stream.
     * XXX: might this sometimes catch ok streams if EOS flag is missing,
     * but the stream is otherwise ok?
     */
    if (streams_open(set) && !set->in_headers) {
        constraint = CONSTRAINT_MUXING_VIOLATED;
        invalid = 1;
    }

    set->in_headers = 1;

    if (set->allocated < set->used) {
        stream = &set->streams[set->used];
    } else {
        set->allocated += 5;
        set->streams = realloc(set->streams, sizeof(stream_processor)*
                set->allocated);
        stream = &set->streams[set->used];
    }
    set->used++;
    stream->num = set->used; /* We count from 1 */

    stream->isnew = 1;
    stream->isillegal = invalid;
    stream->constraint_violated = constraint;

    {
        int res;
        ogg_packet packet;

        /* We end up processing the header page twice, but that's ok. */
        ogg_stream_init(&stream->os, serial);
        ogg_stream_pagein(&stream->os, page);
        res = ogg_stream_packetout(&stream->os, &packet);
        if (res <= 0) {
            warn(_("WARNING: Invalid header page, no packet found\n"));
            invalid_start(stream);
        } else if (packet.bytes >= 7 && memcmp(packet.packet, "\x01vorbis", 7)==0) {
            vorbis_start(stream);
        } else if (packet.bytes >= 7 && memcmp(packet.packet, "\x80theora", 7)==0) {
            theora_start(stream);
        } else if (packet.bytes >= 8 && memcmp(packet.packet, "OggMIDI\0", 8)==0) {
            other_start(stream, "MIDI");
        } else if (packet.bytes >= 5 && memcmp(packet.packet, "\177FLAC", 5)==0) {
            flac_start(stream);
        } else if (packet.bytes == 4 && memcmp(packet.packet, "fLaC", 4)==0) {
            other_start(stream, "FLAC (legacy)");
        } else if (packet.bytes >= 8 && memcmp(packet.packet, "Speex   ", 8)==0) {
            speex_start(stream);
        } else if (packet.bytes >= 8 && memcmp(packet.packet, "fishead\0", 8)==0) {
            skeleton_start(stream);
        } else if (packet.bytes >= 5 && memcmp(packet.packet, "BBCD\0", 5)==0) {
            other_start(stream, "dirac");
        } else if (packet.bytes >= 8 && memcmp(packet.packet, "KW-DIRAC", 8)==0) {
            other_start(stream, "dirac (legacy)");
        } else if (packet.bytes >= 8 && memcmp(packet.packet, "OpusHead", 8)==0) {
            opus_start(stream);
        } else if (packet.bytes >= 8 && memcmp(packet.packet, "\x80kate\0\0\0", 8)==0) {
            kate_start(stream);
        } else {
            other_start(stream, NULL);
        }

        res = ogg_stream_packetout(&stream->os, &packet);
        if (res > 0) {
            warn(_("WARNING: Invalid header page in stream %d, "
                              "contains multiple packets\n"), stream->num);
        }

        /* re-init, ready for processing */
        ogg_stream_clear(&stream->os);
        ogg_stream_init(&stream->os, serial);
   }

   stream->start = ogg_page_bos(page);
   stream->end = ogg_page_eos(page);
   stream->serial = serial;

   if (stream->serial == 0 || stream->serial == -1) {
       info(_("Note: Stream %d has serial number %d, which is legal but may "
              "cause problems with some tools.\n"), stream->num,
               stream->serial);
   }

   return stream;
}

static int get_next_page(FILE *f, ogg_sync_state *sync, ogg_page *page,
        ogg_int64_t *written)
{
    int ret;
    char *buffer;
    int bytes;

    while ((ret = ogg_sync_pageseek(sync, page)) <= 0) {
        if (ret < 0) {
            /* unsynced, we jump over bytes to a possible capture - we don't need to read more just yet */
            warn(_("WARNING: Hole in data (%d bytes) found at approximate offset %" PRId64 " bytes. Corrupted Ogg.\n"), -ret, *written);
            continue;
        }

        /* zero return, we didn't have enough data to find a whole page, read */
        buffer = ogg_sync_buffer(sync, CHUNK);
        bytes = fread(buffer, 1, CHUNK, f);
        if (bytes <= 0) {
            ogg_sync_wrote(sync, 0);
            return 0;
        }
        ogg_sync_wrote(sync, bytes);
        *written += bytes;
    }

    return 1;
}

static void process_file(const char *filename) {
    FILE *file = fopen(filename, "rb");
    ogg_sync_state sync;
    ogg_page page;
    stream_set *processors = create_stream_set();
    int gotpage = 0;
    ogg_int64_t written = 0;

    if (!file) {
        error(_("Error opening input file \"%s\": %s\n"), filename,
                    strerror(errno));
        return;
    }

    printf(_("Processing file \"%s\"...\n\n"), filename);

    ogg_sync_init(&sync);

    while (get_next_page(file, &sync, &page, &written)) {
        stream_processor *p = find_stream_processor(processors, &page);
        gotpage = 1;

        if (!p) {
            error(_("Could not find a processor for stream, bailing\n"));
            return;
        }

        if (p->isillegal && !p->shownillegal) {
            char *constraint;
            switch(p->constraint_violated) {
                case CONSTRAINT_PAGE_AFTER_EOS:
                    constraint = _("Page found for stream after EOS flag");
                    break;
                case CONSTRAINT_MUXING_VIOLATED:
                    constraint = _("Ogg muxing constraints violated, new "
                                   "stream before EOS of all previous streams");
                    break;
                default:
                    constraint = _("Error unknown.");
            }

            warn(_("WARNING: illegally placed page(s) for logical stream %d\n"
                   "This indicates a corrupt Ogg file: %s.\n"),
                    p->num, constraint);
            p->shownillegal = 1;
            /* If it's a new stream, we want to continue processing this page
             * anyway to suppress additional spurious errors
             */
            if (!p->isnew)
                continue;
        }

        if (p->isnew) {
            info(_("New logical stream (#%d, serial: %08x): type %s\n"),
                    p->num, p->serial, p->type);
            if (!p->start)
                warn(_("WARNING: stream start flag not set on stream %d\n"),
                        p->num);
        } else if (p->start) {
            warn(_("WARNING: stream start flag found in mid-stream "
                      "on stream %d\n"), p->num);
        }

        if (p->seqno++ != ogg_page_pageno(&page)) {
            if (!p->lostseq)
                warn(_("WARNING: sequence number gap in stream %d. Got page "
                       "%ld when expecting page %ld. Indicates missing data.\n"
                       ), p->num, ogg_page_pageno(&page), p->seqno - 1);
            p->seqno = ogg_page_pageno(&page);
            p->lostseq = 1;
        } else {
            p->lostseq = 0;
        }

        if (!p->isillegal) {
            p->process_page(p, &page);

            if (p->end) {
                if (p->process_end)
                    p->process_end(p);
                info(_("Logical stream %d ended\n"), p->num);
                p->isillegal = 1;
                p->constraint_violated = CONSTRAINT_PAGE_AFTER_EOS;
            }
        }
    }

    if (!gotpage)
        error(_("ERROR: No Ogg data found in file \"%s\".\n"
                "Input probably not Ogg.\n"), filename);

    free_stream_set(processors);

    ogg_sync_clear(&sync);

    fclose(file);
}

static void version (void) {
    printf (_("ogginfo from %s %s\n"), PACKAGE, VERSION);
}

static void usage(void) {
    version ();
    printf (_(" by the Xiph.Org Foundation (https://www.xiph.org/)\n\n"));
    printf(_("(c) 2003-2005 Michael Smith <msmith@xiph.org>\n"
             "\n"
             "Usage: ogginfo [flags] file1.ogg [file2.ogx ... fileN.ogv]\n"
             "Flags supported:\n"
             "\t-h Show this help message\n"
             "\t-q Make less verbose. Once will remove detailed informative\n"
             "\t   messages, two will remove warnings\n"
             "\t-v Make more verbose. This may enable more detailed checks\n"
             "\t   for some stream types.\n"));
    printf (_("\t-V Output version information and exit\n"));
}

int original_main(int argc, char **argv) {
    int f, ret;

    setlocale(LC_ALL, "");
    bindtextdomain(PACKAGE, LOCALEDIR);
    textdomain(PACKAGE);

    if (argc < 2) {
        fprintf(stdout,
                _("Usage: ogginfo [flags] file1.ogg [file2.ogx ... fileN.ogv]\n"
                  "\n"
                  "ogginfo is a tool for printing information about Ogg files\n"
                  "and for diagnosing problems with them.\n"
                  "Full help shown with \"ogginfo -h\".\n"));
        return 1;
    }

    while ((ret = getopt(argc, argv, "hqvV")) >= 0) {
        switch(ret) {
            case 'h':
                usage();
                return 0;
            case 'V':
                version();
                return 0;
            case 'v':
                verbose++;
                break;
            case 'q':
                verbose--;
                break;
        }
    }

    if (verbose > 1)
        printlots = 1;
    if (verbose < 1)
        printinfo = 0;
    if (verbose < 0)
        printwarn = 0;

    if (optind >= argc) {
        fprintf(stderr,
                _("No input files specified. \"ogginfo -h\" for help\n"));
        return 1;
    }

    ret = 0;

    for (f=optind; f < argc; f++) {
        flawed = 0;
        process_file(argv[f]);
        if (flawed != 0)
            ret = flawed;
    }

    convert_free_charset();
    return ret;
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

