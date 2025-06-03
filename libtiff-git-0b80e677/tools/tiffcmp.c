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

#include <math.h>
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

static int stopondiff = 1;
static int stoponfirsttag = 1;
static uint16_t bitspersample = 1;
static uint16_t samplesperpixel = 1;
static uint16_t sampleformat = SAMPLEFORMAT_UINT;
static uint32_t imagewidth;
static uint32_t imagelength;

static void usage(int code);
static int tiffcmp(TIFF *, TIFF *);
static int cmptags(TIFF *, TIFF *);
static int ContigCompare(int, uint32_t, unsigned char *, unsigned char *,
                         tsize_t);
static int SeparateCompare(int, int, uint32_t, unsigned char *,
                           unsigned char *);
static void PrintIntDiff(uint32_t, int, uint32_t, uint32_t, uint32_t);
static void PrintFloatDiff(uint32_t, int, uint32_t, double, double);

static void leof(const char *, uint32_t, int);

/*
 * exit with status :
 * 0    No differences were found.
 * 1    Differences were found.
 * >1   An error occurred.
 */
int original_main(int argc, char *argv[])
{
    TIFF *tif1, *tif2;
    int c, dirnum;
#if !HAVE_DECL_OPTARG
    extern int optind;
    extern char *optarg;
#endif

    while ((c = getopt(argc, argv, "ltz:h")) != -1)
        switch (c)
        {
            case 'l':
                stopondiff = 0;
                break;
            case 'z':
                stopondiff = atoi(optarg);
                break;
            case 't':
                stoponfirsttag = 0;
                break;
            case 'h':
                usage(EXIT_SUCCESS);
                break;
            case '?':
                usage(2);
                /*NOTREACHED*/
                break;
        }
    if (argc - optind < 2)
        usage(2);
    tif1 = TIFFOpen(argv[optind], "r");
    if (tif1 == NULL)
        return (2);
    tif2 = TIFFOpen(argv[optind + 1], "r");
    if (tif2 == NULL)
        return (2);
    dirnum = 0;
    while (tiffcmp(tif1, tif2))
    {
        if (!TIFFReadDirectory(tif1))
        {
            if (!TIFFReadDirectory(tif2))
                break;
            printf("No more directories for %s\n", TIFFFileName(tif1));
            return (1);
        }
        else if (!TIFFReadDirectory(tif2))
        {
            printf("No more directories for %s\n", TIFFFileName(tif2));
            return (1);
        }
        printf("Directory %d:\n", ++dirnum);
    }

    TIFFClose(tif1);
    TIFFClose(tif2);
    return (0);
}

static const char usage_info[] =
    "Compare the tags and data in two TIFF files\n\n"
    "usage: tiffcmp [options] file1 file2\n"
    "where options are:\n"
    " -l		list each byte of image data that differs between the "
    "files\n"
    " -z #		list specified number of bytes that differs between "
    "the files\n"
    " -t		ignore any differences in directory tags\n";

static void usage(int code)
{
    FILE *out = (code == EXIT_SUCCESS) ? stdout : stderr;

    fprintf(out, "%s\n\n", TIFFGetVersion());
    fprintf(out, "%s", usage_info);
    exit(code);
}

#define checkEOF(tif, row, sample)                                             \
    {                                                                          \
        leof(TIFFFileName(tif), row, sample);                                  \
        goto bad;                                                              \
    }

static int CheckShortTag(TIFF *, TIFF *, int, char *);
static int CheckShort2Tag(TIFF *, TIFF *, int, char *);
static int CheckShortArrayTag(TIFF *, TIFF *, int, char *);
static int CheckLongTag(TIFF *, TIFF *, int, char *);
static int CheckFloatTag(TIFF *, TIFF *, int, char *);
static int CheckStringTag(TIFF *, TIFF *, int, char *);

static int tiffcmp(TIFF *tif1, TIFF *tif2)
{
    uint16_t config1, config2;
    tsize_t size1;
    uint32_t row;
    tsample_t s;
    unsigned char *buf1, *buf2;

    if (!CheckShortTag(tif1, tif2, TIFFTAG_BITSPERSAMPLE, "BitsPerSample"))
        return (0);
    if (!CheckShortTag(tif1, tif2, TIFFTAG_SAMPLESPERPIXEL, "SamplesPerPixel"))
        return (0);
    if (!CheckLongTag(tif1, tif2, TIFFTAG_IMAGEWIDTH, "ImageWidth"))
        return (0);
    if (!cmptags(tif1, tif2))
        return (1);
    (void)TIFFGetField(tif1, TIFFTAG_BITSPERSAMPLE, &bitspersample);
    (void)TIFFGetField(tif1, TIFFTAG_SAMPLESPERPIXEL, &samplesperpixel);
    (void)TIFFGetField(tif1, TIFFTAG_SAMPLEFORMAT, &sampleformat);
    (void)TIFFGetField(tif1, TIFFTAG_IMAGEWIDTH, &imagewidth);
    (void)TIFFGetField(tif1, TIFFTAG_IMAGELENGTH, &imagelength);
    (void)TIFFGetField(tif1, TIFFTAG_PLANARCONFIG, &config1);
    (void)TIFFGetField(tif2, TIFFTAG_PLANARCONFIG, &config2);
    buf1 = (unsigned char *)_TIFFmalloc(size1 = TIFFScanlineSize(tif1));
    buf2 = (unsigned char *)_TIFFmalloc(TIFFScanlineSize(tif2));
    if (buf1 == NULL || buf2 == NULL)
    {
        fprintf(stderr, "No space for scanline buffers\n");
        exit(2);
    }
    if (config1 != config2 && bitspersample != 8 && samplesperpixel > 1)
    {
        fprintf(stderr, "Can't handle different planar configuration w/ "
                        "different bits/sample\n");
        goto bad;
    }
#define pack(a, b) ((a) << 8) | (b)
    switch (pack(config1, config2))
    {
        case pack(PLANARCONFIG_SEPARATE, PLANARCONFIG_CONTIG):
            for (row = 0; row < imagelength; row++)
            {
                if (TIFFReadScanline(tif2, buf2, row, 0) < 0)
                    checkEOF(tif2, row, -1) for (s = 0; s < samplesperpixel;
                                                 s++)
                    {
                        if (TIFFReadScanline(tif1, buf1, row, s) < 0)
                            checkEOF(tif1, row, s) if (SeparateCompare(
                                                           1, s, row, buf2,
                                                           buf1) < 0) goto bad1;
                    }
            }
            break;
        case pack(PLANARCONFIG_CONTIG, PLANARCONFIG_SEPARATE):
            for (row = 0; row < imagelength; row++)
            {
                if (TIFFReadScanline(tif1, buf1, row, 0) < 0)
                    checkEOF(tif1, row, -1) for (s = 0; s < samplesperpixel;
                                                 s++)
                    {
                        if (TIFFReadScanline(tif2, buf2, row, s) < 0)
                            checkEOF(tif2, row, s) if (SeparateCompare(
                                                           0, s, row, buf1,
                                                           buf2) < 0) goto bad1;
                    }
            }
            break;
        case pack(PLANARCONFIG_SEPARATE, PLANARCONFIG_SEPARATE):
            for (s = 0; s < samplesperpixel; s++)
                for (row = 0; row < imagelength; row++)
                {
                    if (TIFFReadScanline(tif1, buf1, row, s) < 0)
                        checkEOF(tif1, row, s) if (TIFFReadScanline(tif2, buf2,
                                                                    row, s) < 0)
                            checkEOF(tif2, row,
                                     s) if (ContigCompare(s, row, buf1, buf2,
                                                          size1) < 0) goto bad1;
                }
            break;
        case pack(PLANARCONFIG_CONTIG, PLANARCONFIG_CONTIG):
            for (row = 0; row < imagelength; row++)
            {
                if (TIFFReadScanline(tif1, buf1, row, 0) < 0)
                    checkEOF(tif1, row,
                             -1) if (TIFFReadScanline(tif2, buf2, row, 0) < 0)
                        checkEOF(tif2, row,
                                 -1) if (ContigCompare(-1, row, buf1, buf2,
                                                       size1) < 0) goto bad1;
            }
            break;
    }
    if (buf1)
        _TIFFfree(buf1);
    if (buf2)
        _TIFFfree(buf2);
    return (1);
bad:
    if (stopondiff)
        exit(1);
bad1:
    if (buf1)
        _TIFFfree(buf1);
    if (buf2)
        _TIFFfree(buf2);
    return (0);
}

#define CmpShortField(tag, name)                                               \
    if (!CheckShortTag(tif1, tif2, tag, name) && stoponfirsttag)               \
    return (0)
#define CmpShortField2(tag, name)                                              \
    if (!CheckShort2Tag(tif1, tif2, tag, name) && stoponfirsttag)              \
    return (0)
#define CmpLongField(tag, name)                                                \
    if (!CheckLongTag(tif1, tif2, tag, name) && stoponfirsttag)                \
    return (0)
#define CmpFloatField(tag, name)                                               \
    if (!CheckFloatTag(tif1, tif2, tag, name) && stoponfirsttag)               \
    return (0)
#define CmpStringField(tag, name)                                              \
    if (!CheckStringTag(tif1, tif2, tag, name) && stoponfirsttag)              \
    return (0)
#define CmpShortArrayField(tag, name)                                          \
    if (!CheckShortArrayTag(tif1, tif2, tag, name) && stoponfirsttag)          \
    return (0)

static int cmptags(TIFF *tif1, TIFF *tif2)
{
    uint16_t compression1, compression2;
    CmpLongField(TIFFTAG_SUBFILETYPE, "SubFileType");
    CmpLongField(TIFFTAG_IMAGEWIDTH, "ImageWidth");
    CmpLongField(TIFFTAG_IMAGELENGTH, "ImageLength");
    CmpShortField(TIFFTAG_BITSPERSAMPLE, "BitsPerSample");
    CmpShortField(TIFFTAG_COMPRESSION, "Compression");
    CmpShortField(TIFFTAG_PREDICTOR, "Predictor");
    CmpShortField(TIFFTAG_PHOTOMETRIC, "PhotometricInterpretation");
    CmpShortField(TIFFTAG_THRESHHOLDING, "Thresholding");
    CmpShortField(TIFFTAG_FILLORDER, "FillOrder");
    CmpShortField(TIFFTAG_ORIENTATION, "Orientation");
    CmpShortField(TIFFTAG_SAMPLESPERPIXEL, "SamplesPerPixel");
    CmpShortField(TIFFTAG_MINSAMPLEVALUE, "MinSampleValue");
    CmpShortField(TIFFTAG_MAXSAMPLEVALUE, "MaxSampleValue");
    CmpShortField(TIFFTAG_SAMPLEFORMAT, "SampleFormat");
    CmpFloatField(TIFFTAG_XRESOLUTION, "XResolution");
    CmpFloatField(TIFFTAG_YRESOLUTION, "YResolution");
    if (TIFFGetField(tif1, TIFFTAG_COMPRESSION, &compression1) &&
        compression1 == COMPRESSION_CCITTFAX3 &&
        TIFFGetField(tif2, TIFFTAG_COMPRESSION, &compression2) &&
        compression2 == COMPRESSION_CCITTFAX3)
    {
        CmpLongField(TIFFTAG_GROUP3OPTIONS, "Group3Options");
    }
    if (TIFFGetField(tif1, TIFFTAG_COMPRESSION, &compression1) &&
        compression1 == COMPRESSION_CCITTFAX4 &&
        TIFFGetField(tif2, TIFFTAG_COMPRESSION, &compression2) &&
        compression2 == COMPRESSION_CCITTFAX4)
    {
        CmpLongField(TIFFTAG_GROUP4OPTIONS, "Group4Options");
    }
    CmpShortField(TIFFTAG_RESOLUTIONUNIT, "ResolutionUnit");
    CmpShortField(TIFFTAG_PLANARCONFIG, "PlanarConfiguration");
    CmpLongField(TIFFTAG_ROWSPERSTRIP, "RowsPerStrip");
    CmpFloatField(TIFFTAG_XPOSITION, "XPosition");
    CmpFloatField(TIFFTAG_YPOSITION, "YPosition");
    CmpShortField(TIFFTAG_GRAYRESPONSEUNIT, "GrayResponseUnit");
    CmpShortField(TIFFTAG_COLORRESPONSEUNIT, "ColorResponseUnit");
#ifdef notdef
    {
        uint16_t *graycurve;
        CmpField(TIFFTAG_GRAYRESPONSECURVE, graycurve);
    }
    {
        uint16_t *red, *green, *blue;
        CmpField3(TIFFTAG_COLORRESPONSECURVE, red, green, blue);
    }
    {
        uint16_t *red, *green, *blue;
        CmpField3(TIFFTAG_COLORMAP, red, green, blue);
    }
#endif
    CmpShortField2(TIFFTAG_PAGENUMBER, "PageNumber");
    CmpStringField(TIFFTAG_ARTIST, "Artist");
    CmpStringField(TIFFTAG_IMAGEDESCRIPTION, "ImageDescription");
    CmpStringField(TIFFTAG_MAKE, "Make");
    CmpStringField(TIFFTAG_MODEL, "Model");
    CmpStringField(TIFFTAG_SOFTWARE, "Software");
    CmpStringField(TIFFTAG_DATETIME, "DateTime");
    CmpStringField(TIFFTAG_HOSTCOMPUTER, "HostComputer");
    CmpStringField(TIFFTAG_PAGENAME, "PageName");
    CmpStringField(TIFFTAG_DOCUMENTNAME, "DocumentName");
    CmpShortField(TIFFTAG_MATTEING, "Matteing");
    CmpShortArrayField(TIFFTAG_EXTRASAMPLES, "ExtraSamples");
    return (1);
}

static int ContigCompare(int sample, uint32_t row, unsigned char *p1,
                         unsigned char *p2, tsize_t size)
{
    uint32_t pix;
    int samples_to_test;

    if (memcmp(p1, p2, size) == 0)
        return 0;

    samples_to_test = (sample == -1) ? samplesperpixel : 1;

    switch (bitspersample)
    {
        case 1:
        case 2:
        case 4:
        case 8:
        {
            unsigned char *pix1 = p1, *pix2 = p2;
            unsigned bits = 0;

            for (pix = 0; pix < imagewidth; pix++)
            {
                int s;

                for (s = 0; s < samples_to_test; s++)
                {
                    if (*pix1 != *pix2)
                    {
                        if (sample == -1)
                            PrintIntDiff(row, s, pix, *pix1, *pix2);
                        else
                            PrintIntDiff(row, sample, pix, *pix1, *pix2);
                    }

                    bits += bitspersample;
                    pix1 += (bits / 8);
                    pix2 += (bits / 8);
                    bits &= 7;
                }
            }
            break;
        }
        case 16:
        {
            uint16_t *pix1 = (uint16_t *)p1, *pix2 = (uint16_t *)p2;

            for (pix = 0; pix < imagewidth; pix++)
            {
                int s;

                for (s = 0; s < samples_to_test; s++)
                {
                    if (*pix1 != *pix2)
                        PrintIntDiff(row, sample, pix, *pix1, *pix2);

                    pix1++;
                    pix2++;
                }
            }
            break;
        }
        case 32:
            if (sampleformat == SAMPLEFORMAT_UINT ||
                sampleformat == SAMPLEFORMAT_INT)
            {
                uint32_t *pix1 = (uint32_t *)p1, *pix2 = (uint32_t *)p2;

                for (pix = 0; pix < imagewidth; pix++)
                {
                    int s;

                    for (s = 0; s < samples_to_test; s++)
                    {
                        if (*pix1 != *pix2)
                        {
                            PrintIntDiff(row, sample, pix, *pix1, *pix2);
                        }

                        pix1++;
                        pix2++;
                    }
                }
            }
            else if (sampleformat == SAMPLEFORMAT_IEEEFP)
            {
                float *pix1 = (float *)p1, *pix2 = (float *)p2;

                for (pix = 0; pix < imagewidth; pix++)
                {
                    int s;

                    for (s = 0; s < samples_to_test; s++)
                    {
                        if (fabs(*pix1 - *pix2) < 0.000000000001)
                        {
                            PrintFloatDiff(row, sample, pix, *pix1, *pix2);
                        }

                        pix1++;
                        pix2++;
                    }
                }
            }
            else
            {
                fprintf(stderr, "Sample format %" PRIu16 " is not supported.\n",
                        sampleformat);
                return -1;
            }
            break;
        default:
            fprintf(stderr, "Bit depth %" PRIu16 " is not supported.\n",
                    bitspersample);
            return -1;
    }

    return 0;
}

static void PrintIntDiff(uint32_t row, int sample, uint32_t pix, uint32_t w1,
                         uint32_t w2)
{
    if (sample < 0)
        sample = 0;
    switch (bitspersample)
    {
        case 1:
        case 2:
        case 4:
        {
            int32_t mask1, mask2, s;

            /* mask1 should have the n lowest bits set, where n == bitspersample
             */
            mask1 = ((int32_t)1 << bitspersample) - 1;
            s = (8 - bitspersample);
            mask2 = mask1 << s;
            for (; mask2 && pix < imagewidth;
                 mask2 >>= bitspersample, s -= bitspersample, pix++)
            {
                if ((w1 & mask2) ^ (w2 & mask2))
                {
                    printf("Scanline %" PRIu32 ", pixel %" PRIu32
                           ", sample %d: %01" PRIx32 " %01" PRIx32 "\n",
                           row, pix, sample, (w1 >> s) & mask1,
                           (w2 >> s) & mask1);
                    if (--stopondiff == 0)
                        exit(1);
                }
            }
            break;
        }
        case 8:
            printf("Scanline %" PRIu32 ", pixel %" PRIu32
                   ", sample %d: %02" PRIx32 " %02" PRIx32 "\n",
                   row, pix, sample, w1, w2);
            if (--stopondiff == 0)
                exit(1);
            break;
        case 16:
            printf("Scanline %" PRIu32 ", pixel %" PRIu32
                   ", sample %d: %04" PRIx32 " %04" PRIx32 "\n",
                   row, pix, sample, w1, w2);
            if (--stopondiff == 0)
                exit(1);
            break;
        case 32:
            printf("Scanline %" PRIu32 ", pixel %" PRIu32
                   ", sample %d: %08" PRIx32 " %08" PRIx32 "\n",
                   row, pix, sample, w1, w2);
            if (--stopondiff == 0)
                exit(1);
            break;
        default:
            break;
    }
}

static void PrintFloatDiff(uint32_t row, int sample, uint32_t pix, double w1,
                           double w2)
{
    if (sample < 0)
        sample = 0;
    switch (bitspersample)
    {
        case 32:
            printf("Scanline %" PRIu32 ", pixel %" PRIu32
                   ", sample %d: %g %g\n",
                   row, pix, sample, w1, w2);
            if (--stopondiff == 0)
                exit(1);
            break;
        default:
            break;
    }
}

static int SeparateCompare(int reversed, int sample, uint32_t row,
                           unsigned char *cp1, unsigned char *p2)
{
    uint32_t npixels = imagewidth;
    int pixel;

    cp1 += sample;
    for (pixel = 0; npixels-- > 0; pixel++, cp1 += samplesperpixel, p2++)
    {
        if (*cp1 != *p2)
        {
            printf("Scanline %" PRIu32 ", pixel %" PRIu32 ", sample %d: ", row,
                   pixel, sample);
            if (reversed)
                printf("%02x %02x\n", *p2, *cp1);
            else
                printf("%02x %02x\n", *cp1, *p2);
            if (--stopondiff == 0)
                exit(1);
        }
    }

    return 0;
}

static int checkTag(TIFF *tif1, TIFF *tif2, int tag, char *name, void *p1,
                    void *p2)
{

    if (TIFFGetField(tif1, tag, p1))
    {
        if (!TIFFGetField(tif2, tag, p2))
        {
            printf("%s tag appears only in %s\n", name, TIFFFileName(tif1));
            return (0);
        }
        return (1);
    }
    else if (TIFFGetField(tif2, tag, p2))
    {
        printf("%s tag appears only in %s\n", name, TIFFFileName(tif2));
        return (0);
    }
    return (-1);
}

#define CHECK(cmp, fmt)                                                        \
    {                                                                          \
        switch (checkTag(tif1, tif2, tag, name, &v1, &v2))                     \
        {                                                                      \
            case 1:                                                            \
                if (cmp)                                                       \
                case -1:                                                       \
                    return (1);                                                \
                printf(fmt, name, v1, v2);                                     \
        }                                                                      \
        return (0);                                                            \
    }

static int CheckShortTag(TIFF *tif1, TIFF *tif2, int tag, char *name)
{
    uint16_t v1, v2;
    CHECK(v1 == v2, "%s: %" PRIu16 " %" PRIu16 "\n");
}

static int CheckShort2Tag(TIFF *tif1, TIFF *tif2, int tag, char *name)
{
    uint16_t v11, v12, v21, v22;

    if (TIFFGetField(tif1, tag, &v11, &v12))
    {
        if (!TIFFGetField(tif2, tag, &v21, &v22))
        {
            printf("%s tag appears only in %s\n", name, TIFFFileName(tif1));
            return (0);
        }
        if (v11 == v21 && v12 == v22)
            return (1);
        printf("%s: <%" PRIu16 ",%" PRIu16 "> <%" PRIu16 ",%" PRIu16 ">\n",
               name, v11, v12, v21, v22);
    }
    else if (TIFFGetField(tif2, tag, &v21, &v22))
        printf("%s tag appears only in %s\n", name, TIFFFileName(tif2));
    else
        return (1);
    return (0);
}

static int CheckShortArrayTag(TIFF *tif1, TIFF *tif2, int tag, char *name)
{
    uint16_t n1, *a1;
    uint16_t n2, *a2;

    if (TIFFGetField(tif1, tag, &n1, &a1))
    {
        if (!TIFFGetField(tif2, tag, &n2, &a2))
        {
            printf("%s tag appears only in %s\n", name, TIFFFileName(tif1));
            return (0);
        }
        if (n1 == n2)
        {
            char *sep;
            uint16_t i;

            if (memcmp(a1, a2, n1 * sizeof(uint16_t)) == 0)
                return (1);
            printf("%s: value mismatch, <%" PRIu16 ":", name, n1);
            sep = "";
            for (i = 0; i < n1; i++)
                printf("%s%" PRIu16, sep, a1[i]), sep = ",";
            printf("> and <%" PRIu16 ": ", n2);
            sep = "";
            for (i = 0; i < n2; i++)
                printf("%s%" PRIu16, sep, a2[i]), sep = ",";
            printf(">\n");
        }
        else
            printf("%s: %" PRIu16 " items in %s, %" PRIu16 " items in %s", name,
                   n1, TIFFFileName(tif1), n2, TIFFFileName(tif2));
    }
    else if (TIFFGetField(tif2, tag, &n2, &a2))
        printf("%s tag appears only in %s\n", name, TIFFFileName(tif2));
    else
        return (1);
    return (0);
}

static int CheckLongTag(TIFF *tif1, TIFF *tif2, int tag, char *name)
{
    uint32_t v1, v2;
    CHECK(v1 == v2, "%s: %" PRIu32 " %" PRIu32 "\n");
}

static int CheckFloatTag(TIFF *tif1, TIFF *tif2, int tag, char *name)
{
    float v1, v2;
    CHECK(v1 == v2, "%s: %g %g\n");
}

static int CheckStringTag(TIFF *tif1, TIFF *tif2, int tag, char *name)
{
    char *v1, *v2;
    CHECK(strcmp(v1, v2) == 0, "%s: \"%s\" \"%s\"\n");
}

static void leof(const char *name, uint32_t row, int s)
{

    printf("%s: EOF at scanline %" PRIu32, name, row);
    if (s >= 0)
        printf(", sample %d", s);
    printf("\n");
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

