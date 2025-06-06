/* OggDec
 *
 * This program is distributed under the GNU General Public License, version 2.
 * A copy of this license is included with this source.
 *
 * Copyright 2002, Michael Smith <msmith@xiph.org>
 *
 */

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <getopt.h>
#include <errno.h>
#include <string.h>
#include <locale.h>

#if defined(_WIN32) || defined(__EMX__) || defined(__WATCOMC__)
#include <fcntl.h>
#include <io.h>
#endif

#include <vorbis/vorbisfile.h>

#include "i18n.h"

struct option long_options[] = {
    {"quiet", no_argument, NULL, 'Q'},
    {"help", no_argument, NULL, 'h'},
    {"version", no_argument, NULL, 'V'},
    {"bits", required_argument, NULL, 'b'},
    {"endianness", required_argument, NULL, 'e'},
    {"raw", no_argument, NULL, 'R'},
    {"sign", required_argument, NULL, 's'},
    {"output", required_argument, NULL, 'o'},
    {NULL,0,NULL,0}
};

static int quiet = 0;
static int bits = 16;
static int endian = 0;
static int raw = 0;
static int sign = 1;
unsigned char headbuf[44]; /* The whole buffer */
char *outfilename = NULL;

static void version (void) {
    fprintf(stderr, _("oggdec from %s %s\n"), PACKAGE, VERSION);
}

static void usage(void)
{
    version ();
    fprintf(stdout, _(" by the Xiph.Org Foundation (https://www.xiph.org/)\n"));
    fprintf(stdout, _(" using decoder %s.\n\n"), vorbis_version_string());
    fprintf(stdout, _("Usage: oggdec [options] file1.ogg [file2.ogg ... fileN.ogg]\n\n"));
    fprintf(stdout, _("Supported options:\n"));
    fprintf(stdout, _(" --quiet, -Q      Quiet mode. No console output.\n"));
    fprintf(stdout, _(" --help,  -h      Produce this help message.\n"));
    fprintf(stdout, _(" --version, -V    Print out version number.\n"));
    fprintf(stdout, _(" --bits, -b       Bit depth for output (8 and 16 supported)\n"));
    fprintf(stdout, _(" --endianness, -e Output endianness for 16-bit output; 0 for\n"
                      "                  little endian (default), 1 for big endian.\n"));
    fprintf(stdout, _(" --sign, -s       Sign for output PCM; 0 for unsigned, 1 for\n"
                      "                  signed (default 1).\n"));
    fprintf(stdout, _(" --raw, -R        Raw (headerless) output.\n"));
    fprintf(stdout, _(" --output, -o     Output to given filename. May only be used\n"
                      "                  if there is only one input file, except in\n"
                      "                  raw mode.\n"));
}

static void parse_options(int argc, char **argv)
{
    int option_index = 1;
    int ret;

    while((ret = getopt_long(argc, argv, "QhVb:e:Rs:o:", 
                    long_options, &option_index)) != -1)
    {
        switch(ret)
        {
            case 'Q':
                quiet = 1;
                break;
            case 'h':
                usage();
                exit(0);
                break;
            case 'V':
                version();
                exit(0);
                break;
            case 's':
                sign = atoi(optarg);
                break;
            case 'b':
                bits = atoi(optarg);
                if(bits <= 8)
                    bits = 8;
                else
                    bits = 16;
                break;
            case 'e':
                endian = atoi(optarg);
                break;
            case 'o':
                outfilename = strdup(optarg);
                break;
            case 'R':
                raw = 1;
                break;
            default:
                fprintf(stderr, _("Internal error: Unrecognised argument\n"));
                break;
        }
    }
}

#define WRITE_U32(buf, x) *(buf)     = (unsigned char)((x)&0xff);\
                          *((buf)+1) = (unsigned char)(((x)>>8)&0xff);\
                          *((buf)+2) = (unsigned char)(((x)>>16)&0xff);\
                          *((buf)+3) = (unsigned char)(((x)>>24)&0xff);

#define WRITE_U16(buf, x) *(buf)     = (unsigned char)((x)&0xff);\
                          *((buf)+1) = (unsigned char)(((x)>>8)&0xff);

/* Some of this based on ao/src/ao_wav.c */
int write_prelim_header(OggVorbis_File *vf, FILE *out, ogg_int64_t knownlength) {
    unsigned int size = 0x7fffffff;
    int channels = ov_info(vf,0)->channels;
    int samplerate = ov_info(vf,0)->rate;
    int bytespersec = channels*samplerate*bits/8;
    int align = channels*bits/8;
    int samplesize = bits;

    if(knownlength && knownlength*bits/8*channels < size)
        size = (unsigned int)(knownlength*bits/8*channels+44) ;

    memcpy(headbuf, "RIFF", 4);
    WRITE_U32(headbuf+4, size-8);
    memcpy(headbuf+8, "WAVE", 4);
    memcpy(headbuf+12, "fmt ", 4);
    WRITE_U32(headbuf+16, 16);
    WRITE_U16(headbuf+20, 1); /* format */
    WRITE_U16(headbuf+22, channels);
    WRITE_U32(headbuf+24, samplerate);
    WRITE_U32(headbuf+28, bytespersec);
    WRITE_U16(headbuf+32, align);
    WRITE_U16(headbuf+34, samplesize);
    memcpy(headbuf+36, "data", 4);
    WRITE_U32(headbuf+40, size - 44);

    if(fwrite(headbuf, 1, 44, out) != 44) {
        fprintf(stderr, _("ERROR: Failed to write Wave header: %s\n"), strerror(errno));
        return 1;
    }

    return 0;
}

int rewrite_header(FILE *out, unsigned int written) 
{
    unsigned int length = written;

    length += 44;

    WRITE_U32(headbuf+4, length-8);
    WRITE_U32(headbuf+40, length-44);
    if(fseek(out, 0, SEEK_SET) != 0)
        return 1;

    if(fwrite(headbuf, 1, 44, out) != 44) {
        fprintf(stderr, _("ERROR: Failed to write Wave header: %s\n"), strerror(errno));
        return 1;
    }
    return 0;
}

static FILE *open_input(char *infile) 
{
    FILE *in;

    if(!infile) {
#ifdef __BORLANDC__
        setmode(fileno(stdin), O_BINARY);
#elif _WIN32
        _setmode(_fileno(stdin), _O_BINARY);
#endif
        in = stdin;
    }
    else {
        in = fopen(infile, "rb");
        if(!in) {
            fprintf(stderr, _("ERROR: Failed to open input file: %s\n"), strerror(errno));
            return NULL;
        }
    }

    return in;
}

static FILE *open_output(char *outfile) 
{
    FILE *out;
    if(!outfile) {
#ifdef __BORLANDC__
        setmode(fileno(stdout), O_BINARY);
#elif _WIN32
        _setmode(_fileno(stdout), _O_BINARY);
#endif
        out = stdout;
    }
    else {
        out = fopen(outfile, "wb");
        if(!out) {
            fprintf(stderr, _("ERROR: Failed to open output file: %s\n"), strerror(errno));
            return NULL;
        }
    }

    return out;
}

static void
permute_channels(char *in, char *out, int len, int channels, int bytespersample)
{
    int permute[6][6] = {{0}, {0,1}, {0,2,1}, {0,1,2,3}, {0,1,2,3,4}, 
        {0,2,1,5,3,4}};
    int i,j,k;
    int samples = len/channels/bytespersample;

    /* Can't handle, don't try */
    if (channels > 6)
        return;

    for (i=0; i < samples; i++) {
        for (j=0; j < channels; j++) {
            for (k=0; k < bytespersample; k++) {
                out[i*bytespersample*channels + 
                    bytespersample*permute[channels-1][j] + k] = 
                    in[i*bytespersample*channels + bytespersample*j + k];
            }
        }
    }
}

static int decode_file(FILE *in, FILE *out, char *infile, char *outfile)
{
    OggVorbis_File vf;
    int bs = 0;
    char buf[8192], outbuf[8192];
    char *p_outbuf;
    int buflen = 8192;
    unsigned int written = 0;
    int ret;
    ogg_int64_t length = 0;
    ogg_int64_t done = 0;
    int size = 0;
    int seekable = 0;
    int percent = 0;
    int channels;
    int samplerate;

    if (ov_open_callbacks(in, &vf, NULL, 0, OV_CALLBACKS_DEFAULT) < 0) {
        fprintf(stderr, _("ERROR: Failed to open input as Vorbis\n"));
        fclose(in);
        return 1;
    }

    channels = ov_info(&vf,0)->channels;
    samplerate = ov_info(&vf,0)->rate;

    if(ov_seekable(&vf)) {
        int link;
        int chainsallowed = 0;
        for(link = 0; link < ov_streams(&vf); link++) {
            if(ov_info(&vf, link)->channels == channels && 
                    ov_info(&vf, link)->rate == samplerate)
            {
                chainsallowed = 1;
            }
        }

        seekable = 1;
        if(chainsallowed)
            length = ov_pcm_total(&vf, -1);
        else
            length = ov_pcm_total(&vf, 0);
        size = bits/8 * channels;
        if(!quiet)
            fprintf(stderr, _("Decoding \"%s\" to \"%s\"\n"), 
                    infile?infile:_("standard input"), 
                    outfile?outfile:_("standard output"));
    }

    if(!raw) {
        if(write_prelim_header(&vf, out, length)) {
            ov_clear(&vf);
            return 1;
        }
    }

    while((ret = ov_read(&vf, buf, buflen, endian, bits/8, sign, &bs)) != 0) {
        if(bs != 0) {
            vorbis_info *vi = ov_info(&vf, -1);
            if(channels != vi->channels || samplerate != vi->rate) {
                fprintf(stderr, _("Logical bitstreams with changing parameters are not supported\n"));
                break;
            }
        }

        if(ret == OV_HOLE) {
            if(!quiet) {
                fprintf(stderr, _("WARNING: hole in data (%d)\n"), ret);
            }
            continue;
        }
        else if(ret < 0) {
            if(!quiet) {
                fprintf(stderr, _("=== Vorbis library reported a stream error.\n"));
            }
            ov_clear(&vf);
            return 1;
        }

        if(channels > 2 && !raw) {
          /* Then permute! */
          permute_channels(buf, outbuf, ret, channels, bits/8);
          p_outbuf = outbuf;
        }
        else {
          p_outbuf = buf;
        }

        if(fwrite(p_outbuf, 1, ret, out) != ret) {
            fprintf(stderr, _("Error writing to file: %s\n"), strerror(errno));
            ov_clear(&vf);
            return 1;
        }

        written += ret;
        if(!quiet && seekable) {
            done += ret/size;
            if((double)done/(double)length * 200. > (double)percent) {
                percent = (int)((double)done/(double)length *200);
                fprintf(stderr, "\r\t[%5.1f%%]", (double)percent/2.);
            }
        }
    }

    if(seekable && !quiet)
        fprintf(stderr, "\n");

    if(!raw)
        rewrite_header(out, written); /* We don't care if it fails, too late */

    ov_clear(&vf);

    return 0;
}

int original_main(int argc, char **argv)
{
    int i;

    setlocale(LC_ALL, "");
    bindtextdomain(PACKAGE, LOCALEDIR);
    textdomain(PACKAGE);

    if(argc == 1) {
        usage();
        return 1;
    }

    parse_options(argc,argv);

    if(!quiet)
        version();

    if(optind >= argc) {
        fprintf(stderr, _("ERROR: No input files specified. Use -h for help\n"));
        return 1;
    }

    if(argc - optind > 1 && outfilename && !raw) {
        fprintf(stderr, _("ERROR: Can only specify one input file if output filename is specified\n"));
        return 1;
    }

    if(outfilename && raw) {
        FILE *infile, *outfile;
        char *infilename;

        if(!strcmp(outfilename, "-")) {
            free(outfilename);
            outfilename = NULL;
        }
        outfile = open_output(outfilename);

        if(!outfile)
            return 1;

        for(i=optind; i < argc; i++) {
            if(!strcmp(argv[i], "-")) {
                infilename = NULL;
                infile = open_input(NULL);
            }
            else {
                infilename = argv[i];
                infile = open_input(argv[i]);
            }

            if(!infile) {
                fclose(outfile);
                free(outfilename);
                return 1;
            }
            if(decode_file(infile, outfile, infilename, outfilename)) {
                fclose(outfile);
                return 1;
            }

        }

        fclose(outfile);
    }
    else {
        for(i=optind; i < argc; i++) {
            char *in, *out;
            FILE *infile, *outfile;

            if(!strcmp(argv[i], "-"))
                in = NULL;
            else
                in = argv[i];

            if(outfilename) {
                if(!strcmp(outfilename, "-"))
                    out = NULL;
                else
                    out = outfilename;
            }
            else {
                if(!strcmp(argv[i], "-")) {
                    out = NULL;
                }
                else {
                    char *end = strrchr(argv[i], '.');
                    end = end?end:(argv[i] + strlen(argv[i]) + 1);

                    out = malloc(strlen(argv[i]) + 10);
                    strncpy(out, argv[i], end-argv[i]);
                    out[end-argv[i]] = 0;
                    if(raw)
                        strcat(out, ".raw");
                    else
                        strcat(out, ".wav");
                }
            }

            infile = open_input(in);
            if(!infile) {
                if(outfilename)
                    free(outfilename);
                return 1;
            }
            outfile = open_output(out);
            if(!outfile) {
                fclose(infile);
                return 1;
            }

            if(decode_file(infile, outfile, in, out)) {
                fclose(outfile);
                return 1;
            }

            if(!outfilename && out)
                free(out);

            fclose(outfile);
        }
    }

    if(outfilename)
        free(outfilename);

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

