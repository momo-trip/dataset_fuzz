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

#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

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
#include "tiffiop.h"

#ifndef EXIT_SUCCESS
#define EXIT_SUCCESS 0
#endif
#ifndef EXIT_FAILURE
#define EXIT_FAILURE 1
#endif

float defxres = 204.; /* default x resolution (pixels/inch) */
float defyres = 98.;  /* default y resolution (lines/inch) */
const float half = 0.5;
const float points = 72.0;
float pageWidth = 0;  /* image page width (inches) */
float pageHeight = 0; /* image page length (inches) */
int scaleToPage = 0;  /* if true, scale raster to page dimensions */
int totalPages = 0;   /* total # pages printed */
int row;              /* current output row */
int maxline = 512;    /* max output line of PostScript */

/*
 * Turn a bit-mapped scanline into the appropriate sequence
 * of PostScript characters to be rendered.
 *
 * Original version written by Bret D. Whissel,
 * Florida State University Meteorology Department
 * March 13-15, 1995.
 */
static void printruns(unsigned char *buf, uint32_t *runs, uint32_t *erun,
                      uint32_t lastx)
{
    static struct
    {
        char white, black;
        unsigned short width;
    } WBarr[] = {{'d', 'n', 512}, {'e', 'o', 256}, {'f', 'p', 128},
                 {'g', 'q', 64},  {'h', 'r', 32},  {'i', 's', 16},
                 {'j', 't', 8},   {'k', 'u', 4},   {'l', 'v', 2},
                 {'m', 'w', 1}};
    static char *svalue =
        " !\"#$&'*+,-./0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[]^_`abc";
    int colormode = 1; /* 0 for white, 1 for black */
    uint32_t runlength = 0;
    int n = maxline;
    uint32_t x = 0;
    int l;

    (void)buf;
    printf("%d m(", row++);
    while (runs < erun)
    {
        if (runlength <= 0)
        {
            colormode ^= 1;
            runlength = *runs++;
            if (x + runlength > lastx)
                runlength = runs[-1] = lastx - x;
            x += runlength;
            if (!colormode && runs == erun)
                break; /* don't bother printing the final white run */
        }
        /*
         * If a runlength is greater than 6 pixels, then spit out
         * black or white characters until the runlength drops to
         * 6 or less.  Once a runlength is <= 6, then combine black
         * and white runlengths until a 6-pixel pattern is obtained.
         * Then write out the special character.  Six-pixel patterns
         * were selected since 64 patterns is the largest power of
         * two less than the 92 "easily printable" PostScript
         * characters (i.e., no escape codes or octal chars).
         */
        l = 0;
        while (runlength > 6)
        { /* Run is greater than six... */
            if (runlength >= WBarr[l].width)
            {
                if (n == 0)
                {
                    putchar('\n');
                    n = maxline;
                }
                putchar(colormode ? WBarr[l].black : WBarr[l].white), n--;
                runlength -= WBarr[l].width;
            }
            else
                l++;
        }
        while (runlength > 0 && runlength <= 6)
        {
            uint32_t bitsleft = 6;
            int t = 0;
            while (bitsleft)
            {
                if (runlength <= bitsleft)
                {
                    if (colormode)
                        t |= ((1 << runlength) - 1) << (bitsleft - runlength);
                    bitsleft -= runlength;
                    runlength = 0;
                    if (bitsleft)
                    {
                        if (runs >= erun)
                            break;
                        colormode ^= 1;
                        runlength = *runs++;
                        if (x + runlength > lastx)
                            runlength = runs[-1] = lastx - x;
                        x += runlength;
                    }
                }
                else
                { /* runlength exceeds bits left */
                    if (colormode)
                        t |= ((1 << bitsleft) - 1);
                    runlength -= bitsleft;
                    bitsleft = 0;
                }
            }
            if (n == 0)
            {
                putchar('\n');
                n = maxline;
            }
            putchar(svalue[t]), n--;
        }
    }
    printf(")s\n");
}

/*
 * Create a special PostScript font for printing FAX documents.  By taking
 * advantage of the font-cacheing mechanism, a substantial speed-up in
 * rendering time is realized.
 */
static void emitFont(FILE *fd)
{
    static const char *fontPrologue[] = {
        "/newfont 10 dict def newfont begin /FontType 3 def /FontMatrix [1",
        "0 0 1 0 0] def /FontBBox [0 0 512 1] def /Encoding 256 array def",
        "0 1 31{Encoding exch /255 put}for 120 1 255{Encoding exch /255",
        "put}for Encoding 37 /255 put Encoding 40 /255 put Encoding 41 /255",
        "put Encoding 92 /255 put /count 0 def /ls{Encoding exch count 3",
        "string cvs cvn put /count count 1 add def}def 32 1 36{ls}for",
        "38 1 39{ls}for 42 1 91{ls}for 93 1 99{ls}for /count 100",
        "def 100 1 119{ls}for /CharDict 5 dict def CharDict begin /white",
        "{dup 255 eq{pop}{1 dict begin 100 sub neg 512 exch bitshift",
        "/cw exch def cw 0 0 0 cw 1 setcachedevice end}ifelse}def /black",
        "{dup 255 eq{pop}{1 dict begin 110 sub neg 512 exch bitshift",
        "/cw exch def cw 0 0 0 cw 1 setcachedevice 0 0 moveto cw 0 rlineto",
        "0 1 rlineto cw neg 0 rlineto closepath fill end}ifelse}def /numbuild",
        "{dup 255 eq{pop}{6 0 0 0 6 1 setcachedevice 0 1 5{0 moveto",
        "dup 32 and 32 eq{1 0 rlineto 0 1 rlineto -1 0 rlineto closepath",
        "fill newpath}if 1 bitshift}for pop}ifelse}def /.notdef {}",
        "def /255 {}def end /BuildChar{exch begin dup 110 ge{Encoding",
        "exch get 3 string cvs cvi CharDict /black get}{dup 100 ge {Encoding",
        "exch get 3 string cvs cvi CharDict /white get}{Encoding exch get",
        "3 string cvs cvi CharDict /numbuild get}ifelse}ifelse exec end",
        "}def end /Bitfont newfont definefont 1 scalefont setfont",
        NULL};
    int i;
    for (i = 0; fontPrologue[i] != NULL; i++)
        fprintf(fd, "%s\n", fontPrologue[i]);
}

void printTIF(TIFF *tif, uint16_t pageNumber)
{
    uint32_t w, h;
    uint16_t unit, compression;
    float xres, yres, scale = 1.0;
    tstrip_t s, ns;
    time_t creation_time;

    TIFFGetField(tif, TIFFTAG_IMAGELENGTH, &h);
    TIFFGetField(tif, TIFFTAG_IMAGEWIDTH, &w);
    if (!TIFFGetField(tif, TIFFTAG_COMPRESSION, &compression) ||
        compression < COMPRESSION_CCITTRLE ||
        compression > COMPRESSION_CCITT_T6)
        return;
    if (!TIFFGetField(tif, TIFFTAG_XRESOLUTION, &xres) || !xres)
    {
        TIFFWarning(TIFFFileName(tif), "No x-resolution, assuming %g dpi",
                    defxres);
        xres = defxres;
    }
    if (!TIFFGetField(tif, TIFFTAG_YRESOLUTION, &yres) || !yres)
    {
        TIFFWarning(TIFFFileName(tif), "No y-resolution, assuming %g lpi",
                    defyres);
        yres = defyres; /* XXX */
    }
    if (TIFFGetField(tif, TIFFTAG_RESOLUTIONUNIT, &unit) &&
        unit == RESUNIT_CENTIMETER)
    {
        xres *= 2.54F;
        yres *= 2.54F;
    }
    if (pageWidth == 0)
        pageWidth = w / xres;
    if (pageHeight == 0)
        pageHeight = h / yres;

    printf("%%!PS-Adobe-3.0\n");
    printf("%%%%Creator: fax2ps\n");
#ifdef notdef
    printf("%%%%Title: %s\n", file);
#endif
    creation_time = time(0);
    printf("%%%%CreationDate: %s", ctime(&creation_time));
    printf("%%%%Origin: 0 0\n");
    printf("%%%%BoundingBox: 0 0 %u %u\n", (int)(pageWidth * points),
           (int)(pageHeight * points)); /* XXX */
    printf("%%%%Pages: (atend)\n");
    printf("%%%%EndComments\n");
    printf("%%%%BeginProlog\n");
    emitFont(stdout);
    printf("/d{bind def}def\n"); /* bind and def proc */
    printf("/m{0 exch moveto}d\n");
    printf("/s{show}d\n");
    printf("/p{showpage}d \n"); /* end page */
    printf("%%%%EndProlog\n");
    printf("%%%%Page: \"%u\" %u\n", pageNumber, pageNumber);
    printf("/$pageTop save def gsave\n");
    if (scaleToPage)
        scale = pageHeight / (h / yres) < pageWidth / (w / xres)
                    ? pageHeight / (h / yres)
                    : pageWidth / (w / xres);
    printf("%g %g translate\n", points * (pageWidth - scale * w / xres) * half,
           points *
               (scale * h / yres + (pageHeight - scale * h / yres) * half));
    printf("%g %g scale\n", points / xres * scale, -points / yres * scale);
    printf("0 setgray\n");
    TIFFSetField(tif, TIFFTAG_FAXFILLFUNC, printruns);
    ns = TIFFNumberOfStrips(tif);
    row = 0;
    for (s = 0; s < ns; s++)
        (void)TIFFReadEncodedStrip(tif, s, (tdata_t)NULL, (tsize_t)-1);
    printf("p\n");
    printf("grestore $pageTop restore\n");
    totalPages++;
}

#define GetPageNumber(tif) TIFFGetField(tif, TIFFTAG_PAGENUMBER, &pn, &ptotal)

int findPage(TIFF *tif, uint16_t pageNumber)
{
    uint16_t pn = (uint16_t)-1;
    uint16_t ptotal = (uint16_t)-1;
    if (GetPageNumber(tif))
    {
        while (pn != (pageNumber - 1) && TIFFReadDirectory(tif) &&
               GetPageNumber(tif))
            ;
        return (pn == (pageNumber - 1));
    }
    else
        return (TIFFSetDirectory(tif, (tdir_t)(pageNumber - 1)));
}

void fax2ps(TIFF *tif, uint16_t npages, uint16_t *pages, char *filename)
{
    if (npages > 0)
    {
        uint16_t pn, ptotal;
        int i;

        if (!GetPageNumber(tif))
            fprintf(stderr, "%s: No page numbers, counting directories.\n",
                    filename);
        for (i = 0; i < npages; i++)
        {
            if (findPage(tif, pages[i]))
                printTIF(tif, pages[i]);
            else
                fprintf(stderr, "%s: No page number %d\n", filename, pages[i]);
        }
    }
    else
    {
        uint16_t pageNumber = 0;
        do
            printTIF(tif, pageNumber++);
        while (TIFFReadDirectory(tif));
    }
}

#undef GetPageNumber

static int pcompar(const void *va, const void *vb)
{
    const uint16_t *pa = (const uint16_t *)va;
    const uint16_t *pb = (const uint16_t *)vb;
    return ((int32_t)*pa - (int32_t)*pb);
}

static void usage(int code);

int original_main(int argc, char **argv)
{
#if !HAVE_DECL_OPTARG
    extern int optind;
    extern char *optarg;
#endif
    uint16_t *pages = NULL, npages = 0, pageNumber;
    int c, dowarnings = 0; /* if 1, enable library warnings */
    TIFF *tif;

    while ((c = getopt(argc, argv, "l:p:x:y:W:H:wSh")) != -1)
        switch (c)
        {
            case 'H': /* page height */
                pageHeight = (float)atof(optarg);
                break;
            case 'S': /* scale to page */
                scaleToPage = 1;
                break;
            case 'W': /* page width */
                pageWidth = (float)atof(optarg);
                break;
            case 'p': /* print specific page */
                pageNumber = (uint16_t)atoi(optarg);
                if (pages)
                    pages = (uint16_t *)realloc(pages, (npages + 1) *
                                                           sizeof(uint16_t));
                else
                    pages = (uint16_t *)malloc(sizeof(uint16_t));
                if (pages == NULL)
                {
                    fprintf(stderr, "Out of memory\n");
                    exit(EXIT_FAILURE);
                }
                pages[npages++] = pageNumber;
                break;
            case 'w':
                dowarnings = 1;
                break;
            case 'x':
                defxres = (float)atof(optarg);
                break;
            case 'y':
                defyres = (float)atof(optarg);
                break;
            case 'l':
                maxline = atoi(optarg);
                break;
            case 'h':
                free(pages);
                usage(EXIT_SUCCESS);
                break;
            case '?':
                free(pages);
                usage(EXIT_FAILURE);
        }
    if (npages > 0)
        qsort(pages, npages, sizeof(uint16_t), pcompar);
    if (!dowarnings)
        TIFFSetWarningHandler(0);
    if (optind < argc)
    {
        do
        {
            tif = TIFFOpen(argv[optind], "r");
            if (tif)
            {
                fax2ps(tif, npages, pages, argv[optind]);
                TIFFClose(tif);
            }
            else
                fprintf(stderr, "%s: Can not open, or not a TIFF file.\n",
                        argv[optind]);
        } while (++optind < argc);
    }
    else
    {
        int n;
        FILE *fd;
        char buf[16 * 1024];

        /* Silence Coverity Scan warning about insecure temporary file name. */
        /* coverity[secure_temp:SUPPRESS] */
        fd = tmpfile();
        if (fd == NULL)
        {
            fprintf(stderr, "Could not obtain temporary file.\n");
            free(pages);
            exit(EXIT_FAILURE);
        }
#if defined(HAVE_SETMODE) && defined(O_BINARY)
        setmode(fileno(stdin), O_BINARY);
#endif
        while ((n = read(fileno(stdin), buf, sizeof(buf))) > 0)
        {
            if (write(fileno(fd), buf, n) != n)
            {
                fclose(fd);
                fprintf(stderr, "Could not copy stdin to temporary file.\n");
                free(pages);
                exit(EXIT_FAILURE);
            }
        }
        _TIFF_lseek_f(fileno(fd), 0, SEEK_SET);
#if defined(_WIN32) && defined(USE_WIN32_FILEIO)
        tif = TIFFFdOpen(_get_osfhandle(fileno(fd)), "temp", "r");
#else
        tif = TIFFFdOpen(fileno(fd), "temp", "r");
#endif
        if (tif)
        {
            fax2ps(tif, npages, pages, "<stdin>");
            TIFFClose(tif);
        }
        else
            fprintf(stderr, "Can not open, or not a TIFF file.\n");
        fclose(fd);
    }
    printf("%%%%Trailer\n");
    printf("%%%%Pages: %u\n", totalPages);
    printf("%%%%EOF\n");

    free(pages);
    return (EXIT_SUCCESS);
}

static const char usage_info[] =
    "Convert a TIFF facsimile to compressed PostScript\n\n"
    "usage: fax2ps [options] [input.tif ...]\n"
    "where options are:\n"
    " -w            suppress warning messages\n"
    " -l chars      set maximum output line length for generated PostScript\n"
    " -p page#      select page to print (can use multiple times)\n"
    " -x xres       set default horizontal resolution of input data (dpi)\n"
    " -y yres       set default vertical resolution of input data (lpi)\n"
    " -S            scale output to page size\n"
    " -W width      set output page width (inches), default is 8.5\n"
    " -H height     set output page height (inches), default is 11\n";

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

