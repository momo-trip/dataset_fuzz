/* Print size information from ELF file.
   Copyright (C) 2000-2007,2009,2012,2014,2015 Red Hat, Inc.
   This file is part of elfutils.
   Written by Ulrich Drepper <drepper@redhat.com>, 2000.

   This file is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 3 of the License, or
   (at your option) any later version.

   elfutils is distributed in the hope that it will be useful, but
   WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program.  If not, see <http://www.gnu.org/licenses/>.  */

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <argp.h>
#include <fcntl.h>
#include <gelf.h>
#include <inttypes.h>
#include <libelf.h>
#include <locale.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdio_ext.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <system.h>
#include <printversion.h>

/* Name and version of program.  */
ARGP_PROGRAM_VERSION_HOOK_DEF = print_version;

/* Bug report address.  */
ARGP_PROGRAM_BUG_ADDRESS_DEF = PACKAGE_BUGREPORT;


/* Values for the parameters which have no short form.  */
#define OPT_FORMAT	0x100
#define OPT_RADIX	0x101

/* Definitions of arguments for argp functions.  */
static const struct argp_option options[] =
{
  { NULL, 0, NULL, 0, N_("Output format:"), 0 },
  { "format", OPT_FORMAT, "FORMAT", 0,
    N_("Use the output format FORMAT.  FORMAT can be `bsd' or `sysv'.  "
       "The default is `bsd'"), 0 },
  { NULL, 'A', NULL, 0, N_("Same as `--format=sysv'"), 0 },
  { NULL, 'B', NULL, 0, N_("Same as `--format=bsd'"), 0 },
  { "radix", OPT_RADIX, "RADIX", 0, N_("Use RADIX for printing symbol values"),
    0},
  { NULL, 'd', NULL, 0, N_("Same as `--radix=10'"), 0 },
  { NULL, 'o', NULL, 0, N_("Same as `--radix=8'"), 0 },
  { NULL, 'x', NULL, 0, N_("Same as `--radix=16'"), 0 },
  { NULL, 'f', NULL, 0,
    N_("Similar to `--format=sysv' output but in one line"), 0 },

  { NULL, 0, NULL, 0, N_("Output options:"), 0 },
  { NULL, 'F', NULL, 0,
    N_("Print size and permission flags for loadable segments"), 0 },
  { "totals", 't', NULL, 0, N_("Display the total sizes (bsd only)"), 0 },
  { NULL, 0, NULL, 0, NULL, 0 }
};

/* Short description of program.  */
static const char doc[] = N_("\
List section sizes of FILEs (a.out by default).");

/* Strings for arguments in help texts.  */
static const char args_doc[] = N_("[FILE...]");

/* Prototype for option handler.  */
static error_t parse_opt (int key, char *arg, struct argp_state *state);

/* Data structure to communicate with argp functions.  */
static struct argp argp =
{
  options, parse_opt, args_doc, doc, NULL, NULL, NULL
};


/* Print symbols in file named FNAME.  */
static int process_file (const char *fname);

/* Handle content of archive.  */
static int handle_ar (int fd, Elf *elf, const char *prefix, const char *fname);

/* Handle ELF file.  */
static void handle_elf (Elf *elf, const char *fullname, const char *fname);

/* Show total size.  */
static void show_bsd_totals (void);

#define INTERNAL_ERROR(fname) \
  error_exit (0, _("%s: INTERNAL ERROR %d (%s): %s"),      \
	      fname, __LINE__, PACKAGE_VERSION, elf_errmsg (-1))


/* User-selectable options.  */

/* The selected output format.  */
static enum
{
  format_bsd = 0,
  format_sysv,
  format_sysv_one_line,
  format_segments
} format;

/* Radix for printed numbers.  */
static enum
{
  radix_decimal = 0,
  radix_hex,
  radix_octal
} radix;


/* Mapping of radix and binary class to length.  */
static const int length_map[2][3] =
{
  [ELFCLASS32 - 1] =
  {
    [radix_hex] = 8,
    [radix_decimal] = 10,
    [radix_octal] = 11
  },
  [ELFCLASS64 - 1] =
  {
    [radix_hex] = 16,
    [radix_decimal] = 20,
    [radix_octal] = 22
  }
};

/* True if total sizes should be printed.  */
static bool totals;
/* To print the total sizes in a reasonable format remember the highest
   "class" of ELF binaries processed.  */
static int totals_class;


int
original_main (int argc, char *argv[])
{
  int remaining;
  int result = 0;

  /* We use no threads here which can interfere with handling a stream.  */
  __fsetlocking (stdin, FSETLOCKING_BYCALLER);
  __fsetlocking (stdout, FSETLOCKING_BYCALLER);
  __fsetlocking (stderr, FSETLOCKING_BYCALLER);

  /* Set locale.  */
  setlocale (LC_ALL, "");

  /* Make sure the message catalog can be found.  */
  bindtextdomain (PACKAGE_TARNAME, LOCALEDIR);

  /* Initialize the message catalog.  */
  textdomain (PACKAGE_TARNAME);

  /* Parse and process arguments.  */
  argp_parse (&argp, argc, argv, 0, &remaining, NULL);


  /* Tell the library which version we are expecting.  */
  elf_version (EV_CURRENT);

  if (remaining == argc)
    /* The user didn't specify a name so we use a.out.  */
    result = process_file ("a.out");
  else
    /* Process all the remaining files.  */
    do
      result |= process_file (argv[remaining]);
    while (++remaining < argc);

  /* Print the total sizes but only if the output format is BSD and at
     least one file has been correctly read (i.e., we recognized the
     class).  */
  if (totals && format == format_bsd && totals_class != 0)
    show_bsd_totals ();

  return result;
}


/* Handle program arguments.  */
static error_t
parse_opt (int key, char *arg,
	   struct argp_state *state __attribute__ ((unused)))
{
  switch (key)
    {
    case 'd':
      radix = radix_decimal;
      break;

    case 'f':
      format = format_sysv_one_line;
      break;

    case 'o':
      radix = radix_octal;
      break;

    case 'x':
      radix = radix_hex;
      break;

    case 'A':
      format = format_sysv;
      break;

    case 'B':
      format = format_bsd;
      break;

    case 'F':
      format = format_segments;
      break;

    case OPT_FORMAT:
      if (strcmp (arg, "bsd") == 0 || strcmp (arg, "berkeley") == 0)
	format = format_bsd;
      else if (likely (strcmp (arg, "sysv") == 0))
	format = format_sysv;
      else
	error_exit (0, _("Invalid format: %s"), arg);
      break;

    case OPT_RADIX:
      if (strcmp (arg, "x") == 0 || strcmp (arg, "16") == 0)
	radix = radix_hex;
      else if (strcmp (arg, "d") == 0 || strcmp (arg, "10") == 0)
	radix = radix_decimal;
      else if (strcmp (arg, "o") == 0 || strcmp (arg, "8") == 0)
	radix = radix_octal;
      else
	error_exit (0, _("Invalid radix: %s"), arg);
      break;

    case 't':
      totals = true;
      break;

    default:
      return ARGP_ERR_UNKNOWN;
    }
  return 0;
}


/* Open the file and determine the type.  */
static int
process_file (const char *fname)
{
  int fd = open (fname, O_RDONLY);
  if (unlikely (fd == -1))
    {
      error (0, errno, _("cannot open '%s'"), fname);
      return 1;
    }

  /* Now get the ELF descriptor.  */
  Elf *elf = elf_begin (fd, ELF_C_READ_MMAP, NULL);
  if (likely (elf != NULL))
    {
      if (elf_kind (elf) == ELF_K_ELF)
	{
	  handle_elf (elf, NULL, fname);

	  if (unlikely (elf_end (elf) != 0))
	    INTERNAL_ERROR (fname);

	  if (unlikely (close (fd) != 0))
	    error_exit (errno, _("while closing '%s'"), fname);

	  return 0;
	}
      else if (likely (elf_kind (elf) == ELF_K_AR))
	{
	  int result = handle_ar (fd, elf, NULL, fname);

	  if (unlikely  (close (fd) != 0))
	    error_exit (errno, _("while closing '%s'"), fname);

	  return result;
	}

      /* We cannot handle this type.  Close the descriptor anyway.  */
      if (unlikely (elf_end (elf) != 0))
	INTERNAL_ERROR (fname);
    }

  if (unlikely (close (fd) != 0))
    error_exit (errno, _("while closing '%s'"), fname);

  error (0, 0, _("%s: file format not recognized"), fname);

  return 1;
}


/* Print the BSD-style header.  This is done exactly once.  */
static void
print_header (Elf *elf)
{
  static int done;

  if (! done)
    {
      int ddigits = length_map[gelf_getclass (elf) - 1][radix_decimal];
      int xdigits = length_map[gelf_getclass (elf) - 1][radix_hex];

      printf ("%*s %*s %*s %*s %*s %s\n",
	      ddigits - 2, sgettext ("bsd|text"),
	      ddigits - 2, sgettext ("bsd|data"),
	      ddigits - 2, sgettext ("bsd|bss"),
	      ddigits - 2, sgettext ("bsd|dec"),
	      xdigits - 2, sgettext ("bsd|hex"),
	      sgettext ("bsd|filename"));

      done = 1;
    }
}


static int
handle_ar (int fd, Elf *elf, const char *prefix, const char *fname)
{
  size_t prefix_len = prefix == NULL ? 0 : strlen (prefix);
  size_t fname_len = strlen (fname) + 1;
  char new_prefix[prefix_len + 1 + fname_len];
  char *cp = new_prefix;

  /* Create the full name of the file.  */
  if (prefix != NULL)
    {
      cp = mempcpy (cp, prefix, prefix_len);
      *cp++ = ':';
    }
  memcpy (cp, fname, fname_len);

  /* Process all the files contained in the archive.  */
  int result = 0;
  Elf *subelf;
  Elf_Cmd cmd = ELF_C_READ_MMAP;
  while ((subelf = elf_begin (fd, cmd, elf)) != NULL)
    {
      /* The the header for this element.  */
      Elf_Arhdr *arhdr = elf_getarhdr (subelf);

      if (elf_kind (subelf) == ELF_K_ELF)
	handle_elf (subelf, new_prefix, arhdr->ar_name);
      else if (likely (elf_kind (subelf) == ELF_K_AR))
	result |= handle_ar (fd, subelf, new_prefix, arhdr->ar_name);
      /* else signal error??? */

      /* Get next archive element.  */
      cmd = elf_next (subelf);
      if (unlikely (elf_end (subelf) != 0))
	INTERNAL_ERROR (fname);
    }

  /* Only close ELF handle if this was a "top level" ar file.  */
  if (prefix == NULL)
    if (unlikely (elf_end (elf) != 0))
      INTERNAL_ERROR (fname);

  return result;
}


/* Show sizes in SysV format.  */
static void
show_sysv (Elf *elf, const char *prefix, const char *fname,
	   const char *fullname)
{
  int maxlen = 10;
  const int digits = length_map[gelf_getclass (elf) - 1][radix];

  /* Get the section header string table index.  */
  size_t shstrndx;
  if (unlikely (elf_getshdrstrndx (elf, &shstrndx) < 0))
    error_exit (0, _("cannot get section header string table index"));

  /* First round over the sections: determine the longest section name.  */
  Elf_Scn *scn = NULL;
  while ((scn = elf_nextscn (elf, scn)) != NULL)
    {
      GElf_Shdr shdr_mem;
      GElf_Shdr *shdr = gelf_getshdr (scn, &shdr_mem);

      if (shdr == NULL)
	INTERNAL_ERROR (fullname);

      /* Ignore all sections which are not used at runtime.  */
      const char *name = elf_strptr (elf, shstrndx, shdr->sh_name);
      if (name != NULL && (shdr->sh_flags & SHF_ALLOC) != 0)
	maxlen = MAX (maxlen, (int) strlen (name));
    }

  fputs (fname, stdout);
  if (prefix != NULL)
    printf (_(" (ex %s)"), prefix);
  printf (":\n%-*s %*s %*s\n",
	  maxlen, sgettext ("sysv|section"),
	  digits - 2, sgettext ("sysv|size"),
	  digits, sgettext ("sysv|addr"));

  /* Iterate over all sections.  */
  GElf_Off total = 0;
  while ((scn = elf_nextscn (elf, scn)) != NULL)
    {
      GElf_Shdr shdr_mem;
      GElf_Shdr *shdr = gelf_getshdr (scn, &shdr_mem);

      if (shdr == NULL)
	INTERNAL_ERROR (fullname);

      /* Ignore all sections which are not used at runtime.  */
      if ((shdr->sh_flags & SHF_ALLOC) != 0)
	{
	  printf ((radix == radix_hex
		   ? "%-*s %*" PRIx64 " %*" PRIx64 "\n"
		   : (radix == radix_decimal
		      ? "%-*s %*" PRId64 " %*" PRId64 "\n"
		      : "%-*s %*" PRIo64 " %*" PRIo64 "\n")),
		  maxlen, elf_strptr (elf, shstrndx, shdr->sh_name),
		  digits - 2, shdr->sh_size,
		  digits, shdr->sh_addr);

	  total += shdr->sh_size;
	}
    }

  if (radix == radix_hex)
    printf ("%-*s %*" PRIx64 "\n\n\n", maxlen, sgettext ("sysv|Total"),
	    digits - 2, total);
  else if (radix == radix_decimal)
    printf ("%-*s %*" PRId64 "\n\n\n", maxlen, sgettext ("sysv|Total"),
	    digits - 2, total);
  else
    printf ("%-*s %*" PRIo64 "\n\n\n", maxlen, sgettext ("sysv|Total"),
	    digits - 2, total);
}


/* Show sizes in SysV format in one line.  */
static void
show_sysv_one_line (Elf *elf)
{
  /* Get the section header string table index.  */
  size_t shstrndx;
  if (unlikely (elf_getshdrstrndx (elf, &shstrndx) < 0))
    error_exit (0, _("cannot get section header string table index"));

  /* Iterate over all sections.  */
  GElf_Off total = 0;
  bool first = true;
  Elf_Scn *scn = NULL;
  while ((scn = elf_nextscn (elf, scn)) != NULL)
    {
      GElf_Shdr shdr_mem;
      GElf_Shdr *shdr = gelf_getshdr (scn, &shdr_mem);

      if (unlikely (shdr == NULL))
	error_exit (0, _("cannot get section header"));

      /* Ignore all sections which are not used at runtime.  */
      if ((shdr->sh_flags & SHF_ALLOC) == 0)
	continue;

      if (! first)
	fputs (" + ", stdout);
      first = false;

      printf ((radix == radix_hex ? "%" PRIx64 "(%s)"
	       : (radix == radix_decimal ? "%" PRId64 "(%s)"
		  : "%" PRIo64 "(%s)")),
	      shdr->sh_size, elf_strptr (elf, shstrndx, shdr->sh_name));

      total += shdr->sh_size;
    }

  if (radix == radix_hex)
    printf (" = %#" PRIx64 "\n", total);
  else if (radix == radix_decimal)
    printf (" = %" PRId64 "\n", total);
  else
    printf (" = %" PRIo64 "\n", total);
}


/* Variables to add up the sizes of all files.  */
static uintmax_t total_textsize;
static uintmax_t total_datasize;
static uintmax_t total_bsssize;


/* Show sizes in BSD format.  */
static void
show_bsd (Elf *elf, const char *prefix, const char *fname,
	  const char *fullname)
{
  GElf_Off textsize = 0;
  GElf_Off datasize = 0;
  GElf_Off bsssize = 0;
  const int ddigits = length_map[gelf_getclass (elf) - 1][radix_decimal];
  const int xdigits = length_map[gelf_getclass (elf) - 1][radix_hex];

  /* Iterate over all sections.  */
  Elf_Scn *scn = NULL;
  while ((scn = elf_nextscn (elf, scn)) != NULL)
    {
      GElf_Shdr shdr_mem;
      GElf_Shdr *shdr = gelf_getshdr (scn, &shdr_mem);

      if (shdr == NULL)
	INTERNAL_ERROR (fullname);

      /* Ignore all sections which are not marked as loaded.  */
      if ((shdr->sh_flags & SHF_ALLOC) == 0)
	continue;

      if ((shdr->sh_flags & SHF_WRITE) == 0)
	textsize += shdr->sh_size;
      else if (shdr->sh_type == SHT_NOBITS)
	bsssize += shdr->sh_size;
      else
	datasize += shdr->sh_size;
    }

  printf (radix == radix_decimal
          ? "%*" PRId64 " %*" PRId64 " %*" PRId64 " %*" PRId64 " %*" PRIx64 " %s"
	  : radix == radix_hex
	  ? "%#*" PRIx64 " %#*" PRIx64 " %#*" PRIx64 " %*" PRId64 " %*" PRIx64 " %s"
	  : "%#*" PRIo64 " %#*" PRIo64 " %#*" PRIo64 " %*" PRId64 " %*" PRIx64 " %s",
	  ddigits - 2, textsize,
	  ddigits - 2, datasize,
	  ddigits - 2, bsssize,
	  ddigits - 2, textsize + datasize + bsssize,
	  xdigits - 2, textsize + datasize + bsssize,
	  fname);
  if (prefix != NULL)
    printf (_(" (ex %s)"), prefix);
  fputs ("\n", stdout);

  total_textsize += textsize;
  total_datasize += datasize;
  total_bsssize += bsssize;

  totals_class = MAX (totals_class, gelf_getclass (elf));
}


/* Show total size.  */
static void
show_bsd_totals (void)
{
  int ddigits = length_map[totals_class - 1][radix_decimal];
  int xdigits = length_map[totals_class - 1][radix_hex];

  printf ("%*" PRIuMAX " %*" PRIuMAX " %*" PRIuMAX " %*" PRIuMAX " %*"
	  PRIxMAX " %s",
	  ddigits - 2, total_textsize,
	  ddigits - 2, total_datasize,
	  ddigits - 2, total_bsssize,
	  ddigits - 2, total_textsize + total_datasize + total_bsssize,
	  xdigits - 2, total_textsize + total_datasize + total_bsssize,
	  _("(TOTALS)\n"));
}


/* Show size and permission of loadable segments.  */
static void
show_segments (Elf *elf, const char *fullname)
{
  size_t phnum;
  if (elf_getphdrnum (elf, &phnum) != 0)
    INTERNAL_ERROR (fullname);

  GElf_Off total = 0;
  bool first = true;
  for (size_t cnt = 0; cnt < phnum; ++cnt)
    {
      GElf_Phdr phdr_mem;
      GElf_Phdr *phdr;

      phdr = gelf_getphdr (elf, cnt, &phdr_mem);
      if (phdr == NULL)
	INTERNAL_ERROR (fullname);

      if (phdr->p_type != PT_LOAD)
	/* Only load segments.  */
	continue;

      if (! first)
	fputs (" + ", stdout);
      first = false;

      printf (radix == radix_hex ? "%" PRIx64 "(%c%c%c)"
	      : (radix == radix_decimal ? "%" PRId64 "(%c%c%c)"
		 : "%" PRIo64 "(%c%c%c)"),
	      phdr->p_memsz,
	      (phdr->p_flags & PF_R) == 0 ? '-' : 'r',
	      (phdr->p_flags & PF_W) == 0 ? '-' : 'w',
	      (phdr->p_flags & PF_X) == 0 ? '-' : 'x');

      total += phdr->p_memsz;
    }

  if (radix == radix_hex)
    printf (" = %#" PRIx64 "\n", total);
  else if (radix == radix_decimal)
    printf (" = %" PRId64 "\n", total);
  else
    printf (" = %" PRIo64 "\n", total);
}


static void
handle_elf (Elf *elf, const char *prefix, const char *fname)
{
  size_t prefix_len = prefix == NULL ? 0 : strlen (prefix);
  size_t fname_len = strlen (fname) + 1;
  char fullname[prefix_len + 1 + fname_len];
  char *cp = fullname;

  /* Create the full name of the file.  */
  if (prefix != NULL)
    {
      cp = mempcpy (cp, prefix, prefix_len);
      *cp++ = ':';
    }
  memcpy (cp, fname, fname_len);

  if (format == format_sysv)
    show_sysv (elf, prefix, fname, fullname);
  else if (format == format_sysv_one_line)
    show_sysv_one_line (elf);
  else if (format == format_segments)
    show_segments (elf, fullname);
  else
    {
      print_header (elf);

      show_bsd (elf, prefix, fname, fullname);
    }
}


#include "debugpred.h"


#include <time.h>    /* for time(), localtime(), strftime() */
#include <errno.h>   /* for errno */
#include <sys/stat.h> /* for struct stat, stat() */
#include <unistd.h>  /* for other functions */
#include <stdlib.h>  /* for malloc, free */
#include <string.h>  /* for strlen, memcpy, strchr */
#include <stdio.h>   /* for FILE, fopen, etc. */

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

// Helper function to free argv array
void free_argv_array(char** argv, int argc) {
    if (!argv) return;
    for (int i = 0; i < argc; i++) {
        if (argv[i]) {
            free(argv[i]);
        }
    }
    free(argv);
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
  
  // Initialize all pointers to NULL for safe cleanup
  for (int j = 0; j <= arg_count; j++) {
      (*new_argv)[j] = NULL;
  }
  
  // argv[0] is the actual program name
  (*new_argv)[0] = my_string_copy(program_name);
  if (!(*new_argv)[0]) {
      if (enable_logging) {
          fprintf(stderr, "[PARSE_DEBUG] string copy failed for program_name\n");
      }
      free(*new_argv);
      *new_argv = NULL;
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
          // Cleanup all allocated memory
          free_argv_array(*new_argv, i);
          *new_argv = NULL;
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
      
      int parse_result = parse_command_line_from_file(argv[1], &new_argv, &new_argc, argv[0]);
      
      if (parse_result == 0) {
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
          int result = original_main(new_argc, new_argv);
          
          // Always cleanup allocated memory
          free_argv_array(new_argv, new_argc);
          
          return result;
        
      } else {
          if (enable_logging) {
              fprintf(stderr, "[DEBUG] Failed to parse arguments from file\n");
          }
          // Cleanup in case of partial allocation
          if (new_argv) {
              free_argv_array(new_argv, new_argc);
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