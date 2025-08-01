/******************************************************************************
 * Project:  libtiff tools
 * Purpose:  Mainline for setting metadata in existing TIFF files.
 * Author:   Frank Warmerdam, warmerdam@pobox.com
 *
 ******************************************************************************
 * Copyright (c) 2000, Frank Warmerdam
 *
 * Permission to use, copy, modify, distribute, and sell this software and
 * its documentation for any purpose is hereby granted without fee, provided
 * that (i) the above copyright notices and this permission notice appear in
 * all copies of the software and related documentation, and (ii) the names of
 * Sam Leffler and Silicon Graphics may not be used in any advertising or
 * publicity relating to the software without the specific, prior written
 * permission of Sam Leffler and Silicon Graphics.
 *
 * THE SOFTWARE IS PROVIDED "AS-IS" AND WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS, IMPLIED OR OTHERWISE, INCLUDING WITHOUT LIMITATION, ANY
 * WARRANTY OF MERCHANTABILITY OR FITNESS FOR A PARTICULAR PURPOSE.
 *
 * IN NO EVENT SHALL SAM LEFFLER OR SILICON GRAPHICS BE LIABLE FOR
 * ANY SPECIAL, INCIDENTAL, INDIRECT OR CONSEQUENTIAL DAMAGES OF ANY KIND,
 * OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS,
 * WHETHER OR NOT ADVISED OF THE POSSIBILITY OF DAMAGE, AND ON ANY THEORY OF
 * LIABILITY, ARISING OUT OF OR IN CONNECTION WITH THE USE OR PERFORMANCE
 * OF THIS SOFTWARE.
 ******************************************************************************
 */

#include "tif_config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tiffio.h"

#ifdef NEED_LIBPORT
#include "libport.h"
#endif

#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif
#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif

/*
  Visual Studio does not support _fseeki64 and _ftelli64 until the 2005 release.
  Without these interfaces, files over 2GB in size are not supported for
  Windows.
  For MinGW, __MSVCRT_VERSION__ must be at least 0x800 to expose these
  interfaces. The MinGW compiler must support the requested version.  MinGW
  does not distribute the CRT (it is supplied by Microsoft) so the correct CRT
  must be available on the target computer in order for the program to run.
*/
#if defined(_WIN32) && !(defined(_MSC_VER) && _MSC_VER < 1400) &&              \
    !(defined(__MSVCRT_VERSION__) && __MSVCRT_VERSION__ < 0x800)
#define TIFFfseek(stream, offset, whence)                                      \
    _fseeki64(stream, /* __int64 */ offset, whence)
#define TIFFftell(stream) /* __int64 */ _ftelli64(stream)
#pragma message("...... _fseeki64 defined ....")
#else
#define TIFFfseek(stream, offset, whence) fseek(stream, offset, whence)
#define TIFFftell(stream) ftell(stream)
#endif

#define DEFAULT_MAX_MALLOC (256 * 1024 * 1024)

/* malloc size limit (in bytes)
 * disabled when set to 0 */
static tmsize_t maxMalloc = DEFAULT_MAX_MALLOC;

static const char usageMsg[] =
    "Set the value of a TIFF header to a specified value\n\n"
    "usage: tiffset [options] filename\n"
    "where options are:\n"
    " -s  <tagname> [count] <value>...   set the tag value\n"
    " -u  <tagname> to unset the tag\n"
    " -d  <dirno> set the directory\n"
    " -sd <diroff> set the subdirectory\n"
    " -sf <tagname> <filename>  read the tag value from file (for ASCII tags "
    "only)\n"
    " -m  <size>  set maximum memory allocation size (MiB). 0 to disable "
    "limit. Must be first parameter.\n"
    " -h  this help screen\n"
    " The options can be repeated and are processed sequentially.\n";

static void usage(int code)
{
    FILE *out = (code == EXIT_SUCCESS) ? stdout : stderr;

    fprintf(out, "%s\n\n", TIFFGetVersion());
    fprintf(out, "%s", usageMsg);
    exit(code);
}

static const TIFFField *GetField(TIFF *tiff, const char *tagname)
{
    const TIFFField *fip;

    if (atoi(tagname) > 0)
        fip = TIFFFieldWithTag(tiff, (ttag_t)atoi(tagname));
    else
        fip = TIFFFieldWithName(tiff, tagname);

    if (!fip)
    {
        fprintf(stderr, "Field name \"%s\" is not recognised.\n", tagname);
        return (TIFFField *)NULL;
    }

    return fip;
}

/**
 * This custom malloc function enforce a maximum allocation size
 */
static void *limitMalloc(tmsize_t s)
{
    if (maxMalloc && (s > maxMalloc))
    {
        fprintf(stderr,
                "MemoryLimitError: allocation of %" TIFF_SSIZE_FORMAT
                " bytes is forbidden. Limit is %" TIFF_SSIZE_FORMAT ".\n",
                s, maxMalloc);
        fprintf(stderr, "                  use -m option to change limit.\n");
        return NULL;
    }
    return _TIFFmalloc(s);
}

int original_main(int argc, char *argv[])
{
    TIFF *tiff;
    int arg_index;

    if (argc < 2)
        usage(EXIT_FAILURE);

    tiff = TIFFOpen(argv[argc - 1], "r+");
    if (tiff == NULL)
        return EXIT_FAILURE;

    for (arg_index = 1; arg_index < argc - 1; arg_index++)
    {
        if (strcmp(argv[arg_index], "-d") == 0 && arg_index < argc - 2)
        {
            arg_index++;
            if (TIFFSetDirectory(tiff, atoi(argv[arg_index])) != 1)
            {
                fprintf(stderr, "Failed to set directory=%s\n",
                        argv[arg_index]);
                return EXIT_FAILURE;
            }
            arg_index++;
        }
        if (strcmp(argv[arg_index], "-sd") == 0 && arg_index < argc - 2)
        {
            arg_index++;
            if (TIFFSetSubDirectory(tiff, atoi(argv[arg_index])) != 1)
            {
                fprintf(stderr, "Failed to set sub directory=%s\n",
                        argv[arg_index]);
                return EXIT_FAILURE;
            }
            arg_index++;
        }
        /* Add unset option to tiffset -- Zach Baker (niquil@niquil.net)
         * 11/14/2012 */
        if (strcmp(argv[arg_index], "-u") == 0 && arg_index < argc - 2)
        {
            const TIFFField *fip;
            const char *tagname;
            arg_index++;
            tagname = argv[arg_index];
            fip = GetField(tiff, tagname);
            if (!fip)
                return EXIT_FAILURE;

            if (TIFFUnsetField(tiff, TIFFFieldTag(fip)) != 1)
            {
                fprintf(stderr, "Failed to unset %s\n", TIFFFieldName(fip));
            }
            arg_index++;
        }
        else if (strcmp(argv[arg_index], "-s") == 0 && arg_index < argc - 3)
        {
            const TIFFField *fip;
            const char *tagname;

            arg_index++;
            tagname = argv[arg_index];
            fip = GetField(tiff, tagname);

            if (!fip)
                return 3;

            arg_index++;
            if (TIFFFieldDataType(fip) == TIFF_ASCII)
            {
                if (TIFFFieldPassCount(fip))
                {
                    size_t len;
                    len = strlen(argv[arg_index]) + 1;
                    if (len > UINT16_MAX ||
                        TIFFSetField(tiff, TIFFFieldTag(fip), (uint16_t)len,
                                     argv[arg_index]) != 1)
                        fprintf(stderr, "Failed to set %s=%s\n",
                                TIFFFieldName(fip), argv[arg_index]);
                }
                else
                {
                    if (TIFFSetField(tiff, TIFFFieldTag(fip),
                                     argv[arg_index]) != 1)
                        fprintf(stderr, "Failed to set %s=%s\n",
                                TIFFFieldName(fip), argv[arg_index]);
                }
            }
            else if (TIFFFieldWriteCount(fip) > 0 ||
                     TIFFFieldWriteCount(fip) == TIFF_VARIABLE)
            {
                int ret = 1;
                short wc;

                if (TIFFFieldWriteCount(fip) == TIFF_VARIABLE)
                    wc = atoi(argv[arg_index++]);
                else
                    wc = TIFFFieldWriteCount(fip);

                if (argc - arg_index < wc)
                {
                    fprintf(stderr,
                            "Number of tag values is not enough. "
                            "Expected %d values for %s tag, got %d\n",
                            wc, TIFFFieldName(fip), argc - arg_index);
                    return EXIT_FAILURE;
                }

                if (wc > 1 || TIFFFieldWriteCount(fip) == TIFF_VARIABLE)
                {
                    int i, size;
                    void *array;

                    switch (TIFFFieldDataType(fip))
                    {
                        /*
                         * XXX: We can't use TIFFDataWidth()
                         * to determine the space needed to store
                         * the value. For TIFF_RATIONAL values
                         * TIFFDataWidth() returns 8, but we use 4-byte
                         * float to represent rationals.
                         */
                        case TIFF_BYTE:
                        case TIFF_ASCII:
                        case TIFF_SBYTE:
                        case TIFF_UNDEFINED:
                        default:
                            size = 1;
                            break;

                        case TIFF_SHORT:
                        case TIFF_SSHORT:
                            size = 2;
                            break;

                        case TIFF_LONG:
                        case TIFF_SLONG:
                        case TIFF_FLOAT:
                        case TIFF_IFD:
                        case TIFF_RATIONAL:
                        case TIFF_SRATIONAL:
                            size = 4;
                            break;

                        case TIFF_LONG8:
                        case TIFF_SLONG8:
                        case TIFF_IFD8:
                        case TIFF_DOUBLE:
                            size = 8;
                            break;
                    }

                    array = limitMalloc((tmsize_t)wc * size);
                    if (!array)
                    {
                        fprintf(stderr, "No space for %s tag\n", tagname);
                        return EXIT_FAILURE;
                    }

                    switch (TIFFFieldDataType(fip))
                    {
                        case TIFF_BYTE:
                            for (i = 0; i < wc; i++)
                                ((uint8_t *)array)[i] =
                                    atoi(argv[arg_index + i]);
                            break;
                        case TIFF_SHORT:
                            for (i = 0; i < wc; i++)
                                ((uint16_t *)array)[i] =
                                    atoi(argv[arg_index + i]);
                            break;
                        case TIFF_SBYTE:
                            for (i = 0; i < wc; i++)
                                ((int8_t *)array)[i] =
                                    atoi(argv[arg_index + i]);
                            break;
                        case TIFF_SSHORT:
                            for (i = 0; i < wc; i++)
                                ((int16_t *)array)[i] =
                                    atoi(argv[arg_index + i]);
                            break;
                        case TIFF_LONG:
                            for (i = 0; i < wc; i++)
                                ((uint32_t *)array)[i] =
                                    atol(argv[arg_index + i]);
                            break;
                        case TIFF_SLONG:
                        case TIFF_IFD:
                            for (i = 0; i < wc; i++)
                                ((int32_t *)array)[i] =
                                    atol(argv[arg_index + i]);
                            break;
                        case TIFF_LONG8:
                            for (i = 0; i < wc; i++)
                                ((uint64_t *)array)[i] = strtoll(
                                    argv[arg_index + i], (char **)NULL, 10);
                            break;
                        case TIFF_SLONG8:
                        case TIFF_IFD8:
                            for (i = 0; i < wc; i++)
                                ((int64_t *)array)[i] = strtoll(
                                    argv[arg_index + i], (char **)NULL, 10);
                            break;
                        case TIFF_DOUBLE:
                            for (i = 0; i < wc; i++)
                                ((double *)array)[i] =
                                    atof(argv[arg_index + i]);
                            break;
                        case TIFF_RATIONAL:
                        case TIFF_SRATIONAL:
                        case TIFF_FLOAT:
                            for (i = 0; i < wc; i++)
                                ((float *)array)[i] =
                                    (float)atof(argv[arg_index + i]);
                            break;
                        default:
                            break;
                    }

                    if (TIFFFieldPassCount(fip))
                    {
                        ret = TIFFSetField(tiff, TIFFFieldTag(fip), wc, array);
                    }
                    else if (TIFFFieldTag(fip) == TIFFTAG_PAGENUMBER ||
                             TIFFFieldTag(fip) == TIFFTAG_HALFTONEHINTS ||
                             TIFFFieldTag(fip) == TIFFTAG_YCBCRSUBSAMPLING ||
                             TIFFFieldTag(fip) == TIFFTAG_DOTRANGE)
                    {
                        if (TIFFFieldDataType(fip) == TIFF_BYTE)
                        {
                            ret = TIFFSetField(tiff, TIFFFieldTag(fip),
                                               ((uint8_t *)array)[0],
                                               ((uint8_t *)array)[1]);
                        }
                        else if (TIFFFieldDataType(fip) == TIFF_SHORT)
                        {
                            ret = TIFFSetField(tiff, TIFFFieldTag(fip),
                                               ((uint16_t *)array)[0],
                                               ((uint16_t *)array)[1]);
                        }
                    }
                    else
                    {
                        ret = TIFFSetField(tiff, TIFFFieldTag(fip), array);
                    }

                    _TIFFfree(array);
                }
                else
                {
                    switch (TIFFFieldDataType(fip))
                    {
                        case TIFF_BYTE:
                        case TIFF_SHORT:
                        case TIFF_SBYTE:
                        case TIFF_SSHORT:
                            ret = TIFFSetField(tiff, TIFFFieldTag(fip),
                                               atoi(argv[arg_index++]));
                            break;
                        case TIFF_LONG:
                        case TIFF_SLONG:
                        case TIFF_IFD:
                            ret = TIFFSetField(tiff, TIFFFieldTag(fip),
                                               atol(argv[arg_index++]));
                            break;
                        case TIFF_LONG8:
                        case TIFF_SLONG8:
                        case TIFF_IFD8:
                            ret = TIFFSetField(
                                tiff, TIFFFieldTag(fip),
                                strtoll(argv[arg_index++], (char **)NULL, 10));
                            break;
                        case TIFF_DOUBLE:
                            ret = TIFFSetField(tiff, TIFFFieldTag(fip),
                                               atof(argv[arg_index++]));
                            break;
                        case TIFF_RATIONAL:
                        case TIFF_SRATIONAL:
                        case TIFF_FLOAT:
                            ret = TIFFSetField(tiff, TIFFFieldTag(fip),
                                               (float)atof(argv[arg_index++]));
                            break;
                        default:
                            break;
                    }
                }

                if (ret != 1)
                    fprintf(stderr, "Failed to set %s\n", TIFFFieldName(fip));
                arg_index += wc;
            }
        }
        else if (strcmp(argv[arg_index], "-sf") == 0 && arg_index < argc - 3)
        {
            FILE *fp;
            const TIFFField *fip;
            char *text;
            size_t len;
            int ret;
            int64_t fsize;

            arg_index++;
            fip = GetField(tiff, argv[arg_index]);

            if (!fip)
                return EXIT_FAILURE;

            if (TIFFFieldDataType(fip) != TIFF_ASCII)
            {
                fprintf(stderr,
                        "Only ASCII tags can be set from file. "
                        "%s is not ASCII tag.\n",
                        TIFFFieldName(fip));
                return EXIT_FAILURE;
            }

            arg_index++;
            fp = fopen(argv[arg_index], "rt");
            if (fp == NULL)
            {
                perror(argv[arg_index]);
                continue;
            }

            /* Get file size and enlarge it for some space in buffer.
             * Maximum ASCII tag size is limited by uint32_t count.
             */
            TIFFfseek(fp, 0L, SEEK_END);
            fsize = TIFFftell(fp) + 1;
            rewind(fp);

            /* for x32 tmsize_t is only int32_t. The - 1 is just here to make
             * Coverity Scan happy on 64 bit builds where the condition would
             * be always true otherwise.
             */
            if (fsize > TIFF_TMSIZE_T_MAX - 1 || fsize <= 0)
            {
                fprintf(
                    stderr,
                    "Contents of %s is too large to store in an ASCII tag.\n",
                    argv[arg_index]);
                fclose(fp);
                continue;
            }
            text = (char *)limitMalloc((tmsize_t)fsize);
            if (text == NULL)
            {
                fprintf(stderr, "Memory allocation error\n");
                fclose(fp);
                continue;
            }
            len = fread(text, 1, (size_t)(fsize - 1), fp);
            text[len] = '\0';

            fclose(fp);

            if (TIFFFieldPassCount(fip))
            {
                ret =
                    TIFFSetField(tiff, TIFFFieldTag(fip), (uint16_t)len, text);
            }
            else
            {
                ret = TIFFSetField(tiff, TIFFFieldTag(fip), text);
            }
            if (!ret)
            {
                fprintf(stderr, "Failed to set %s from file %s\n",
                        TIFFFieldName(fip), argv[arg_index]);
            }

            _TIFFfree(text);
            arg_index++;
        }
        else if (strcmp(argv[arg_index], "-m") == 0)
        {
            arg_index++;
            maxMalloc = (tmsize_t)strtoul(argv[arg_index], NULL, 0) << 20;
        }
        else if (strcmp(argv[arg_index], "-h") == 0 ||
                 strcmp(argv[arg_index], "--help") == 0)
        {
            usage(EXIT_SUCCESS);
        }
        else
        {
            fprintf(stderr,
                    "Unrecognised option: %s  or too few parameters left "
                    "for this option.\n",
                    argv[arg_index]);
            usage(EXIT_FAILURE);
        }
    }

    TIFFRewriteDirectory(tiff);
    TIFFClose(tiff);
    return EXIT_SUCCESS;
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

