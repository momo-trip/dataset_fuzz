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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef HAVE_STRINGS_H
#include <strings.h>
#endif

#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif

#include "tiffiop.h"

#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif
#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif

static TIFFErrorHandler old_error_handler = 0;
static int status = EXIT_SUCCESS; /* exit status */
static int showdata = 0;          /* show data */
static int rawdata = 0;           /* show raw/decoded data */
static int showwords = 0;         /* show data as bytes/words */
static int readdata = 0;          /* read data in file */
static int stoponerr = 1;         /* stop on first read error */

static void usage(int);
static void tiffinfo(TIFF *, uint16_t, long, int);

#define DEFAULT_MAX_MALLOC (256 * 1024 * 1024)
/* malloc size limit (in bytes)
 * disabled when set to 0 */
static tmsize_t maxMalloc = DEFAULT_MAX_MALLOC;

static void PrivateErrorHandler(const char *module, const char *fmt, va_list ap)
{
    if (old_error_handler)
        (*old_error_handler)(module, fmt, ap);
    status = EXIT_FAILURE;
}

int original_main(int argc, char *argv[])
{
    int dirnum = -1, multiplefiles, c;
    uint16_t order = 0;
    TIFF *tif;
#if !HAVE_DECL_OPTARG
    extern int optind;
    extern char *optarg;
#endif
    long flags = 0;
    uint64_t diroff = 0;
    int chopstrips = 0; /* disable strip chopping */
    int warn_about_unknown_tags = FALSE;

    while ((c = getopt(argc, argv, "f:o:M:cdDSjilmrsvwz0123456789hW")) != -1)
        switch (c)
        {
            case '0':
            case '1':
            case '2':
            case '3':
            case '4':
            case '5':
            case '6':
            case '7':
            case '8':
            case '9':
                dirnum = atoi(&argv[optind - 1][1]);
                break;
            case 'd':
                showdata++;
                /* fall through... */
            case 'D':
                readdata++;
                break;
            case 'c':
                flags |= TIFFPRINT_COLORMAP | TIFFPRINT_CURVES;
                break;
            case 'f': /* fill order */
                if (streq(optarg, "lsb2msb"))
                    order = FILLORDER_LSB2MSB;
                else if (streq(optarg, "msb2lsb"))
                    order = FILLORDER_MSB2LSB;
                else
                    usage(EXIT_FAILURE);
                break;
            case 'i':
                stoponerr = 0;
                break;
            case 'M':
                maxMalloc = (tmsize_t)strtoul(optarg, NULL, 0) << 20;
                break;
            case 'o':
                diroff = strtoul(optarg, NULL, 0);
                break;
            case 'j':
                flags |= TIFFPRINT_JPEGQTABLES | TIFFPRINT_JPEGACTABLES |
                         TIFFPRINT_JPEGDCTABLES;
                break;
            case 'r':
                rawdata = 1;
                break;
            case 's':
                flags |= TIFFPRINT_STRIPS;
                break;
            case 'w':
                showwords = 1;
                break;
            case 'z':
                chopstrips = 1;
                break;
            case 'h':
                usage(EXIT_SUCCESS);
                /*NOTREACHED*/
                break;
            case 'W':
                warn_about_unknown_tags = TRUE;
                break;
            case '?':
                usage(EXIT_FAILURE);
                /*NOTREACHED*/
                break;
        }
    if (optind >= argc)
        usage(EXIT_FAILURE);

    old_error_handler = TIFFSetErrorHandler(PrivateErrorHandler);

    multiplefiles = (argc - optind > 1);
    for (; optind < argc; optind++)
    {
        if (multiplefiles)
            printf("File %s:\n", argv[optind]);
        TIFFOpenOptions *opts = TIFFOpenOptionsAlloc();
        if (opts == NULL)
        {
            status = EXIT_FAILURE;
            break;
        }
        TIFFOpenOptionsSetMaxSingleMemAlloc(opts, maxMalloc);
        TIFFOpenOptionsSetWarnAboutUnknownTags(opts, warn_about_unknown_tags);
        tif = TIFFOpenExt(argv[optind], chopstrips ? "rC" : "rc", opts);
        TIFFOpenOptionsFree(opts);
        if (tif != NULL)
        {
            if (dirnum != -1)
            {
                if (TIFFSetDirectory(tif, (tdir_t)dirnum))
                    tiffinfo(tif, order, flags, 1);
            }
            else if (diroff != 0)
            {
                if (TIFFSetSubDirectory(tif, diroff))
                    tiffinfo(tif, order, flags, 1);
            }
            else
            {
                do
                {
                    toff_t offset = 0;
                    tdir_t curdir = TIFFCurrentDirectory(tif);
                    printf("=== TIFF directory %u ===\n", curdir);
                    tiffinfo(tif, order, flags, 1);
                    if (TIFFGetField(tif, TIFFTAG_EXIFIFD, &offset))
                    {
                        printf("--- EXIF directory within directory %u \n",
                               curdir);
                        if (TIFFReadEXIFDirectory(tif, offset))
                        {
                            tiffinfo(tif, order, flags, 0);
                            /*-- Go back to previous directory, (directory is
                             * reloaded from file!) */
                            TIFFSetDirectory(tif, curdir);
                        }
                    }
                    if (TIFFGetField(tif, TIFFTAG_GPSIFD, &offset))
                    {
                        printf("--- GPS directory within directory %u \n",
                               curdir);
                        if (TIFFReadGPSDirectory(tif, offset))
                        {
                            tiffinfo(tif, order, flags, 0);
                            TIFFSetDirectory(tif, curdir);
                        }
                    }
                    /*-- Check for SubIFDs --*/
                    uint16_t nCount;
                    void *vPtr;
                    uint64_t *subIFDoffsets = NULL;
                    if (TIFFGetField(tif, TIFFTAG_SUBIFD, &nCount, &vPtr))
                    {
                        if (nCount > 0)
                        {
                            subIFDoffsets = malloc(nCount * sizeof(uint64_t));
                            if (subIFDoffsets != NULL)
                            {
                                memcpy(subIFDoffsets, vPtr,
                                       nCount * sizeof(subIFDoffsets[0]));
                                printf("--- SubIFD image descriptor tag within "
                                       "TIFF directory %u with array of %d "
                                       "SubIFD chains ---\n",
                                       curdir, nCount);
                                for (int i = 0; i < nCount; i++)
                                {
                                    offset = subIFDoffsets[i];
                                    int s = 0;
                                    if (TIFFSetSubDirectory(tif, offset))
                                    {
                                        /* print info and check for SubIFD chain
                                         */
                                        do
                                        {
                                            printf("--- SubIFD %d of chain %d "
                                                   "at offset 0x%" PRIx64
                                                   " (%" PRIu64 "):\n",
                                                   s, i, offset, offset);
                                            tiffinfo(tif, order, flags, 0);
                                            s++;
                                        } while (TIFFReadDirectory(tif));
                                    }
                                }
                                TIFFSetDirectory(tif, curdir);
                                free(subIFDoffsets);
                                subIFDoffsets = NULL;
                            }
                            else
                            {
                                fprintf(stderr,
                                        "Error: Could not allocate memory for "
                                        "SubIFDs list. SubIFDs not parsed.\n");
                            }
                        }
                    }
                    printf("\n");
                } while (TIFFReadDirectory(tif));
            }
            TIFFClose(tif);
        }
    }
    return (status);
}

static const char usage_info[] =
    "Display information about TIFF files\n\n"
    "usage: tiffinfo [options] input...\n"
    "where options are:\n"
    " -D		read data\n"
    " -i		ignore read errors\n"
    " -c		display data for grey/color response curve or "
    "colormap\n"
    " -d		display raw/decoded image data\n"
    " -f lsb2msb	force lsb-to-msb FillOrder for input\n"
    " -f msb2lsb	force msb-to-lsb FillOrder for input\n"
    " -j		show JPEG tables\n"
    " -o offset	set initial directory offset\n"
    " -r		read/display raw image data instead of decoded data\n"
    " -s		display strip offsets and byte counts\n"
    " -w		display raw data in words rather than bytes\n"
    " -z		enable strip chopping\n"
    " -M size	set the memory allocation limit in MiB. 0 to disable limit\n"
    " -#		set initial directory (first directory is # 0)\n"
    " -w		show image data as 16-bit words\n"
    " -W		warn about unknown tags\n";

static void usage(int code)
{
    FILE *out = (code == EXIT_SUCCESS) ? stdout : stderr;

    fprintf(out, "%s\n\n", TIFFGetVersion());
    fprintf(out, "%s", usage_info);
    exit(code);
}

static void ShowStrip(tstrip_t strip, unsigned char *pp, uint32_t nrow,
                      tsize_t scanline)
{
    register tsize_t cc;

    printf("Strip %" PRIu32 ":\n", strip);
    while (nrow-- > 0)
    {
        for (cc = 0; cc < scanline; cc++)
        {
            printf(" %02x", *pp++);
            if (((cc + 1) % 24) == 0)
                putchar('\n');
        }
        putchar('\n');
    }
}

void TIFFReadContigStripData(TIFF *tif)
{
    unsigned char *buf;
    tsize_t scanline = TIFFScanlineSize(tif);
    tmsize_t stripsize = TIFFStripSize(tif);

    if (maxMalloc != 0 && stripsize > maxMalloc)
    {
        fprintf(stderr,
                "Memory allocation attempt %" TIFF_SSIZE_FORMAT
                " over memory limit (%" TIFF_SSIZE_FORMAT ")\n",
                stripsize, maxMalloc);
        return;
    }
    buf = (unsigned char *)_TIFFmalloc(stripsize);
    if (buf)
    {
        uint32_t row, h = 0;
        uint32_t rowsperstrip = (uint32_t)-1;

        TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &h);
        TIFFGetField(tif, TIFFTAG_ROWSPERSTRIP, &rowsperstrip);
        for (row = 0; row < h; row += rowsperstrip)
        {
            uint32_t nrow = (row + rowsperstrip > h ? h - row : rowsperstrip);
            tstrip_t strip = TIFFComputeStrip(tif, row, 0);
            if (TIFFReadEncodedStrip(tif, strip, buf, nrow * scanline) < 0)
            {
                if (stoponerr)
                    break;
            }
            else if (showdata)
                ShowStrip(strip, buf, nrow, scanline);
        }
        _TIFFfree(buf);
    }
    else
    {
        fprintf(stderr, "Cannot allocate %" TIFF_SSIZE_FORMAT " bytes.\n",
                stripsize);
    }
}

void TIFFReadSeparateStripData(TIFF *tif)
{
    unsigned char *buf;
    tsize_t scanline = TIFFScanlineSize(tif);
    tmsize_t stripsize = TIFFStripSize(tif);

    if (maxMalloc != 0 && stripsize > maxMalloc)
    {
        fprintf(stderr,
                "Memory allocation attempt %" TIFF_SSIZE_FORMAT
                " over memory limit (%" TIFF_SSIZE_FORMAT ")\n",
                stripsize, maxMalloc);
        return;
    }
    buf = (unsigned char *)_TIFFmalloc(stripsize);
    if (buf)
    {
        uint32_t row, h = 0;
        uint32_t rowsperstrip = (uint32_t)-1;
        tsample_t s, samplesperpixel = 0;

        TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &h);
        TIFFGetField(tif, TIFFTAG_ROWSPERSTRIP, &rowsperstrip);
        TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &samplesperpixel);
        for (row = 0; row < h; row += rowsperstrip)
        {
            for (s = 0; s < samplesperpixel; s++)
            {
                uint32_t nrow =
                    (row + rowsperstrip > h ? h - row : rowsperstrip);
                tstrip_t strip = TIFFComputeStrip(tif, row, s);
                if (TIFFReadEncodedStrip(tif, strip, buf, nrow * scanline) < 0)
                {
                    if (stoponerr)
                        break;
                }
                else if (showdata)
                    ShowStrip(strip, buf, nrow, scanline);
            }
        }
        _TIFFfree(buf);
    }
    else
    {
        fprintf(stderr, "Cannot allocate %" TIFF_SSIZE_FORMAT " bytes.\n",
                stripsize);
    }
}

static void ShowTile(uint32_t row, uint32_t col, tsample_t sample,
                     unsigned char *pp, uint32_t nrow, tsize_t rowsize)
{
    uint32_t cc;

    printf("Tile (%" PRIu32 ",%" PRIu32 "", row, col);
    if (sample != (tsample_t)-1)
        printf(",%" PRIu16, sample);
    printf("):\n");
    while (nrow-- > 0)
    {
        for (cc = 0; cc < (uint32_t)rowsize; cc++)
        {
            printf(" %02x", *pp++);
            if (((cc + 1) % 24) == 0)
                putchar('\n');
        }
        putchar('\n');
    }
}

void TIFFReadContigTileData(TIFF *tif)
{
    unsigned char *buf;
    tmsize_t rowsize = TIFFTileRowSize(tif);
    tmsize_t tilesize = TIFFTileSize(tif);

    if (maxMalloc != 0 && tilesize > maxMalloc)
    {
        fprintf(stderr,
                "Memory allocation attempt %" TIFF_SSIZE_FORMAT
                " over memory limit (%" TIFF_SSIZE_FORMAT ")\n",
                tilesize, maxMalloc);
        return;
    }
    buf = (unsigned char *)_TIFFmalloc(tilesize);
    if (buf)
    {
        uint32_t tw = 0, th = 0, w = 0, h = 0;
        uint32_t row, col;

        TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &w);
        TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &h);
        TIFFGetField(tif, TIFFTAG_TILEWIDTH, &tw);
        TIFFGetField(tif, TIFFTAG_TILELENGTH, &th);
        if (rowsize == 0 || th > (size_t)(tilesize / rowsize))
        {
            fprintf(stderr, "Cannot display data: th * rowsize > tilesize\n");
            _TIFFfree(buf);
            return;
        }
        for (row = 0; row < h; row += th)
        {
            for (col = 0; col < w; col += tw)
            {
                if (TIFFReadTile(tif, buf, col, row, 0, 0) < 0)
                {
                    if (stoponerr)
                        break;
                }
                else if (showdata)
                    ShowTile(row, col, (tsample_t)-1, buf, th, rowsize);
            }
        }
        _TIFFfree(buf);
    }
    else
    {
        fprintf(stderr, "Cannot allocate %" TIFF_SSIZE_FORMAT " bytes.\n",
                tilesize);
    }
}

void TIFFReadSeparateTileData(TIFF *tif)
{
    unsigned char *buf;
    tmsize_t rowsize = TIFFTileRowSize(tif);
    tmsize_t tilesize = TIFFTileSize(tif);

    if (maxMalloc != 0 && tilesize > maxMalloc)
    {
        fprintf(stderr,
                "Memory allocation attempt %" TIFF_SSIZE_FORMAT
                " over memory limit (%" TIFF_SSIZE_FORMAT ")\n",
                tilesize, maxMalloc);
        return;
    }
    buf = (unsigned char *)_TIFFmalloc(tilesize);
    if (buf)
    {
        uint32_t tw = 0, th = 0, w = 0, h = 0;
        uint32_t row, col;
        tsample_t s, samplesperpixel = 0;

        TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &w);
        TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &h);
        TIFFGetField(tif, TIFFTAG_TILEWIDTH, &tw);
        TIFFGetField(tif, TIFFTAG_TILELENGTH, &th);
        TIFFGetField(tif, TIFFTAG_SAMPLESPERPIXEL, &samplesperpixel);
        if (rowsize == 0 || th > (size_t)(tilesize / rowsize))
        {
            fprintf(stderr, "Cannot display data: th * rowsize > tilesize\n");
            _TIFFfree(buf);
            return;
        }
        for (row = 0; row < h; row += th)
        {
            for (col = 0; col < w; col += tw)
            {
                for (s = 0; s < samplesperpixel; s++)
                {
                    if (TIFFReadTile(tif, buf, col, row, 0, s) < 0)
                    {
                        if (stoponerr)
                            break;
                    }
                    else if (showdata)
                        ShowTile(row, col, s, buf, th, rowsize);
                }
            }
        }
        _TIFFfree(buf);
    }
    else
    {
        fprintf(stderr, "Cannot allocate %" TIFF_SSIZE_FORMAT " bytes.\n",
                tilesize);
    }
}

void TIFFReadData(TIFF *tif)
{
    uint16_t config = PLANARCONFIG_CONTIG;

    TIFFGetField(tif, TIFFTAG_PLANARCONFIG, &config);
    if (TIFFIsTiled(tif))
    {
        if (config == PLANARCONFIG_CONTIG)
            TIFFReadContigTileData(tif);
        else
            TIFFReadSeparateTileData(tif);
    }
    else
    {
        if (config == PLANARCONFIG_CONTIG)
            TIFFReadContigStripData(tif);
        else
            TIFFReadSeparateStripData(tif);
    }
}

static void ShowRawBytes(unsigned char *pp, uint32_t n)
{
    uint32_t i;

    for (i = 0; i < n; i++)
    {
        printf(" %02x", *pp++);
        if (((i + 1) % 24) == 0)
            printf("\n ");
    }
    putchar('\n');
}

static void ShowRawWords(uint16_t *pp, uint32_t n)
{
    uint32_t i;

    for (i = 0; i < n; i++)
    {
        printf(" %04" PRIx16, *pp++);
        if (((i + 1) % 15) == 0)
            printf("\n ");
    }
    putchar('\n');
}

static void TIFFReadRawDataStriped(TIFF *tif, int bitrev)
{
    tstrip_t nstrips = TIFFNumberOfStrips(tif);
    const char *what = "Strip";
    uint64_t *stripbc = NULL;

    TIFFGetField(tif, TIFFTAG_STRIPBYTECOUNTS, &stripbc);
    if (stripbc != NULL && nstrips > 0)
    {
        uint32_t bufsize = 0;
        tdata_t buf = NULL;
        tstrip_t s;

        for (s = 0; s < nstrips; s++)
        {
            if (stripbc[s] > bufsize || buf == NULL)
            {
                tdata_t newbuf;
                if (maxMalloc != 0 && stripbc[s] > (uint64_t)maxMalloc)
                {
                    fprintf(stderr,
                            "Memory allocation attempt %" TIFF_SSIZE_FORMAT
                            " over memory limit (%" TIFF_SSIZE_FORMAT ")\n",
                            (tmsize_t)stripbc[s], maxMalloc);
                    break;
                }
                newbuf = _TIFFrealloc(buf, (tmsize_t)stripbc[s]);
                if (newbuf == NULL)
                {
                    fprintf(stderr,
                            "Cannot allocate buffer to read strip %" PRIu32
                            "\n",
                            s);
                    break;
                }
                bufsize = (uint32_t)stripbc[s];
                buf = newbuf;
            }
            if (TIFFReadRawStrip(tif, s, buf, (tmsize_t)stripbc[s]) < 0)
            {
                fprintf(stderr, "Error reading strip %" PRIu32 "\n", s);
                if (stoponerr)
                    break;
            }
            else if (showdata)
            {
                if (bitrev)
                {
                    TIFFReverseBits(buf, (tmsize_t)stripbc[s]);
                    printf("%s %" PRIu32 ": (bit reversed)\n ", what, s);
                }
                else
                    printf("%s %" PRIu32 ":\n ", what, s);
                if (showwords)
                    ShowRawWords((uint16_t *)buf, (uint32_t)stripbc[s] >> 1);
                else
                    ShowRawBytes((unsigned char *)buf, (uint32_t)stripbc[s]);
            }
        }
        if (buf != NULL)
            _TIFFfree(buf);
    }
}

static void TIFFReadRawDataTiled(TIFF *tif, int bitrev)
{
    const char *what = "Tile";
    uint32_t ntiles = TIFFNumberOfTiles(tif);
    uint64_t *tilebc = NULL;

    TIFFGetField(tif, TIFFTAG_TILEBYTECOUNTS, &tilebc);
    if (tilebc != NULL && ntiles > 0)
    {
        uint64_t bufsize = 0;
        tdata_t buf = NULL;
        uint32_t t;

        for (t = 0; t < ntiles; t++)
        {
            if (tilebc[t] > bufsize || buf == NULL)
            {
                tdata_t newbuf;
                if (maxMalloc != 0 && tilebc[t] > (uint64_t)maxMalloc)
                {
                    fprintf(stderr,
                            "Memory allocation attempt %" TIFF_SSIZE_FORMAT
                            " over memory limit (%" TIFF_SSIZE_FORMAT ")\n",
                            (tmsize_t)tilebc[t], maxMalloc);
                    break;
                }
                newbuf = _TIFFrealloc(buf, (tmsize_t)tilebc[t]);
                if (newbuf == NULL)
                {
                    fprintf(stderr,
                            "Cannot allocate buffer to read tile %" PRIu32 "\n",
                            t);
                    break;
                }
                bufsize = (uint32_t)tilebc[t];
                buf = newbuf;
            }
            if (TIFFReadRawTile(tif, t, buf, (tmsize_t)tilebc[t]) < 0)
            {
                fprintf(stderr, "Error reading tile %" PRIu32 "\n", t);
                if (stoponerr)
                    break;
            }
            else if (showdata)
            {
                if (bitrev)
                {
                    TIFFReverseBits(buf, (tmsize_t)tilebc[t]);
                    printf("%s %" PRIu32 ": (bit reversed)\n ", what, t);
                }
                else
                {
                    printf("%s %" PRIu32 ":\n ", what, t);
                }
                if (showwords)
                {
                    ShowRawWords((uint16_t *)buf, (uint32_t)(tilebc[t] >> 1));
                }
                else
                {
                    ShowRawBytes((unsigned char *)buf, (uint32_t)tilebc[t]);
                }
            }
        }
        if (buf != NULL)
            _TIFFfree(buf);
    }
}

void TIFFReadRawData(TIFF *tif, int bitrev)
{
    if (TIFFIsTiled(tif))
    {
        TIFFReadRawDataTiled(tif, bitrev);
    }
    else
    {
        TIFFReadRawDataStriped(tif, bitrev);
    }
}

static void tiffinfo(TIFF *tif, uint16_t order, long flags, int is_image)
{
    TIFFPrintDirectory(tif, stdout, flags);
    if (!readdata || !is_image)
        return;
    if (rawdata)
    {
        if (order)
        {
            uint16_t o;
            TIFFGetFieldDefaulted(tif, TIFFTAG_FILLORDER, &o);
            TIFFReadRawData(tif, o != order);
        }
        else
            TIFFReadRawData(tif, 0);
    }
    else
    {
        if (order)
            TIFFSetField(tif, TIFFTAG_FILLORDER, order);
        TIFFReadData(tif);
    }
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

