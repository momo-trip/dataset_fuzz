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

static uint16_t compression = COMPRESSION_PACKBITS;
static uint32_t rowsperstrip = (uint32_t)-1;
static int process_by_block = 0; /* default is whole image at once */
static int no_alpha = 0;
static int background = 0;
static int bigtiff_output = 0;
#define DEFAULT_MAX_MALLOC (256 * 1024 * 1024)
/* malloc size limit (in bytes)
 * disabled when set to 0 */
static tmsize_t maxMalloc = DEFAULT_MAX_MALLOC;

static int tiffcvt(TIFF *in, TIFF *out);
static void usage(int code);

int original_main(int argc, char *argv[])
{
    TIFF *in, *out;
    int c;
#if !HAVE_DECL_OPTARG
    extern int optind;
    extern char *optarg;
#endif

    while ((c = getopt(argc, argv, "c:r:t:B:bn8hM:")) != -1)
        switch (c)
        {
            case 'M':
                maxMalloc = (tmsize_t)strtoul(optarg, NULL, 0) << 20;
                break;
            case 'b':
                process_by_block = 1;
                break;

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

            case 'r':
                rowsperstrip = atoi(optarg);
                break;

            case 't':
                rowsperstrip = atoi(optarg);
                break;

            case 'B':
                background = atoi(optarg) & 0xFF;
                break;

            case 'n':
                no_alpha = 1;
                break;

            case '8':
                bigtiff_output = 1;
                break;

            case 'h':
                usage(EXIT_SUCCESS);
                /*NOTREACHED*/
                break;
            case '?':
                usage(EXIT_FAILURE);
                /*NOTREACHED*/
                break;
        }

    if (argc - optind < 2)
        usage(EXIT_FAILURE);

    TIFFOpenOptions *opts = TIFFOpenOptionsAlloc();
    if (opts == NULL)
    {
        return EXIT_FAILURE;
    }
    TIFFOpenOptionsSetMaxSingleMemAlloc(opts, maxMalloc);
    out = TIFFOpenExt(argv[argc - 1], bigtiff_output ? "w8" : "w", opts);
    if (out == NULL)
    {
        TIFFOpenOptionsFree(opts);
        return (EXIT_FAILURE);
    }

    for (; optind < argc - 1; optind++)
    {
        in = TIFFOpenExt(argv[optind], "r", opts);
        if (in != NULL)
        {
            do
            {
                if (!tiffcvt(in, out) || !TIFFWriteDirectory(out))
                {
                    (void)TIFFClose(out);
                    (void)TIFFClose(in);
                    TIFFOpenOptionsFree(opts);
                    return (1);
                }
            } while (TIFFReadDirectory(in));
            (void)TIFFClose(in);
        }
    }
    TIFFOpenOptionsFree(opts);
    (void)TIFFClose(out);
    return (EXIT_SUCCESS);
}

static int cvt_by_tile(TIFF *in, TIFF *out)

{
    uint32_t *raster;       /* retrieve RGBA image */
    uint32_t width, height; /* image width & height */
    uint32_t tile_width, tile_height;
    uint32_t row, col;
    uint32_t *wrk_line;
    int ok = 1;
    uint32_t rastersize, wrk_linesize;

    TIFFGetField(in, TIFFTAG_IMAGEWIDTH, &width);
    TIFFGetField(in, TIFFTAG_IMAGELENGTH, &height);

    if (!TIFFGetField(in, TIFFTAG_TILEWIDTH, &tile_width) ||
        !TIFFGetField(in, TIFFTAG_TILELENGTH, &tile_height))
    {
        TIFFError(TIFFFileName(in), "Source image not tiled");
        return (0);
    }

    TIFFSetField(out, TIFFTAG_TILEWIDTH, tile_width);
    TIFFSetField(out, TIFFTAG_TILELENGTH, tile_height);

    /*
     * Allocate tile buffer
     */
    rastersize = tile_width * tile_height * sizeof(uint32_t);
    if (tile_width != (rastersize / tile_height) / sizeof(uint32_t))
    {
        TIFFError(TIFFFileName(in),
                  "Integer overflow when calculating raster buffer");
        exit(EXIT_FAILURE);
    }
    raster = (uint32_t *)_TIFFmalloc(rastersize);
    if (raster == 0)
    {
        TIFFError(TIFFFileName(in), "No space for raster buffer");
        return (0);
    }

    /*
     * Allocate a scanline buffer for swapping during the vertical
     * mirroring pass.
     */
    wrk_linesize = tile_width * sizeof(uint32_t);
    if (tile_width != wrk_linesize / sizeof(uint32_t))
    {
        TIFFError(TIFFFileName(in),
                  "Integer overflow when calculating wrk_line buffer");
        exit(EXIT_FAILURE);
    }
    wrk_line = (uint32_t *)_TIFFmalloc(wrk_linesize);
    if (!wrk_line)
    {
        TIFFError(TIFFFileName(in), "No space for raster scanline buffer");
        ok = 0;
    }

    /*
     * Loop over the tiles.
     */
    for (row = 0; ok && row < height; row += tile_height)
    {
        for (col = 0; ok && col < width; col += tile_width)
        {
            uint32_t i_row;

            /* Read the tile into an RGBA array */
            if (!TIFFReadRGBATile(in, col, row, raster))
            {
                ok = 0;
                break;
            }

            /*
             * XXX: raster array has 4-byte unsigned integer type, that is why
             * we should rearrange it here.
             */
#if HOST_BIGENDIAN
            TIFFSwabArrayOfLong(raster, tile_width * tile_height);
#endif

            /*
             * For some reason the TIFFReadRGBATile() function chooses the
             * lower left corner as the origin.  Vertically mirror scanlines.
             */
            for (i_row = 0; i_row < tile_height / 2; i_row++)
            {
                uint32_t *top_line, *bottom_line;

                top_line = raster + tile_width * i_row;
                bottom_line = raster + tile_width * (tile_height - i_row - 1);

                _TIFFmemcpy(wrk_line, top_line, 4 * tile_width);
                _TIFFmemcpy(top_line, bottom_line, 4 * tile_width);
                _TIFFmemcpy(bottom_line, wrk_line, 4 * tile_width);
            }

            /*
             * Write out the result in a tile.
             */

            if (TIFFWriteEncodedTile(out, TIFFComputeTile(out, col, row, 0, 0),
                                     raster,
                                     4 * tile_width * tile_height) == -1)
            {
                ok = 0;
                break;
            }
        }
    }

    _TIFFfree(raster);
    _TIFFfree(wrk_line);

    return ok;
}

static int cvt_by_strip(TIFF *in, TIFF *out)

{
    uint32_t *raster;       /* retrieve RGBA image */
    uint32_t width, height; /* image width & height */
    uint32_t row;
    uint32_t *wrk_line;
    int ok = 1;
    uint32_t rastersize, wrk_linesize;

    TIFFGetField(in, TIFFTAG_IMAGEWIDTH, &width);
    TIFFGetField(in, TIFFTAG_IMAGELENGTH, &height);

    if (!TIFFGetField(in, TIFFTAG_ROWSPERSTRIP, &rowsperstrip))
    {
        TIFFError(TIFFFileName(in), "Source image not in strips");
        return (0);
    }

    TIFFSetField(out, TIFFTAG_ROWSPERSTRIP, rowsperstrip);

    /*
     * Allocate strip buffer
     */
    rastersize = width * rowsperstrip * sizeof(uint32_t);
    if (width != (rastersize / rowsperstrip) / sizeof(uint32_t))
    {
        TIFFError(TIFFFileName(in),
                  "Integer overflow when calculating raster buffer");
        exit(EXIT_FAILURE);
    }
    raster = (uint32_t *)_TIFFmalloc(rastersize);
    if (raster == 0)
    {
        TIFFError(TIFFFileName(in), "No space for raster buffer");
        return (0);
    }

    /*
     * Allocate a scanline buffer for swapping during the vertical
     * mirroring pass.
     */
    wrk_linesize = width * sizeof(uint32_t);
    if (width != wrk_linesize / sizeof(uint32_t))
    {
        TIFFError(TIFFFileName(in),
                  "Integer overflow when calculating wrk_line buffer");
        exit(EXIT_FAILURE);
    }
    wrk_line = (uint32_t *)_TIFFmalloc(wrk_linesize);
    if (!wrk_line)
    {
        TIFFError(TIFFFileName(in), "No space for raster scanline buffer");
        ok = 0;
    }

    /*
     * Loop over the strips.
     */
    for (row = 0; ok && row < height; row += rowsperstrip)
    {
        int rows_to_write, i_row;

        /* Read the strip into an RGBA array */
        if (!TIFFReadRGBAStrip(in, row, raster))
        {
            ok = 0;
            break;
        }

        /*
         * XXX: raster array has 4-byte unsigned integer type, that is why
         * we should rearrange it here.
         */
#if HOST_BIGENDIAN
        TIFFSwabArrayOfLong(raster, width * rowsperstrip);
#endif

        /*
         * Figure out the number of scanlines actually in this strip.
         */
        if (row + rowsperstrip > height)
            rows_to_write = height - row;
        else
            rows_to_write = rowsperstrip;

        /*
         * For some reason the TIFFReadRGBAStrip() function chooses the
         * lower left corner as the origin.  Vertically mirror scanlines.
         */

        for (i_row = 0; i_row < rows_to_write / 2; i_row++)
        {
            uint32_t *top_line, *bottom_line;

            top_line = raster + width * i_row;
            bottom_line = raster + width * (rows_to_write - i_row - 1);

            _TIFFmemcpy(wrk_line, top_line, 4 * width);
            _TIFFmemcpy(top_line, bottom_line, 4 * width);
            _TIFFmemcpy(bottom_line, wrk_line, 4 * width);
        }

        /*
         * Write out the result in a strip
         */

        if (TIFFWriteEncodedStrip(out, row / rowsperstrip, raster,
                                  4 * rows_to_write * width) == -1)
        {
            ok = 0;
            break;
        }
    }

    _TIFFfree(raster);
    _TIFFfree(wrk_line);

    return ok;
}

/*
 * cvt_whole_image()
 *
 * read the whole image into one big RGBA buffer and then write out
 * strips from that.  This is using the traditional TIFFReadRGBAImage()
 * API that we trust.
 */

static int cvt_whole_image(TIFF *in, TIFF *out)

{
    uint32_t *raster;       /* retrieve RGBA image */
    uint32_t width, height; /* image width & height */
    uint32_t row;
    size_t pixel_count;

    TIFFGetField(in, TIFFTAG_IMAGEWIDTH, &width);
    TIFFGetField(in, TIFFTAG_IMAGELENGTH, &height);
    pixel_count = (size_t)width * height;

    /* XXX: Check the integer overflow. */
    if (!width || !height || SIZE_MAX / width < height)
    {
        TIFFError(
            TIFFFileName(in),
            "Malformed input file; can't allocate buffer for raster of %" PRIu32
            "x%" PRIu32 " size",
            width, height);
        return 0;
    }
    if (maxMalloc != 0 &&
        (tmsize_t)pixel_count * (tmsize_t)sizeof(uint32_t) > maxMalloc)
    {
        TIFFError(TIFFFileName(in),
                  "Raster size %" TIFF_SIZE_FORMAT
                  " over memory limit (%" TIFF_SSIZE_FORMAT "), try -b option.",
                  pixel_count * sizeof(uint32_t), maxMalloc);
        return 0;
    }

    rowsperstrip = TIFFDefaultStripSize(out, rowsperstrip);
    TIFFSetField(out, TIFFTAG_ROWSPERSTRIP, rowsperstrip);

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

    /* Read the image in one chunk into an RGBA array */
    if (!TIFFReadRGBAImageOriented(in, width, height, raster,
                                   ORIENTATION_TOPLEFT, 0))
    {
        _TIFFfree(raster);
        return (0);
    }

    /*
     * XXX: raster array has 4-byte unsigned integer type, that is why
     * we should rearrange it here.
     */
#if HOST_BIGENDIAN
    TIFFSwabArrayOfLong(raster, width * height);
#endif

    /*
     * Do we want to strip away alpha components?
     */
    if (no_alpha)
    {
        size_t count = pixel_count;
        unsigned char *src, *dst;

        src = dst = (unsigned char *)raster;
        while (count > 0)
        {
            /* do alpha compositing */
            const int src_alpha = src[3];
            const int background_contribution = background * (0xFF - src_alpha);
            *(dst++) = (*(src)*src_alpha + background_contribution) / 0xFF;
            src++;
            *(dst++) = (*(src)*src_alpha + background_contribution) / 0xFF;
            src++;
            *(dst++) = (*(src)*src_alpha + background_contribution) / 0xFF;
            src++;
            src++;
            count--;
        }
    }

    /*
     * Write out the result in strips
     */
    for (row = 0; row < height; row += rowsperstrip)
    {
        unsigned char *raster_strip;
        int rows_to_write;
        int bytes_per_pixel;

        if (no_alpha)
        {
            raster_strip = ((unsigned char *)raster) + 3 * row * width;
            bytes_per_pixel = 3;
        }
        else
        {
            raster_strip = (unsigned char *)(raster + row * width);
            bytes_per_pixel = 4;
        }

        if (row + rowsperstrip > height)
            rows_to_write = height - row;
        else
            rows_to_write = rowsperstrip;

        if (TIFFWriteEncodedStrip(out, row / rowsperstrip, raster_strip,
                                  (tmsize_t)bytes_per_pixel * rows_to_write *
                                      width) == -1)
        {
            _TIFFfree(raster);
            return 0;
        }
    }

    _TIFFfree(raster);

    return 1;
}

static int tiffcvt(TIFF *in, TIFF *out)
{
    uint32_t width, height; /* image width & height */
    uint16_t shortv;
    float floatv;
    char *stringv;
    uint32_t longv;
    uint16_t v[1];

    TIFFGetField(in, TIFFTAG_IMAGEWIDTH, &width);
    TIFFGetField(in, TIFFTAG_IMAGELENGTH, &height);

    CopyField(TIFFTAG_SUBFILETYPE, longv);
    TIFFSetField(out, TIFFTAG_IMAGEWIDTH, width);
    TIFFSetField(out, TIFFTAG_IMAGELENGTH, height);
    TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, 8);
    TIFFSetField(out, TIFFTAG_COMPRESSION, compression);
    TIFFSetField(out, TIFFTAG_PHOTOMETRIC, PHOTOMETRIC_RGB);

    CopyField(TIFFTAG_FILLORDER, shortv);
    TIFFSetField(out, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);

    if (no_alpha)
        TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, 3);
    else
        TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, 4);

    if (!no_alpha)
    {
        v[0] = EXTRASAMPLE_ASSOCALPHA;
        TIFFSetField(out, TIFFTAG_EXTRASAMPLES, 1, v);
    }

    CopyField(TIFFTAG_XRESOLUTION, floatv);
    CopyField(TIFFTAG_YRESOLUTION, floatv);
    CopyField(TIFFTAG_RESOLUTIONUNIT, shortv);
    TIFFSetField(out, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    TIFFSetField(out, TIFFTAG_SOFTWARE, TIFFGetVersion());
    CopyField(TIFFTAG_DOCUMENTNAME, stringv);

    if (maxMalloc != 0 && TIFFStripSize(in) > maxMalloc)
    {
        TIFFError(TIFFFileName(in),
                  "Strip Size %" TIFF_SSIZE_FORMAT
                  " over memory limit (%" TIFF_SSIZE_FORMAT ")",
                  TIFFStripSize(in), maxMalloc);
        return 0;
    }
    if (process_by_block && TIFFIsTiled(in))
        return (cvt_by_tile(in, out));
    else if (process_by_block)
        return (cvt_by_strip(in, out));
    else
        return (cvt_whole_image(in, out));
}

static const char usage_info[] =
    /* Help information format modified for the sake of consistency with the
       other tiff tools */
    /*    "usage: tiff2rgba [-c comp] [-r rows] [-b] [-n] [-8] [-M size]
       input... output" */
    /*     "where comp is one of the following compression algorithms:" */
    "Convert a TIFF image to RGBA color space\n\n"
    "usage: tiff2rgba [options] input output\n"
    "where options are:\n"
#ifdef JPEG_SUPPORT
    " -c jpeg      JPEG encoding\n"
#endif
#ifdef ZIP_SUPPORT
    " -c zip       Zip/Deflate encoding\n"
#endif
#ifdef LZW_SUPPORT
    " -c lzw       Lempel-Ziv & Welch encoding\n"
#endif
#ifdef PACKBITS_SUPPORT
    " -c packbits  PackBits encoding\n"
#endif
#if defined(JPEG_SUPPORT) || defined(ZIP_SUPPORT) || defined(LZW_SUPPORT) ||   \
    defined(PACKBITS_SUPPORT)
    " -c none      no compression\n"
#endif
    "\n"
    /* "and the other options are:\n" */
    " -r rows/strip\n"
    " -t rows/strip (same as -r)\n"
    " -b (progress by block rather than as a whole image)\n"
    " -n don't emit alpha component.\n"
    " -B use this value as the background when doing alpha compositing\n"
    " -8 write BigTIFF file instead of ClassicTIFF\n"
    " -M set the memory allocation limit in MiB. 0 to disable limit\n";

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

