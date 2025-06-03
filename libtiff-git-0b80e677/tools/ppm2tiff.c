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

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#ifdef HAVE_FCNTL_H
#include <fcntl.h>
#endif

#ifdef HAVE_IO_H
#include <io.h>
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

static uint16_t compression = COMPRESSION_PACKBITS;
static uint16_t predictor = 0;
static int quality = 75; /* JPEG quality */
static int jpegcolormode = JPEGCOLORMODE_RGB;
static uint32_t g3opts;

static void usage(int code);
static int processCompressOptions(char *);

static void pack_none(unsigned char *buf, unsigned int smpls, uint16_t bps)
{
    (void)buf;
    (void)smpls;
    (void)bps;
    return;
}

static void pack_swab(unsigned char *buf, unsigned int smpls, uint16_t bps)
{
    unsigned int s;
    unsigned char h;
    unsigned char l;
    (void)bps;

    for (s = 0; smpls > s; s++)
    {

        h = buf[s * 2 + 0];
        l = buf[s * 2 + 1];

        buf[s * 2 + 0] = l;
        buf[s * 2 + 1] = h;
    }
    return;
}

static void pack_bytes(unsigned char *buf, unsigned int smpls, uint16_t bps)
{
    unsigned int s;
    unsigned int in;
    unsigned int out;
    int bits;
    uint16_t t;

    in = 0;
    out = 0;
    bits = 0;
    t = 0;

    for (s = 0; smpls > s; s++)
    {

        t <<= bps;
        t |= (uint16_t)buf[in++];

        bits += bps;

        if (8 <= bits)
        {
            bits -= 8;
            buf[out++] = (t >> bits) & 0xFF;
        }
    }
    if (0 != bits)
        buf[out] = (t << (8 - bits)) & 0xFF;
}

static void pack_words(unsigned char *buf, unsigned int smpls, uint16_t bps)
{
    unsigned int s;
    unsigned int in;
    unsigned int out;
    int bits;
    uint32_t t;

    in = 0;
    out = 0;
    bits = 0;
    t = 0;

    for (s = 0; smpls > s; s++)
    {

        t <<= bps;
        t |= (uint32_t)buf[in++] << 8;
        t |= (uint32_t)buf[in++] << 0;

        bits += bps;

        if (16 <= bits)
        {

            bits -= 16;
            buf[out++] = (t >> (bits + 8));
            buf[out++] = (t >> (bits + 0));
        }
    }
    if (0 != bits)
    {
        t <<= 16 - bits;

        buf[out++] = (t >> (0 + 8));
        buf[out++] = (t >> (0 + 0));
    }
}

static void BadPPM(char *file)
{
    fprintf(stderr, "%s: Not a PPM file.\n", file);
    exit(EXIT_FAILURE);
}

static tmsize_t multiply_ms(tmsize_t m1, tmsize_t m2)
{
    if (m1 == 0 || m2 > TIFF_TMSIZE_T_MAX / m1)
        return 0;
    return m1 * m2;
}

int original_main(int argc, char *argv[])
{
    uint16_t photometric = 0;
    uint32_t rowsperstrip = (uint32_t)-1;
    double resolution = -1;
    unsigned char *buf = NULL;
    tmsize_t linebytes = 0;
    int pbm;
    uint16_t spp = 1;
    uint16_t bpp = 8;
    void (*pack_func)(unsigned char *buf, unsigned int smpls, uint16_t bps);
    TIFF *out;
    FILE *in;
    unsigned int w, h, prec, row;
    char *infile;
    int c;
#if !HAVE_DECL_OPTARG
    extern int optind;
    extern char *optarg;
#endif
    tmsize_t scanline_size;

    if (argc < 2)
    {
        fprintf(stderr, "%s: Too few arguments\n", argv[0]);
        usage(EXIT_FAILURE);
    }
    while ((c = getopt(argc, argv, "c:r:R:h")) != -1)
        switch (c)
        {
            case 'c': /* compression scheme */
                if (!processCompressOptions(optarg))
                    usage(EXIT_FAILURE);
                break;
            case 'r': /* rows/strip */
                rowsperstrip = atoi(optarg);
                break;
            case 'R': /* resolution */
                resolution = atof(optarg);
                break;
            case 'h':
                usage(EXIT_SUCCESS);
                break;
            case '?':
                usage(EXIT_FAILURE);
                /*NOTREACHED*/
        }

    if (optind + 2 < argc)
    {
        fprintf(stderr, "%s: Too many arguments\n", argv[0]);
        usage(EXIT_FAILURE);
    }

    /*
     * If only one file is specified, read input from
     * stdin; otherwise usage is: ppm2tiff input output.
     */
    if (argc - optind > 1)
    {
        infile = argv[optind++];
        in = fopen(infile, "rb");
        if (in == NULL)
        {
            fprintf(stderr, "%s: Can not open.\n", infile);
            return (EXIT_FAILURE);
        }
    }
    else
    {
        infile = "<stdin>";
        in = stdin;
#if defined(HAVE_SETMODE) && defined(O_BINARY)
        setmode(fileno(stdin), O_BINARY);
#endif
    }

    if (fgetc(in) != 'P')
        BadPPM(infile);
    switch (fgetc(in))
    {
        case '4': /* it's a PBM file */
            pbm = !0;
            spp = 1;
            photometric = PHOTOMETRIC_MINISWHITE;
            break;
        case '5': /* it's a PGM file */
            pbm = 0;
            spp = 1;
            photometric = PHOTOMETRIC_MINISBLACK;
            break;
        case '6': /* it's a PPM file */
            pbm = 0;
            spp = 3;
            photometric = PHOTOMETRIC_RGB;
            if (compression == COMPRESSION_JPEG &&
                jpegcolormode == JPEGCOLORMODE_RGB)
                photometric = PHOTOMETRIC_YCBCR;
            break;
        default:
            BadPPM(infile);
    }

    /* Parse header */
    while (1)
    {
        if (feof(in))
            BadPPM(infile);
        c = fgetc(in);
        /* Skip whitespaces (blanks, TABs, CRs, LFs) */
        if (strchr(" \t\r\n", c))
            continue;

        /* Check for comment line */
        if (c == '#')
        {
            do
            {
                c = fgetc(in);
            } while (!(strchr("\r\n", c) || feof(in)));
            continue;
        }

        ungetc(c, in);
        break;
    }
    if (pbm)
    {
        if (fscanf(in, " %u %u", &w, &h) != 2)
            BadPPM(infile);
        if (fgetc(in) != '\n')
            BadPPM(infile);
        bpp = 1;
        pack_func = pack_none;
    }
    else
    {
        if (fscanf(in, " %u %u %u", &w, &h, &prec) != 3)
            BadPPM(infile);
        if (fgetc(in) != '\n' || 0 == prec || 65535 < prec)
            BadPPM(infile);

        if (0 != (prec & (prec + 1)))
        {
            fprintf(stderr, "%s: unsupported maxval %u.\n", infile, prec);
            exit(EXIT_FAILURE);
        }
        bpp = 0;
        if ((prec + 1) & 0xAAAAAAAA)
            bpp |= 1;
        if ((prec + 1) & 0xCCCCCCCC)
            bpp |= 2;
        if ((prec + 1) & 0xF0F0F0F0)
            bpp |= 4;
        if ((prec + 1) & 0xFF00FF00)
            bpp |= 8;
        if ((prec + 1) & 0xFFFF0000)
            bpp |= 16;

        switch (bpp)
        {
            case 8:
                pack_func = pack_none;
                break;
            case 16:
            {
                const unsigned short i = 0x0100;

                if (0 == *(unsigned char *)&i)
                    pack_func = pack_swab;
                else
                    pack_func = pack_none;
            }
            break;
            default:
                if (8 >= bpp)
                    pack_func = pack_bytes;
                else
                    pack_func = pack_words;
                break;
        }
    }
    out = TIFFOpen(argv[optind], "w");
    if (out == NULL)
        return (EXIT_FAILURE);
    TIFFSetField(out, TIFFTAG_IMAGEWIDTH, (uint32_t)w);
    TIFFSetField(out, TIFFTAG_IMAGELENGTH, (uint32_t)h);
    TIFFSetField(out, TIFFTAG_ORIENTATION, ORIENTATION_TOPLEFT);
    TIFFSetField(out, TIFFTAG_SAMPLESPERPIXEL, spp);
    TIFFSetField(out, TIFFTAG_BITSPERSAMPLE, bpp);
    TIFFSetField(out, TIFFTAG_PLANARCONFIG, PLANARCONFIG_CONTIG);
    TIFFSetField(out, TIFFTAG_PHOTOMETRIC, photometric);
    TIFFSetField(out, TIFFTAG_COMPRESSION, compression);
    switch (compression)
    {
        case COMPRESSION_JPEG:
            TIFFSetField(out, TIFFTAG_JPEGQUALITY, quality);
            TIFFSetField(out, TIFFTAG_JPEGCOLORMODE, jpegcolormode);
            break;
        case COMPRESSION_LZW:
        case COMPRESSION_ADOBE_DEFLATE:
        case COMPRESSION_DEFLATE:
            if (predictor != 0)
                TIFFSetField(out, TIFFTAG_PREDICTOR, predictor);
            break;
        case COMPRESSION_CCITTFAX3:
            TIFFSetField(out, TIFFTAG_GROUP3OPTIONS, g3opts);
            break;
    }
    if (pbm)
    {
        /* if round-up overflows, result will be zero, OK */
        linebytes = (multiply_ms(spp, w) + (8 - 1)) / 8;
    }
    else if (bpp <= 8)
    {
        linebytes = multiply_ms(spp, w);
    }
    else
    {
        linebytes = multiply_ms(2 * spp, w);
    }
    if (rowsperstrip == (uint32_t)-1)
    {
        TIFFSetField(out, TIFFTAG_ROWSPERSTRIP, h);
    }
    else
    {
        TIFFSetField(out, TIFFTAG_ROWSPERSTRIP,
                     TIFFDefaultStripSize(out, rowsperstrip));
    }
    if (linebytes == 0)
    {
        fprintf(stderr, "%s: scanline size overflow\n", infile);
        (void)TIFFClose(out);
        exit(EXIT_FAILURE);
    }
    scanline_size = TIFFScanlineSize(out);
    if (scanline_size == 0)
    {
        /* overflow - TIFFScanlineSize already printed a message */
        (void)TIFFClose(out);
        exit(EXIT_FAILURE);
    }
    if (scanline_size < linebytes)
        buf = (unsigned char *)_TIFFmalloc(linebytes);
    else
        buf = (unsigned char *)_TIFFmalloc(scanline_size);
    if (buf == NULL)
    {
        fprintf(stderr, "%s: Not enough memory\n", infile);
        (void)TIFFClose(out);
        exit(EXIT_FAILURE);
    }
    if (resolution > 0)
    {
        TIFFSetField(out, TIFFTAG_XRESOLUTION, resolution);
        TIFFSetField(out, TIFFTAG_YRESOLUTION, resolution);
        TIFFSetField(out, TIFFTAG_RESOLUTIONUNIT, RESUNIT_INCH);
    }
    for (row = 0; row < h; row++)
    {
        if (fread(buf, linebytes, 1, in) != 1)
        {
            fprintf(stderr, "%s: scanline %u: Read error.\n", infile, row);
            break;
        }
        pack_func(buf, w * spp, bpp);
        if (TIFFWriteScanline(out, buf, row, 0) < 0)
            break;
    }
    if (in != stdin)
        fclose(in);
    (void)TIFFClose(out);
    if (buf)
        _TIFFfree(buf);
    return (EXIT_SUCCESS);
}

static void processG3Options(char *cp)
{
    g3opts = 0;
    if ((cp = strchr(cp, ':')))
    {
        do
        {
            cp++;
            if (strneq(cp, "1d", 2))
                g3opts &= ~GROUP3OPT_2DENCODING;
            else if (strneq(cp, "2d", 2))
                g3opts |= GROUP3OPT_2DENCODING;
            else if (strneq(cp, "fill", 4))
                g3opts |= GROUP3OPT_FILLBITS;
            else
                usage(EXIT_FAILURE);
        } while ((cp = strchr(cp, ':')));
    }
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
    else if (strneq(opt, "g3", 2))
    {
        processG3Options(opt);
        compression = COMPRESSION_CCITTFAX3;
    }
    else if (streq(opt, "g4"))
    {
        compression = COMPRESSION_CCITTFAX4;
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

static const char usage_info[] =
    "Create a TIFF file from PPM, PGM and PBM image files\n\n"
    "usage: ppm2tiff [options] input.ppm output.tif\n"
    "where options are:\n"
    " -r #		make each strip have no more than # rows\n"
    " -R #		set x&y resolution (dpi)\n"
    "\n"
#ifdef JPEG_SUPPORT
    " -c jpeg[:opts] compress output with JPEG encoding\n"
    /*     "JPEG options:\n" */
    "    #  set compression quality level (0-100, default 75)\n"
    "    r  output color image as RGB rather than YCbCr\n"
#endif
#ifdef LZW_SUPPORT
    " -c lzw[:opts]  compress output with Lempel-Ziv & Welch encoding\n"
    /* "    LZW options:\n" */
    "    #  set predictor value\n"
    "    For example, -c lzw:2 for LZW-encoded data with horizontal "
    "differencing\n"
#endif
#ifdef ZIP_SUPPORT
    " -c zip[:opts]  compress output with deflate encoding\n"
    /* "    Deflate (ZIP) options:\n" */
    "    #  set predictor value\n"
#endif
#ifdef PACKBITS_SUPPORT
    " -c packbits    compress output with packbits encoding (the default)\n"
#endif
#ifdef CCITT_SUPPORT
    " -c g3[:opts]   compress output with CCITT Group 3 encoding\n"
    " -c g4          compress output with CCITT Group 4 encoding\n"
#endif
#if defined(JPEG_SUPPORT) || defined(LZW_SUPPORT) || defined(ZIP_SUPPORT) ||   \
    defined(PACKBITS_SUPPORT) || defined(CCITT_SUPPORT)
    " -c none        use no compression algorithm on output\n"
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

