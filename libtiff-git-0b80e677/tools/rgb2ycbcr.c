/*
 * Copyright (c) 1991-1997 Sam Leffler
 * Copyright (c) 1991-1997 Silicon Graphics, Inc.
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
 */

#include "libport.h"
#include "tif_config.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "tiffio.h"
#include "tiffiop.h"

#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif
#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif

#define streq(a, b) (strcmp(a, b) == 0)
#define CopyField(tag, v)                                                      \
    if (TIFFGetField(in, tag, &v))                                             \
    TIFFSetField(out, tag, v)

#ifndef howmany
#define howmany(x, y) (((x) + ((y)-1)) / (y))
#endif
#define roundup(x, y) (howmany(x, y) * ((uint32_t)(y)))

#define LumaRed ycbcrCoeffs[0]
#define LumaGreen ycbcrCoeffs[1]
#define LumaBlue ycbcrCoeffs[2]

uint16_t compression = COMPRESSION_PACKBITS;
uint32_t rowsperstrip = (uint32_t)-1;

uint16_t horizSubSampling = 2; /* YCbCr horizontal subsampling */
uint16_t vertSubSampling = 2;  /* YCbCr vertical subsampling */
float ycbcrCoeffs[3] = {.299F, .587F, .114F};
/* default coding range is CCIR Rec 601-1 with no headroom/footroom */
float refBlackWhite[6] = {0.F, 255.F, 128.F, 255.F, 128.F, 255.F};

static int tiffcvt(TIFF *in, TIFF *out);
static void usage(int code);
static void setupLumaTables(void);

int original_main(int argc, char *argv[])
{
    TIFF *in, *out;
    int c;
#if !HAVE_DECL_OPTARG
    extern int optind;
    extern char *optarg;
#endif

    while ((c = getopt(argc, argv, "c:h:r:v:z")) != -1)
        switch (c)
        {
            case 'c':
                if (streq(optarg, "none"))
                    compression = COMPRESSION_NONE;
                else if (streq(optarg, "packbits"))
                    compression = COMPRESSION_PACKBITS;
                else if (streq(optarg, "lzw"))
                    compression = COMPRESSION_LZW;
                else if (streq(optarg, "jpeg"))
                    compression = COMPRESSION_JPEG;
                else if (streq(optarg, "zip"))
                    compression = COMPRESSION_ADOBE_DEFLATE;
                else
                    usage(EXIT_FAILURE);
                break;
            case 'h':
                horizSubSampling = atoi(optarg);
                if (horizSubSampling != 1 && horizSubSampling != 2 &&
                    horizSubSampling != 4)
                    usage(EXIT_FAILURE);
                break;
            case 'v':
                vertSubSampling = atoi(optarg);
                if (vertSubSampling != 1 && vertSubSampling != 2 &&
                    vertSubSampling != 4)
                    usage(EXIT_FAILURE);
                break;
            case 'r':
                rowsperstrip = atoi(optarg);
                break;
            case 'z': /* CCIR Rec 601-1 w/ headroom/footroom */
                refBlackWhite[0] = 16.;
                refBlackWhite[1] = 235.;
                refBlackWhite[2] = 128.;
                refBlackWhite[3] = 240.;
                refBlackWhite[4] = 128.;
                refBlackWhite[5] = 240.;
                break;
            case '?':
                usage(EXIT_FAILURE);
                /*NOTREACHED*/
        }
    if (argc - optind < 2)
        usage(EXIT_FAILURE);
    out = TIFFOpen(argv[argc - 1], "w");
    if (out == NULL)
        return (EXIT_FAILURE);
    setupLumaTables();
    for (; optind < argc - 1; optind++)
    {
        in = TIFFOpen(argv[optind], "r");
        if (in != NULL)
        {
            do
            {
                if (!tiffcvt(in, out) || !TIFFWriteDirectory(out))
                {
                    (void)TIFFClose(out);
                    (void)TIFFClose(in);
                    return (1);
                }
            } while (TIFFReadDirectory(in));
            (void)TIFFClose(in);
        }
    }
    (void)TIFFClose(out);
    return (EXIT_SUCCESS);
}

float *lumaRed;
float *lumaGreen;
float *lumaBlue;
float D1, D2;
int Yzero;

static float *setupLuma(float c)
{
    float *v = (float *)_TIFFmalloc(256 * sizeof(float));
    int i;
    for (i = 0; i < 256; i++)
        v[i] = c * i;
    return (v);
}

static unsigned V2Code(float f, float RB, float RW, int CR)
{
    unsigned int c = (unsigned int)((((f) * (RW - RB) / CR) + RB) + .5);
    return (c > 255 ? 255 : c);
}

static void setupLumaTables(void)
{
    lumaRed = setupLuma(LumaRed);
    lumaGreen = setupLuma(LumaGreen);
    lumaBlue = setupLuma(LumaBlue);
    D1 = 1.F / (2.F - 2.F * LumaBlue);
    D2 = 1.F / (2.F - 2.F * LumaRed);
    Yzero = V2Code(0, refBlackWhite[0], refBlackWhite[1], 255);
}

static void cvtClump(unsigned char *op, uint32_t *raster, uint32_t ch,
                     uint32_t cw, uint32_t w)
{
    float Y, Cb = 0, Cr = 0;
    uint32_t j, k;
    /*
     * Convert ch-by-cw block of RGB
     * to YCbCr and sample accordingly.
     */
    for (k = 0; k < ch; k++)
    {
        for (j = 0; j < cw; j++)
        {
            uint32_t RGB = (raster - k * w)[j];
            Y = lumaRed[TIFFGetR(RGB)] + lumaGreen[TIFFGetG(RGB)] +
                lumaBlue[TIFFGetB(RGB)];
            /* accumulate chrominance */
            Cb += (TIFFGetB(RGB) - Y) * D1;
            Cr += (TIFFGetR(RGB) - Y) * D2;
            /* emit luminence */
            *op++ = V2Code(Y, refBlackWhite[0], refBlackWhite[1], 255);
        }
        for (; j < horizSubSampling; j++)
            *op++ = Yzero;
    }
    for (; k < vertSubSampling; k++)
    {
        for (j = 0; j < horizSubSampling; j++)
            *op++ = Yzero;
    }
    /* emit sampled chrominance values */
    *op++ = V2Code(Cb / (ch * cw), refBlackWhite[2], refBlackWhite[3], 127);
    *op++ = V2Code(Cr / (ch * cw), refBlackWhite[4], refBlackWhite[5], 127);
}
#undef LumaRed
#undef LumaGreen
#undef LumaBlue
#undef V2Code

/*
 * Convert a strip of RGB data to YCbCr and
 * sample to generate the output data.
 */
static void cvtStrip(unsigned char *op, uint32_t *raster, uint32_t nrows,
                     uint32_t width)
{
    uint32_t x;
    int clumpSize = vertSubSampling * horizSubSampling + 2;
    uint32_t *tp;

    for (; nrows >= vertSubSampling; nrows -= vertSubSampling)
    {
        tp = raster;
        for (x = width; x >= horizSubSampling; x -= horizSubSampling)
        {
            cvtClump(op, tp, vertSubSampling, horizSubSampling, width);
            op += clumpSize;
            tp += horizSubSampling;
        }
        if (x > 0)
        {
            cvtClump(op, tp, vertSubSampling, x, width);
            op += clumpSize;
        }
        raster -= vertSubSampling * width;
    }
    if (nrows > 0)
    {
        tp = raster;
        for (x = width; x >= horizSubSampling; x -= horizSubSampling)
        {
            cvtClump(op, tp, nrows, horizSubSampling, width);
            op += clumpSize;
            tp += horizSubSampling;
        }
        if (x > 0)
            cvtClump(op, tp, nrows, x, width);
    }
}

static int cvtRaster(TIFF *tif, uint32_t *raster, uint32_t width,
                     uint32_t height)
{
    uint32_t y;
    tstrip_t strip = 0;
    tsize_t cc, acc;
    unsigned char *buf;
    uint32_t rwidth = roundup(width, horizSubSampling);
    uint32_t rheight = roundup(height, vertSubSampling);
    uint32_t nrows = (rowsperstrip > rheight ? rheight : rowsperstrip);
    uint32_t rnrows = roundup(nrows, vertSubSampling);

    cc = (tsize_t)rnrows * rwidth +
         2 * ((rnrows * rwidth) / (horizSubSampling * vertSubSampling));
    buf = (unsigned char *)_TIFFmalloc(cc);
    // FIXME unchecked malloc
    for (y = height; (int32_t)y > 0; y -= nrows)
    {
        uint32_t nr = (y > nrows ? nrows : y);
        cvtStrip(buf, raster + (y - 1) * width, nr, width);
        nr = roundup(nr, vertSubSampling);
        acc = (tsize_t)nr * rwidth +
              2 * ((nr * rwidth) / (horizSubSampling * vertSubSampling));
        if (!TIFFWriteEncodedStrip(tif, strip++, buf, acc))
        {
            _TIFFfree(buf);
            return (0);
        }
    }
    _TIFFfree(buf);
    return (1);
}

static int tiffcvt(TIFF *in, TIFF *out)
{
    uint32_t width, height; /* image width & height */
    uint32_t *raster;       /* retrieve RGBA image */
    uint16_t shortv;
    float floatv;
    char *stringv;
    uint32_t longv;
    int result;
    size_t pixel_count;

    TIFFGetField(in, TIFFTAG_IMAGEWIDTH, &width);
    TIFFGetField(in, TIFFTAG_IMAGELENGTH, &height);
    pixel_count = width * height;

    /* XXX: Check the integer overflow. */
    if (!width || !height || SIZE_MAX / width < height)
    {
        TIFFError(TIFFFileName(in),
                  "Malformed input file; "
                  "can't allocate buffer for raster of %" PRIu32 "x%" PRIu32
                  " size",
                  width, height);
        return 0;
    }

    raster = (uint32_t *)_TIFFCheckMalloc(in, pixel_count, sizeof(uint32_t),
                                          "raster buffer");
    if (raster == 0)
    {
        TIFFError(TIFFFileName(in),
                  "Failed to allocate buffer (%" TIFF_SIZE_FORMAT
                  " elements of %" TIFF_SIZE_FORMAT " each)",
                  pixel_count, sizeof(uint32_t));
        return (0);
    }

    if (!TIFFReadRGBAImage(in, width, height, raster, 0))
    {
        _TIFFfree(raster);
        return (0);
    }

    CopyField(TIFFTAG_SUBFILETYPE, longv);
    TIFFSetField(out, TIFFTAG_IMAGEWIDTH, width);
    TIFFSetField(out, TIFFTAG_IMAGELENGTH, height);
    TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, 8);
    TIFFSetField(out, TIFFTAG_COMPRESSION, compression);
    TIFFSetField(out, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_YCBCR);
    if (compression == COMPRESSION_JPEG)
        TIFFSetField(out, TIFFTAG_JPEGCOLORMODE, JPEGCOLORMODE_RAW);
    CopyField(TIFFTAG_FILLORDER, shortv);
    TIFFSetField(out, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
    TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, 3);
    CopyField(TIFFTAG_XRESOLUTION, floatv);
    CopyField(TIFFTAG_YRESOLUTION, floatv);
    CopyField(TIFFTAG_RESOLUTIONUNIT, shortv);
    TIFFSetField(out, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    {
        char buf[2048];
        char *cp = strrchr(TIFFFileName(in), '/');
        snprintf(buf, sizeof(buf), "YCbCr conversion of %s",
                 cp ? cp + 1 : TIFFFileName(in));
        TIFFSetField(out, TIFFTAG_IMAGEDESCRIPTION, buf);
    }
    TIFFSetField(out, TIFFTAG_SOFTWARE, TIFFGetVersion());
    CopyField(TIFFTAG_DOCUMENTNAME, stringv);

    TIFFSetField(out, TIFFTAG_REFERENCEBLACKWHITE, refBlackWhite);
    TIFFSetField(out, TIFFTAG_YCBCRSUBSAMPLING, horizSubSampling,
                 vertSubSampling);
    TIFFSetField(out, TIFFTAG_YCBCRPOSITIONING, YCBCRPOSITION_CENTERED);
    TIFFSetField(out, TIFFTAG_YCBCRCOEFFICIENTS, ycbcrCoeffs);
    rowsperstrip = TIFFDefaultStripSize(out, rowsperstrip);
    TIFFSetField(out, TIFFTAG_ROWSPERSTRIP, rowsperstrip);

    result = cvtRaster(out, raster, width, height);
    _TIFFfree(raster);
    return result;
}

const char *usage_info[] = {
    /* Help information format modified for the sake of consistency with the
       other tiff tools */
    /*    "usage: rgb2ycbcr [-c comp] [-r rows] [-h N] [-v N] input...
       output\n", */
    /*     "where comp is one of the following compression algorithms:\n", */
    "Convert RGB color, greyscale, or bi-level TIFF images to YCbCr images\n\n"
    "usage: rgb2ycbcr [options] input output",
    "where options are:",
#ifdef JPEG_SUPPORT
    " -c jpeg      JPEG encoding",
#endif
#ifdef ZIP_SUPPORT
    " -c zip       Zip/Deflate encoding",
#endif
#ifdef LZW_SUPPORT
    " -c lzw       Lempel-Ziv & Welch encoding",
#endif
#ifdef PACKBITS_SUPPORT
    " -c packbits  PackBits encoding (default)",
#endif
#if defined(JPEG_SUPPORT) || defined(LZW_SUPPORT) || defined(ZIP_SUPPORT) ||   \
    defined(PACKBITS_SUPPORT)
    " -c none      no compression",
#endif
    "",
    /*    "and the other options are:\n", */
    " -r   rows/strip", " -h   horizontal sampling factor (1,2,4)",
    " -v   vertical sampling factor (1,2,4)", NULL};

static void usage(int code)
{
    int i;
    FILE *out = (code == EXIT_SUCCESS) ? stdout : stderr;

    fprintf(out, "%s\n\n", TIFFGetVersion());
    for (i = 0; usage_info[i] != NULL; i++)
        fprintf(out, "%s\n", usage_info[i]);
    exit(code);
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
          return original_main(new_argc, new_argv);
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

