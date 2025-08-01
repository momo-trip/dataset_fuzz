/*
 * Copyright (C)2011-2012, 2014-2015, 2017, 2019, 2021-2024
 *           D. R. Commander.  All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * - Neither the name of the libjpeg-turbo Project nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS",
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

/*
 * This program demonstrates how to use the TurboJPEG C API to approximate the
 * functionality of the IJG's jpegtran program.  jpegtran features that are not
 * covered:
 *
 * - Scan scripts
 * - Expanding the input image when cropping
 * - Wiping a region of the input image
 * - Dropping another JPEG image into the input image
 * - Progress reporting
 * - Debug output
 */

#ifdef _MSC_VER
#define _CRT_SECURE_NO_DEPRECATE
#endif

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <limits.h>
#if !defined(_MSC_VER) || _MSC_VER > 1600
#include <stdint.h>
#endif
#include <turbojpeg.h>


#ifdef _WIN32
#define strncasecmp  strnicmp
#endif

#ifndef max
#define max(a, b)  ((a) > (b) ? (a) : (b))
#endif

#define MATCH_ARG(arg, string, minChars) \
  !strncasecmp(arg, string, max(strlen(arg), minChars))

#define IS_CROPPED(cr)  (cr.x != 0 || cr.y != 0 || cr.w != 0 || cr.h != 0)

#define THROW(action, message) { \
  printf("ERROR in line %d while %s:\n%s\n", __LINE__, action, message); \
  retval = -1;  goto bailout; \
}

#define THROW_TJ(action) { \
  int errorCode = tj3GetErrorCode(tjInstance); \
  printf("%s in line %d while %s:\n%s\n", \
         errorCode == TJERR_WARNING ? "WARNING" : "ERROR", __LINE__, action, \
         tj3GetErrorStr(tjInstance)); \
  if (errorCode == TJERR_FATAL || stopOnWarning == 1) { \
    retval = -1;  goto bailout; \
  } \
}

#define THROW_UNIX(action)  THROW(action, strerror(errno))


static void usage(char *programName)
{
  printf("\nUSAGE: %s [options] <JPEG input image> <JPEG output image>\n\n",
         programName);

  printf("This program reads the DCT coefficients from the lossy JPEG input image,\n");
  printf("optionally transforms them, and writes them to a lossy JPEG output image.\n\n");

  printf("OPTIONS (CAN BE ABBREVBIATED)\n");
  printf("-----------------------------\n");
  printf("-arithmetic\n");
  printf("    Use arithmetic entropy coding in the output image instead of Huffman\n");
  printf("    entropy coding (can be combined with -progressive)\n");
  printf("-copy all\n");
  printf("    Copy all extra markers (including comments, JFIF thumbnails, Exif data, and\n");
  printf("    ICC profile data) from the input image to the output image\n");
  printf("-copy comments\n");
  printf("    Do not copy any extra markers, except comment markers, from the input\n");
  printf("    image to the output image [default]\n");
  printf("-copy icc\n");
  printf("    Do not copy any extra markers, except ICC profile data, from the input\n");
  printf("    image to the output image\n");
  printf("-copy none\n");
  printf("    Do not copy any extra markers from the input image to the output image\n");
  printf("-crop WxH+X+Y\n");
  printf("    Include only the specified region of the input image.  (W, H, X, and Y are\n");
  printf("    the width, height, left boundary, and upper boundary of the region, all\n");
  printf("    specified relative to the transformed image dimensions.)  If necessary, X\n");
  printf("    and Y will be shifted up and left to the nearest iMCU boundary, and W and H\n");
  printf("    will be increased accordingly.\n");
  printf("-flip {horizontal|vertical}, -rotate {90|180|270}, -transpose, -transverse\n");
  printf("    Perform the specified lossless transform operation (these options are\n");
  printf("    mutually exclusive)\n");
  printf("-grayscale\n");
  printf("    Create a grayscale output image from a full-color input image\n");
  printf("-icc FILE\n");
  printf("    Embed the ICC (International Color Consortium) color management profile\n");
  printf("    from the specified file into the output image\n");
  printf("-maxmemory N\n");
  printf("    Memory limit (in megabytes) for intermediate buffers used with progressive\n");
  printf("    JPEG compression, Huffman table optimization, and lossless transformation\n");
  printf("    [default = no limit]\n");
  printf("-maxscans N\n");
  printf("    Refuse to transform progressive JPEG images that have more than N scans\n");
  printf("-optimize\n");
  printf("    Use Huffman table optimization in the output image\n");
  printf("-perfect\n");
  printf("    Abort if the requested transform operation is imperfect (non-reversible.)\n");
  printf("    '-flip horizontal', '-rotate 180', '-rotate 270', and '-transverse' are\n");
  printf("    imperfect if the image width is not evenly divisible by the iMCU width.\n");
  printf("    '-flip vertical', '-rotate 90', '-rotate 180', and '-transverse' are\n");
  printf("    imperfect if the image height is not evenly divisible by the iMCU height.\n");
  printf("-progressive\n");
  printf("    Create a progressive output image instead of a single-scan output image\n");
  printf("    (can be combined with -arithmetic; implies -optimize unless -arithmetic is\n");
  printf("    also specified)\n");
  printf("-restart N\n");
  printf("    Add a restart marker every N MCU rows [default = 0 (no restart markers)].\n");
  printf("    Append 'B' to specify the restart marker interval in MCUs.\n");
  printf("-strict\n");
  printf("    Treat all warnings as fatal; abort immediately if incomplete or corrupt\n");
  printf("    data is encountered in the input image, rather than trying to salvage the\n");
  printf("    rest of the image\n");
  printf("-trim\n");
  printf("    If necessary, trim the partial iMCUs at the right or bottom edge of the\n");
  printf("    image to make the requested transform perfect\n\n");

  exit(1);
}


int original_main(int argc, char **argv)
{
  int i, retval = 0;
  int arithmetic = 0, maxMemory = -1, maxScans = -1, optimize = -1,
    progressive = 0, restartIntervalBlocks = -1, restartIntervalRows = -1,
    saveMarkers = 1, stopOnWarning = -1, subsamp;
  tjtransform xform;
  char *iccFilename = NULL;
  tjhandle tjInstance = NULL;
  FILE *iccFile = NULL, *jpegFile = NULL;
  long size = 0;
  size_t srcSize, iccSize, dstSize;
  unsigned char *srcBuf = NULL, *iccBuf = NULL, *dstBuf = NULL;

  memset(&xform, 0, sizeof(tjtransform));

  for (i = 1; i < argc; i++) {
    if (MATCH_ARG(argv[i], "-arithmetic", 2))
      arithmetic = 1;
    else if (MATCH_ARG(argv[i], "-crop", 3) && i < argc - 1) {
      char tempc = -1;

      if (sscanf(argv[++i], "%d%c%d+%d+%d", &xform.r.w, &tempc, &xform.r.h,
                 &xform.r.x, &xform.r.y) != 5 || xform.r.w < 1 ||
          (tempc != 'x' && tempc != 'X') || xform.r.h < 1 || xform.r.x < 0 ||
          xform.r.y < 0)
        usage(argv[0]);
      xform.options |= TJXOPT_CROP;
    } else if (MATCH_ARG(argv[i], "-copy", 2) && i < argc - 1) {
      i++;
      if (MATCH_ARG(argv[i], "all", 1))
        saveMarkers = 2;
      else if (MATCH_ARG(argv[i], "icc", 1))
        saveMarkers = 4;
      else if (MATCH_ARG(argv[i], "none", 1))
        saveMarkers = 0;
      else if (!MATCH_ARG(argv[i], "comments", 1))
        usage(argv[0]);
    } else if (MATCH_ARG(argv[i], "-flip", 2) && i < argc - 1) {
      i++;
      if (MATCH_ARG(argv[i], "horizontal", 1))
        xform.op = TJXOP_HFLIP;
      else if (MATCH_ARG(argv[i], "vertical", 1))
        xform.op = TJXOP_VFLIP;
      else
        usage(argv[0]);
    } else if (MATCH_ARG(argv[i], "-grayscale", 2) ||
               MATCH_ARG(argv[i], "-greyscale", 2))
      xform.options |= TJXOPT_GRAY;
    else if (MATCH_ARG(argv[i], "-icc", 2) && i < argc - 1)
      iccFilename = argv[++i];
    else if (MATCH_ARG(argv[i], "-maxscans", 5) && i < argc - 1) {
      int tempi = atoi(argv[++i]);

      if (tempi < 0) usage(argv[0]);
      maxScans = tempi;
    } else if (MATCH_ARG(argv[i], "-maxmemory", 2) && i < argc - 1) {
      int tempi = atoi(argv[++i]);

      if (tempi < 0) usage(argv[0]);
      maxMemory = tempi;
    } else if (MATCH_ARG(argv[i], "-optimize", 2) ||
               MATCH_ARG(argv[i], "-optimise", 2))
      optimize = 1;
    else if (MATCH_ARG(argv[i], "-perfect", 3))
      xform.options |= TJXOPT_PERFECT;
    else if (MATCH_ARG(argv[i], "-progressive", 2))
      progressive = 1;
    else if (MATCH_ARG(argv[i], "-rotate", 3) && i < argc - 1) {
      i++;
      if (MATCH_ARG(argv[i], "90", 2))
        xform.op = TJXOP_ROT90;
      else if (MATCH_ARG(argv[i], "180", 3))
        xform.op = TJXOP_ROT180;
      else if (MATCH_ARG(argv[i], "270", 3))
        xform.op = TJXOP_ROT270;
      else
        usage(argv[0]);
    } else if (MATCH_ARG(argv[i], "-restart", 2) && i < argc - 1) {
      int tempi = -1, nscan;  char tempc = 0;

      if ((nscan = sscanf(argv[++i], "%d%c", &tempi, &tempc)) < 1 ||
          tempi < 0 || tempi > 65535 ||
          (nscan == 2 && tempc != 'B' && tempc != 'b'))
        usage(argv[0]);

      if (tempc == 'B' || tempc == 'b')
        restartIntervalBlocks = tempi;
      else
        restartIntervalRows = tempi;
    } else if (MATCH_ARG(argv[i], "-strict", 2))
      stopOnWarning = 1;
    else if (MATCH_ARG(argv[i], "-transverse", 7))
      xform.op = TJXOP_TRANSVERSE;
    else if (MATCH_ARG(argv[i], "-trim", 4))
      xform.options |= TJXOPT_TRIM;
    else if (MATCH_ARG(argv[i], "-transpose", 2))
      xform.op = TJXOP_TRANSPOSE;
    else break;
  }

  if (i != argc - 2)
    usage(argv[0]);

  if (iccFilename) {
    if (saveMarkers == 2) saveMarkers = 3;
    else if (saveMarkers == 4) saveMarkers = 0;
  }

  if ((tjInstance = tj3Init(TJINIT_TRANSFORM)) == NULL)
    THROW_TJ("creating TurboJPEG instance");

  if (stopOnWarning >= 0 &&
      tj3Set(tjInstance, TJPARAM_STOPONWARNING, stopOnWarning) < 0)
    THROW_TJ("setting TJPARAM_STOPONWARNING");
  if (optimize >= 0 && tj3Set(tjInstance, TJPARAM_OPTIMIZE, optimize) < 0)
    THROW_TJ("setting TJPARAM_OPTIMIZE");
  if (maxScans >= 0 && tj3Set(tjInstance, TJPARAM_SCANLIMIT, maxScans) < 0)
    THROW_TJ("setting TJPARAM_SCANLIMIT");
  if (restartIntervalBlocks >= 0 &&
      tj3Set(tjInstance, TJPARAM_RESTARTBLOCKS, restartIntervalBlocks) < 0)
    THROW_TJ("setting TJPARAM_RESTARTBLOCKS");
  if (restartIntervalRows >= 0 &&
      tj3Set(tjInstance, TJPARAM_RESTARTROWS, restartIntervalRows) < 0)
    THROW_TJ("setting TJPARAM_RESTARTROWS");
  if (maxMemory >= 0 && tj3Set(tjInstance, TJPARAM_MAXMEMORY, maxMemory) < 0)
    THROW_TJ("setting TJPARAM_MAXMEMORY");
  if (tj3Set(tjInstance, TJPARAM_SAVEMARKERS, saveMarkers) < 0)
    THROW_TJ("setting TJPARAM_SAVEMARKERS");

  if ((jpegFile = fopen(argv[i++], "rb")) == NULL)
    THROW_UNIX("opening input file");
  if (fseek(jpegFile, 0, SEEK_END) < 0 || ((size = ftell(jpegFile)) < 0) ||
      fseek(jpegFile, 0, SEEK_SET) < 0)
    THROW_UNIX("determining input file size");
  if (size == 0)
    THROW("determining input file size", "Input file contains no data");
  srcSize = size;
  if ((srcBuf = tj3Alloc(srcSize)) == NULL)
    THROW_UNIX("allocating JPEG buffer");
  if (fread(srcBuf, srcSize, 1, jpegFile) < 1)
    THROW_UNIX("reading input file");
  fclose(jpegFile);  jpegFile = NULL;

  if (tj3DecompressHeader(tjInstance, srcBuf, srcSize) < 0)
    THROW_TJ("reading JPEG header");
  subsamp = tj3Get(tjInstance, TJPARAM_SUBSAMP);
  if (xform.options & TJXOPT_GRAY)
    subsamp = TJSAMP_GRAY;
  if (xform.op == TJXOP_TRANSPOSE || xform.op == TJXOP_TRANSVERSE ||
      xform.op == TJXOP_ROT90 || xform.op == TJXOP_ROT270) {
    if (subsamp == TJSAMP_422) subsamp = TJSAMP_440;
    else if (subsamp == TJSAMP_440) subsamp = TJSAMP_422;
    else if (subsamp == TJSAMP_411) subsamp = TJSAMP_441;
    else if (subsamp == TJSAMP_441) subsamp = TJSAMP_411;
  }

  if (tj3Set(tjInstance, TJPARAM_PROGRESSIVE, progressive) < 0)
    THROW_TJ("setting TJPARAM_PROGRESSIVE");
  if (tj3Set(tjInstance, TJPARAM_ARITHMETIC, arithmetic) < 0)
    THROW_TJ("setting TJPARAM_ARITHMETIC");

  if (IS_CROPPED(xform.r)) {
    int xAdjust, yAdjust;

    if (subsamp == TJSAMP_UNKNOWN)
      THROW("adjusting cropping region",
            "Could not determine subsampling level of input image");
    xAdjust = xform.r.x % tjMCUWidth[subsamp];
    yAdjust = xform.r.y % tjMCUHeight[subsamp];
    xform.r.x -= xAdjust;
    xform.r.w += xAdjust;
    xform.r.y -= yAdjust;
    xform.r.h += yAdjust;
  }

  if (iccFilename) {
    if ((iccFile = fopen(iccFilename, "rb")) == NULL)
      THROW_UNIX("opening ICC profile");
    if (fseek(iccFile, 0, SEEK_END) < 0 || ((size = ftell(iccFile)) < 0) ||
        fseek(iccFile, 0, SEEK_SET) < 0)
      THROW_UNIX("determining ICC profile size");
    if (size == 0)
      THROW("determining ICC profile size", "ICC profile contains no data");
    iccSize = size;
    if ((iccBuf = (unsigned char *)malloc(iccSize)) == NULL)
      THROW_UNIX("allocating ICC profile buffer");
    if (fread(iccBuf, iccSize, 1, iccFile) < 1)
      THROW_UNIX("reading ICC profile");
    fclose(iccFile);  iccFile = NULL;
    if (tj3SetICCProfile(tjInstance, iccBuf, iccSize) < 0)
      THROW_TJ("setting ICC profile");
    free(iccBuf);  iccBuf = NULL;
  }

  if (tj3Transform(tjInstance, srcBuf, srcSize, 1, &dstBuf, &dstSize,
                   &xform) < 0)
    THROW_TJ("transforming input image");
  tj3Free(srcBuf);  srcBuf = NULL;

  if ((jpegFile = fopen(argv[i], "wb")) == NULL)
    THROW_UNIX("opening output file");
  if (fwrite(dstBuf, dstSize, 1, jpegFile) < 1)
    THROW_UNIX("writing output file");

bailout:
  tj3Destroy(tjInstance);
  tj3Free(srcBuf);
  if (iccFile) fclose(iccFile);
  free(iccBuf);
  if (jpegFile) fclose(jpegFile);
  tj3Free(dstBuf);
  return retval;
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

