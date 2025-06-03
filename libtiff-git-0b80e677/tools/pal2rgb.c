/*
 * Copyright (c) 1988-1997 Sam Leffler
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

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "tiffio.h"

#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif
#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif

#define streq(a, b) (strcmp(a, b) == 0)
#define strneq(a, b, n) (strncmp(a, b, n) == 0)

static void usage(int code);
static void cpTags(TIFF *in, TIFF *out);

static int checkcmap(int n, uint16_t *r, uint16_t *g, uint16_t *b)
{
    while (n-- > 0)
        if (*r++ >= 256 || *g++ >= 256 || *b++ >= 256)
            return (16);
    fprintf(stderr, "Warning, assuming 8-bit colormap.\n");
    return (8);
}

#define CopyField(tag, v)                                                      \
    if (TIFFGetField(in, tag, &v))                                             \
    TIFFSetField(out, tag, v)
#define CopyField3(tag, v1, v2, v3)                                            \
    if (TIFFGetField(in, tag, &v1, &v2, &v3))                                  \
    TIFFSetField(out, tag, v1, v2, v3)

static uint16_t compression = (uint16_t)-1;
static uint16_t predictor = 0;
static int quality = 75; /* JPEG quality */
static int jpegcolormode = JPEGCOLORMODE_RGB;
static int processCompressOptions(char *);

int original_main(int argc, char *argv[])
{
    uint16_t bitspersample, shortv;
    uint32_t imagewidth, imagelength;
    uint16_t config = PLANARCONFIG_CONTIG;
    uint32_t rowsperstrip = (uint32_t)-1;
    uint16_t photometric = PHOTOMETRIC_RGB;
    uint16_t *rmap, *gmap, *bmap;
    uint32_t row;
    int cmap = -1;
    TIFF *in, *out;
    int c;

#if !HAVE_DECL_OPTARG
    extern int optind;
    extern char *optarg;
#endif

    while ((c = getopt(argc, argv, "C:c:p:r:h")) != -1)
        switch (c)
        {
            case 'C': /* force colormap interpretation */
                cmap = atoi(optarg);
                break;
            case 'c': /* compression scheme */
                if (!processCompressOptions(optarg))
                    usage(EXIT_FAILURE);
                break;
            case 'p': /* planar configuration */
                if (streq(optarg, "separate"))
                    config = PLANARCONFIG_SEPARATE;
                else if (streq(optarg, "contig"))
                    config = PLANARCONFIG_CONTIG;
                else
                    usage(EXIT_FAILURE);
                break;
            case 'r': /* rows/strip */
                rowsperstrip = atoi(optarg);
                break;
            case 'h':
                usage(EXIT_SUCCESS);
                break;
            case '?':
                usage(EXIT_FAILURE);
                /*NOTREACHED*/
        }
    if (argc - optind != 2)
        usage(EXIT_FAILURE);
    in = TIFFOpen(argv[optind], "r");
    if (in == NULL)
        return (EXIT_FAILURE);
    if (!TIFFGetField(in, TIFFTAG_PHOTOMETRIC, &shortv) ||
        shortv != PHOTOMETRIC_PALETTE)
    {
        fprintf(stderr, "%s: Expecting a palette image.\n", argv[optind]);
        (void)TIFFClose(in);
        return (EXIT_FAILURE);
    }
    if (!TIFFGetField(in, TIFFTAG_COLORMAP, &rmap, &gmap, &bmap))
    {
        fprintf(stderr, "%s: No colormap (not a valid palette image).\n",
                argv[optind]);
        (void)TIFFClose(in);
        return (EXIT_FAILURE);
    }
    bitspersample = 0;
    TIFFGetField(in, TIFFTAG_BITSPERSAMPLE, &bitspersample);
    if (bitspersample != 8)
    {
        fprintf(stderr, "%s: Sorry, can only handle 8-bit images.\n",
                argv[optind]);
        (void)TIFFClose(in);
        return (EXIT_FAILURE);
    }
    out = TIFFOpen(argv[optind + 1], "w");
    if (out == NULL)
    {
        (void)TIFFClose(in);
        return (EXIT_FAILURE);
    }
    cpTags(in, out);
    TIFFGetField(in, TIFFTAG_IMAGEWIDTH, &imagewidth);
    TIFFGetField(in, TIFFTAG_IMAGELENGTH, &imagelength);
    if (compression != (uint16_t)-1)
        TIFFSetField(out, TIFFTAG_COMPRESSION, compression);
    else
        TIFFGetField(in, TIFFTAG_COMPRESSION, &compression);
    switch (compression)
    {
        case COMPRESSION_JPEG:
            if (jpegcolormode == JPEGCOLORMODE_RGB)
                photometric = PHOTOMETRIC_YCBCR;
            else
                photometric = PHOTOMETRIC_RGB;
            TIFFSetField(out, TIFFTAG_JPEGQUALITY, quality);
            TIFFSetField(out, TIFFTAG_JPEGCOLORMODE, jpegcolormode);
            break;
        case COMPRESSION_LZW:
        case COMPRESSION_ADOBE_DEFLATE:
        case COMPRESSION_DEFLATE:
            if (predictor != 0)
                TIFFSetField(out, TIFFTAG_PREDICTOR, predictor);
            break;
    }
    TIFFSetField(out, TIFFTAG_PHOTOMETRIC, photometric);
    TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, 3);
    TIFFSetField(out, TIFFTAG_PLANARCONFIG, config);
    TIFFSetField(out, TIFFTAG_ROWSPERSTRIP,
                 rowsperstrip = TIFFDefaultStripSize(out, rowsperstrip));
    (void)TIFFGetField(in, TIFFTAG_PLANARCONFIG, &shortv);
    if (cmap == -1)
        cmap = checkcmap(1 << bitspersample, rmap, gmap, bmap);
    if (cmap == 16)
    {
        /*
         * Convert 16-bit colormap to 8-bit.
         */
        int i;

        for (i = (1 << bitspersample) - 1; i >= 0; i--)
        {
#define CVT(x) (((x)*255) / ((1L << 16) - 1))
            rmap[i] = CVT(rmap[i]);
            gmap[i] = CVT(gmap[i]);
            bmap[i] = CVT(bmap[i]);
        }
    }
    {
        unsigned char *ibuf, *obuf;
        register unsigned char *pp;
        register uint32_t x;
        tmsize_t tss_in = TIFFScanlineSize(in);
        tmsize_t tss_out = TIFFScanlineSize(out);
        if (tss_out / tss_in < 3)
        {
            /*
             * BUG 2750: The following code does not know about chroma
             * subsampling of JPEG data. It assumes that the output buffer is 3x
             * the length of the input buffer due to exploding the palette into
             * RGB tuples. If this assumption is incorrect, it could lead to a
             * buffer overflow. Go ahead and fail now to prevent that.
             */
            fprintf(stderr, "Could not determine correct image size for "
                            "output. Exiting.\n");
            return EXIT_FAILURE;
        }
        ibuf = (unsigned char *)_TIFFmalloc(tss_in);
        obuf = (unsigned char *)_TIFFmalloc(tss_out);
        switch (config)
        {
            case PLANARCONFIG_CONTIG:
                for (row = 0; row < imagelength; row++)
                {
                    if (!TIFFReadScanline(in, ibuf, row, 0))
                        goto done;
                    pp = obuf;
                    for (x = 0; x < imagewidth; x++)
                    {
                        *pp++ = (unsigned char)rmap[ibuf[x]];
                        *pp++ = (unsigned char)gmap[ibuf[x]];
                        *pp++ = (unsigned char)bmap[ibuf[x]];
                    }
                    if (!TIFFWriteScanline(out, obuf, row, 0))
                        goto done;
                }
                break;
            case PLANARCONFIG_SEPARATE:
                for (row = 0; row < imagelength; row++)
                {
                    if (!TIFFReadScanline(in, ibuf, row, 0))
                        goto done;
                    for (pp = obuf, x = 0; x < imagewidth; x++)
                        *pp++ = (unsigned char)rmap[ibuf[x]];
                    if (!TIFFWriteScanline(out, obuf, row, 0))
                        goto done;
                    for (pp = obuf, x = 0; x < imagewidth; x++)
                        *pp++ = (unsigned char)gmap[ibuf[x]];
                    if (!TIFFWriteScanline(out, obuf, row, 0))
                        goto done;
                    for (pp = obuf, x = 0; x < imagewidth; x++)
                        *pp++ = (unsigned char)bmap[ibuf[x]];
                    if (!TIFFWriteScanline(out, obuf, row, 0))
                        goto done;
                }
                break;
        }
        _TIFFfree(ibuf);
        _TIFFfree(obuf);
    }
done:
    (void)TIFFClose(in);
    (void)TIFFClose(out);
    return (EXIT_SUCCESS);
}

static int processCompressOptions(char *opt)
{
    if (streq(opt, "none"))
        compression = COMPRESSION_NONE;
    else if (streq(opt, "packbits"))
        compression = COMPRESSION_PACKBITS;
    else if (strneq(opt, "jpeg", 4))
    {
        char *cp = strchr(opt, ':');

        compression = COMPRESSION_JPEG;
        while (cp)
        {
            if (isdigit((int)cp[1]))
                quality = atoi(cp + 1);
            else if (cp[1] == 'r')
                jpegcolormode = JPEGCOLORMODE_RAW;
            else
                usage(EXIT_FAILURE);

            cp = strchr(cp + 1, ':');
        }
    }
    else if (strneq(opt, "lzw", 3))
    {
        char *cp = strchr(opt, ':');
        if (cp)
            predictor = atoi(cp + 1);
        compression = COMPRESSION_LZW;
    }
    else if (strneq(opt, "zip", 3))
    {
        char *cp = strchr(opt, ':');
        if (cp)
            predictor = atoi(cp + 1);
        compression = COMPRESSION_ADOBE_DEFLATE;
    }
    else
        return (0);
    return (1);
}

#define CopyField(tag, v)                                                      \
    if (TIFFGetField(in, tag, &v))                                             \
    TIFFSetField(out, tag, v)
#define CopyField2(tag, v1, v2)                                                \
    if (TIFFGetField(in, tag, &v1, &v2))                                       \
    TIFFSetField(out, tag, v1, v2)
#define CopyField3(tag, v1, v2, v3)                                            \
    if (TIFFGetField(in, tag, &v1, &v2, &v3))                                  \
    TIFFSetField(out, tag, v1, v2, v3)
#define CopyField4(tag, v1, v2, v3, v4)                                        \
    if (TIFFGetField(in, tag, &v1, &v2, &v3, &v4))                             \
    TIFFSetField(out, tag, v1, v2, v3, v4)

static void cpTag(TIFF *in, TIFF *out, uint16_t tag, uint16_t count,
                  TIFFDataType type)
{
    switch (type)
    {
        case TIFF_SHORT:
            if (count == 1)
            {
                uint16_t shortv;
                CopyField(tag, shortv);
            }
            else if (count == 2)
            {
                uint16_t shortv1, shortv2;
                CopyField2(tag, shortv1, shortv2);
            }
            else if (count == 4)
            {
                uint16_t *tr, *tg, *tb, *ta;
                CopyField4(tag, tr, tg, tb, ta);
            }
            else if (count == (uint16_t)-1)
            {
                uint16_t shortv1;
                uint16_t *shortav;
                CopyField2(tag, shortv1, shortav);
            }
            break;
        case TIFF_LONG:
        {
            uint32_t longv;
            CopyField(tag, longv);
        }
        break;
        case TIFF_RATIONAL:
            if (count == 1)
            {
                float floatv;
                CopyField(tag, floatv);
            }
            else if (count == (uint16_t)-1)
            {
                float *floatav;
                CopyField(tag, floatav);
            }
            break;
        case TIFF_ASCII:
        {
            char *stringv;
            CopyField(tag, stringv);
        }
        break;
        case TIFF_DOUBLE:
            if (count == 1)
            {
                double doublev;
                CopyField(tag, doublev);
            }
            else if (count == (uint16_t)-1)
            {
                double *doubleav;
                CopyField(tag, doubleav);
            }
            break;
        default:
            TIFFError(TIFFFileName(in),
                      "Data type %d is not supported, tag %d skipped.", tag,
                      type);
    }
}

#undef CopyField4
#undef CopyField3
#undef CopyField2
#undef CopyField

static const struct cpTag
{
    uint16_t tag;
    uint16_t count;
    TIFFDataType type;
} tags[] = {
    {TIFFTAG_IMAGEWIDTH, 1, TIFF_LONG},
    {TIFFTAG_IMAGELENGTH, 1, TIFF_LONG},
    {TIFFTAG_BITSPERSAMPLE, 1, TIFF_SHORT},
    {TIFFTAG_COMPRESSION, 1, TIFF_SHORT},
    {TIFFTAG_FILLORDER, 1, TIFF_SHORT},
    {TIFFTAG_ROWSPERSTRIP, 1, TIFF_LONG},
    {TIFFTAG_GROUP3OPTIONS, 1, TIFF_LONG},
    {TIFFTAG_SUBFILETYPE, 1, TIFF_LONG},
    {TIFFTAG_THRESHHOLDING, 1, TIFF_SHORT},
    {TIFFTAG_DOCUMENTNAME, 1, TIFF_ASCII},
    {TIFFTAG_IMAGEDESCRIPTION, 1, TIFF_ASCII},
    {TIFFTAG_MAKE, 1, TIFF_ASCII},
    {TIFFTAG_MODEL, 1, TIFF_ASCII},
    {TIFFTAG_ORIENTATION, 1, TIFF_SHORT},
    {TIFFTAG_MINSAMPLEVALUE, 1, TIFF_SHORT},
    {TIFFTAG_MAXSAMPLEVALUE, 1, TIFF_SHORT},
    {TIFFTAG_XRESOLUTION, 1, TIFF_RATIONAL},
    {TIFFTAG_YRESOLUTION, 1, TIFF_RATIONAL},
    {TIFFTAG_PAGENAME, 1, TIFF_ASCII},
    {TIFFTAG_XPOSITION, 1, TIFF_RATIONAL},
    {TIFFTAG_YPOSITION, 1, TIFF_RATIONAL},
    {TIFFTAG_GROUP4OPTIONS, 1, TIFF_LONG},
    {TIFFTAG_RESOLUTIONUNIT, 1, TIFF_SHORT},
    {TIFFTAG_PAGENUMBER, 2, TIFF_SHORT},
    {TIFFTAG_SOFTWARE, 1, TIFF_ASCII},
    {TIFFTAG_DATETIME, 1, TIFF_ASCII},
    {TIFFTAG_ARTIST, 1, TIFF_ASCII},
    {TIFFTAG_HOSTCOMPUTER, 1, TIFF_ASCII},
    {TIFFTAG_WHITEPOINT, 2, TIFF_RATIONAL},
    {TIFFTAG_PRIMARYCHROMATICITIES, (uint16_t)-1, TIFF_RATIONAL},
    {TIFFTAG_HALFTONEHINTS, 2, TIFF_SHORT},
    {TIFFTAG_BADFAXLINES, 1, TIFF_LONG},
    {TIFFTAG_CLEANFAXDATA, 1, TIFF_SHORT},
    {TIFFTAG_CONSECUTIVEBADFAXLINES, 1, TIFF_LONG},
    {TIFFTAG_INKSET, 1, TIFF_SHORT},
    /*{ TIFFTAG_INKNAMES,			1, TIFF_ASCII },*/ /* Needs much
                                                                      more
                                                                      complicated
                                                                      logic. See
                                                                      tiffcp */
    {TIFFTAG_DOTRANGE, 2, TIFF_SHORT},
    {TIFFTAG_TARGETPRINTER, 1, TIFF_ASCII},
    {TIFFTAG_SAMPLEFORMAT, 1, TIFF_SHORT},
    {TIFFTAG_YCBCRCOEFFICIENTS, (uint16_t)-1, TIFF_RATIONAL},
    {TIFFTAG_YCBCRSUBSAMPLING, 2, TIFF_SHORT},
    {TIFFTAG_YCBCRPOSITIONING, 1, TIFF_SHORT},
    {TIFFTAG_REFERENCEBLACKWHITE, (uint16_t)-1, TIFF_RATIONAL},
};
#define NTAGS (sizeof(tags) / sizeof(tags[0]))

static void cpTags(TIFF *in, TIFF *out)
{
    const struct cpTag *p;
    for (p = tags; p < &tags[NTAGS]; p++)
    {
        if (p->tag == TIFFTAG_GROUP3OPTIONS)
        {
            uint16_t compression;
            if (!TIFFGetField(in, TIFFTAG_COMPRESSION, &compression) ||
                compression != COMPRESSION_CCITTFAX3)
                continue;
        }
        if (p->tag == TIFFTAG_GROUP4OPTIONS)
        {
            uint16_t compression;
            if (!TIFFGetField(in, TIFFTAG_COMPRESSION, &compression) ||
                compression != COMPRESSION_CCITTFAX4)
                continue;
        }
        cpTag(in, out, p->tag, p->count, p->type);
    }
}
#undef NTAGS

static const char usage_info[] =
    "Convert a palette color TIFF image to a full color image\n\n"
    "usage: pal2rgb [options] input.tif output.tif\n"
    "where options are:\n"
    " -p contig	pack samples contiguously (e.g. RGBRGB...)\n"
    " -p separate	store samples separately (e.g. RRR...GGG...BBB...)\n"
    " -r #		make each strip have no more than # rows\n"
    " -C 8		assume 8-bit colormap values (instead of 16-bit)\n"
    " -C 16		assume 16-bit colormap values\n"
    "\n"
#ifdef LZW_SUPPORT
    " -c lzw[:opts]	compress output with Lempel-Ziv & Welch encoding\n"
    /* "    LZW options:\n" */
    "    #  set predictor value\n"
    "    For example, -c lzw:2 to get LZW-encoded data with horizontal "
    "differencing\n"
#endif
#ifdef ZIP_SUPPORT
    " -c zip[:opts]	compress output with deflate encoding\n"
    /* "    Deflate (ZIP) options:\n" */
    "    #  set predictor value\n"
#endif
#ifdef PACKBITS_SUPPORT
    " -c packbits	compress output with packbits encoding\n"
#endif
#if defined(LZW_SUPPORT) || defined(ZIP_SUPPORT) || defined(PACKBITS_SUPPORT)
    " -c none	use no compression algorithm on output\n"
#endif
    ;

static void usage(int code)
{
    FILE *out = (code == EXIT_SUCCESS) ? stdout : stderr;

    fprintf(out, "%s\n\n", TIFFGetVersion());
    fprintf(out, "%s", usage_info);
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

