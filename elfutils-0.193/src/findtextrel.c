/* Locate source files or functions which caused text relocations.
   Copyright (C) 2005-2010, 2012, 2014, 2018 Red Hat, Inc.
   This file is part of elfutils.
   Written by Ulrich Drepper <drepper@redhat.com>, 2005.

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
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <gelf.h>
#include <libdw.h>
#include <locale.h>
#include <search.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <printversion.h>
#include "libeu.h"
#include "system.h"

struct segments
{
  GElf_Addr from;
  GElf_Addr to;
};


/* Name and version of program.  */
ARGP_PROGRAM_VERSION_HOOK_DEF = print_version;

/* Bug report address.  */
ARGP_PROGRAM_BUG_ADDRESS_DEF = PACKAGE_BUGREPORT;

/* Values for the parameters which have no short form.  */
#define OPT_DEBUGINFO 0x100

/* Definitions of arguments for argp functions.  */
static const struct argp_option options[] =
{
  { NULL, 0, NULL, 0, N_("Input Selection:"), 0 },
  { "root", 'r', "PATH", 0, N_("Prepend PATH to all file names"), 0 },
  { "debuginfo", OPT_DEBUGINFO, "PATH", 0,
    N_("Use PATH as root of debuginfo hierarchy"), 0 },

  { NULL, 0, NULL, 0, N_("Miscellaneous:"), 0 },
  { NULL, 0, NULL, 0, NULL, 0 }
};

/* Short description of program.  */
static const char doc[] = N_("\
Locate source of text relocations in FILEs (a.out by default).");

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
static int process_file (const char *fname, bool more_than_one);

/* Check for text relocations in the given file.  The segment
   information is known.  */
static void check_rel (size_t nsegments, struct segments segments[nsegments],
		       GElf_Addr addr, Elf *elf, Elf_Scn *symscn, Dwarf *dw,
		       const char *fname, bool more_than_one,
		       void **knownsrcs);



/* User-provided root directory.  */
static const char *rootdir = "/";

/* Root of debuginfo directory hierarchy.  */
static const char *debuginfo_root;


int
original_main (int argc, char *argv[])
{
  int remaining;
  int result = 0;

  /* Set locale.  */
  (void) setlocale (LC_ALL, "");

  /* Make sure the message catalog can be found.  */
  (void) bindtextdomain (PACKAGE_TARNAME, LOCALEDIR);

  /* Initialize the message catalog.  */
  (void) textdomain (PACKAGE_TARNAME);

  /* Parse and process arguments.  */
  (void) argp_parse (&argp, argc, argv, 0, &remaining, NULL);

  /* Tell the library which version we are expecting.  */
  elf_version (EV_CURRENT);

  /* If the user has not specified the root directory for the
     debuginfo hierarchy, we have to determine it ourselves.  */
  if (debuginfo_root == NULL)
    {
      // XXX The runtime should provide this information.
#if defined __ia64__ || defined __alpha__
      debuginfo_root = "/usr/lib/debug";
#else
      debuginfo_root = (sizeof (long int) == 4
			? "/usr/lib/debug" : "/usr/lib64/debug");
#endif
    }

  if (remaining == argc)
    result = process_file ("a.out", false);
  else
    {
      /* Process all the remaining files.  */
      const bool more_than_one = remaining + 1 < argc;

      do
	result |= process_file (argv[remaining], more_than_one);
      while (++remaining < argc);
    }

  return result;
}


/* Handle program arguments.  */
static error_t
parse_opt (int key, char *arg,
	   struct argp_state *state __attribute__ ((unused)))
{
  switch (key)
    {
    case 'r':
      rootdir = arg;
      break;

    case OPT_DEBUGINFO:
      debuginfo_root = arg;
      break;

    default:
      return ARGP_ERR_UNKNOWN;
    }
  return 0;
}


static void
noop (void *arg __attribute__ ((unused)))
{
}


static int
open_rootdir_file (const char *fname)
{
  char *new_fname = NULL;
  const char *real_fname = fname;

  if (fname[0] == '/' && (rootdir[0] != '/' || rootdir[1] != '\0'))
    real_fname = new_fname = xasprintf ("%s/%s", rootdir, fname);

  int fd = open (real_fname, O_RDONLY);
  if (fd == -1)
    error (0, errno, _("cannot open '%s'"), fname);

  free (new_fname);
  return fd;
}


static int
process_file (const char *fname, bool more_than_one)
{
  int result = 0;
  void *knownsrcs = NULL;
  int fd = open_rootdir_file (fname);
  if (fd == -1)
    return 1;

  Elf *elf = elf_begin (fd, ELF_C_READ_MMAP, NULL);
  if (elf == NULL)
    {
      error (0, 0, _("cannot create ELF descriptor for '%s': %s"),
	     fname, elf_errmsg (-1));
      goto err_close;
    }

  /* Make sure the file is a DSO.  */
  GElf_Ehdr ehdr_mem;
  GElf_Ehdr *ehdr = gelf_getehdr (elf, &ehdr_mem);
  if (ehdr == NULL)
    {
      error (0, 0, _("cannot get ELF header '%s': %s"),
	     fname, elf_errmsg (-1));
    err_elf_close:
      elf_end (elf);
    err_close:
      close (fd);
      return 1;
    }

  if (ehdr->e_type != ET_DYN)
    {
      error (0, 0, _("'%s' is not a DSO or PIE"), fname);
      goto err_elf_close;
    }

  /* Determine whether the DSO has text relocations at all and locate
     the symbol table.  */
  Elf_Scn *symscn = NULL;
  Elf_Scn *scn = NULL;
  bool seen_dynamic = false;
  bool have_textrel = false;
  while ((scn = elf_nextscn (elf, scn)) != NULL
	 && (!seen_dynamic || symscn == NULL))
    {
      /* Handle the section if it is a symbol table.  */
      GElf_Shdr shdr_mem;
      GElf_Shdr *shdr = gelf_getshdr (scn, &shdr_mem);

      if (shdr == NULL)
	{
	  error (0, 0,
		 _("getting get section header of section %zu: %s"),
		 elf_ndxscn (scn), elf_errmsg (-1));
	  goto err_elf_close;
	}

      switch (shdr->sh_type)
	{
	case SHT_DYNAMIC:
	  if (!seen_dynamic)
	    {
	      seen_dynamic = true;

	      Elf_Data *data = elf_getdata (scn, NULL);
	      size_t entries = (shdr->sh_entsize == 0
				? 0 : shdr->sh_size / shdr->sh_entsize);

	      for (size_t cnt = 0; cnt < entries; ++cnt)
		{
		  GElf_Dyn dynmem;
		  GElf_Dyn *dyn;

		  dyn = gelf_getdyn (data, cnt, &dynmem);
		  if (dyn == NULL)
		    {
		      error (0, 0, _("cannot read dynamic section: %s"),
			     elf_errmsg (-1));
		      goto err_elf_close;
		    }

		  if (dyn->d_tag == DT_TEXTREL
		      || (dyn->d_tag == DT_FLAGS
			  && (dyn->d_un.d_val & DF_TEXTREL) != 0))
		    have_textrel = true;
		}
	    }
	  break;

	case SHT_SYMTAB:
	  symscn = scn;
	  break;
	}
    }

  if (!have_textrel)
    {
      error (0, 0, _("no text relocations reported in '%s'"), fname);
      goto err_elf_close;
    }

  int fd2 = -1;
  Elf *elf2 = NULL;
  /* Get the address ranges for the loaded segments.  */
  size_t nsegments_max = 10;
  size_t nsegments = 0;
  struct segments *segments = malloc (nsegments_max * sizeof (segments[0]));
  if (segments == NULL)
    error (1, errno, _("while reading ELF file"));

  size_t phnum;
  if (elf_getphdrnum (elf, &phnum) != 0)
    error (1, 0, _("cannot get program header count: %s"),
           elf_errmsg (-1));


  for (size_t i = 0; i < phnum; ++i)
    {
      GElf_Phdr phdr_mem;
      GElf_Phdr *phdr = gelf_getphdr (elf, i, &phdr_mem);
      if (phdr == NULL)
	{
	  error (0, 0,
		 _("cannot get program header index at offset %zd: %s"),
		 i, elf_errmsg (-1));
	  result = 1;
	  goto next;
	}

      if (phdr->p_type == PT_LOAD && (phdr->p_flags & PF_W) == 0)
	{
	  if (nsegments == nsegments_max)
	    {
	      nsegments_max *= 2;
	      segments
		= realloc (segments, nsegments_max * sizeof (segments[0]));
	      if (segments == NULL)
		{
		  error (0, 0, _("\
cannot get program header index at offset %zd: %s"),
			 i, elf_errmsg (-1));
		  result = 1;
		  goto next;
		}
	    }

	  segments[nsegments].from = phdr->p_vaddr;
	  segments[nsegments].to = phdr->p_vaddr + phdr->p_memsz;
	  ++nsegments;
	}
    }

  if (nsegments > 0)
    {

      Dwarf *dw = dwarf_begin_elf (elf, DWARF_C_READ, NULL);
      /* Look for debuginfo files if the information is not the in
	 opened file itself.  This makes only sense if the input file
	 is specified with an absolute path.  */
      if (dw == NULL && fname[0] == '/')
	{
	  char *difname =
	    xasprintf("%s%s/%s.debug", rootdir, debuginfo_root, fname);
	  fd2 = open (difname, O_RDONLY);
	  free (difname);
	  if (fd2 != -1
	      && (elf2 = elf_begin (fd2, ELF_C_READ_MMAP, NULL)) != NULL)
	    dw = dwarf_begin_elf (elf2, DWARF_C_READ, NULL);
	}

      /* Look at all relocations and determine which modify
	 write-protected segments.  */
      scn = NULL;
      while ((scn = elf_nextscn (elf, scn)) != NULL)
	{
	  /* Handle the section if it is a symbol table.  */
	  GElf_Shdr shdr_mem;
	  GElf_Shdr *shdr = gelf_getshdr (scn, &shdr_mem);

	  if (shdr == NULL)
	    {
	      error (0, 0,
		     _("cannot get section header of section %zu: %s"),
		     elf_ndxscn (scn), elf_errmsg (-1));
	      result = 1;
	      goto next;
	    }

	  if ((shdr->sh_type == SHT_REL || shdr->sh_type == SHT_RELA)
	      && symscn == NULL)
	    {
	      symscn = elf_getscn (elf, shdr->sh_link);
	      if (symscn == NULL)
		{
		  error (0, 0, _("\
cannot get symbol table section %zu in '%s': %s"),
			 (size_t) shdr->sh_link, fname, elf_errmsg (-1));
		  result = 1;
		  goto next;
		}
	    }

	  if (shdr->sh_type == SHT_REL)
	    {
	      Elf_Data *data = elf_getdata (scn, NULL);
	      size_t entries = (shdr->sh_entsize == 0
				? 0 : shdr->sh_size / shdr->sh_entsize);

	      for (int cnt = 0;
		   (size_t) cnt < entries; ++cnt)
		{
		  GElf_Rel rel_mem;
		  GElf_Rel *rel = gelf_getrel (data, cnt, &rel_mem);
		  if (rel == NULL)
		    {
		      error (0, 0, _("\
cannot get relocation at index %d in section %zu in '%s': %s"),
			     cnt, elf_ndxscn (scn), fname, elf_errmsg (-1));
		      result = 1;
		      goto next;
		    }

		  check_rel (nsegments, segments, rel->r_offset, elf,
			     symscn, dw, fname, more_than_one, &knownsrcs);
		}
	    }
	  else if (shdr->sh_type == SHT_RELA)
	    {
	      Elf_Data *data = elf_getdata (scn, NULL);
	      size_t entries = (shdr->sh_entsize == 0
				? 0 : shdr->sh_size / shdr->sh_entsize);

	      for (int cnt = 0; (size_t) cnt < entries; ++cnt)
		{
		  GElf_Rela rela_mem;
		  GElf_Rela *rela = gelf_getrela (data, cnt, &rela_mem);
		  if (rela == NULL)
		    {
		      error (0, 0, _("\
cannot get relocation at index %d in section %zu in '%s': %s"),
			     cnt, elf_ndxscn (scn), fname, elf_errmsg (-1));
		      result = 1;
		      goto next;
		    }

		  check_rel (nsegments, segments, rela->r_offset, elf,
			     symscn, dw, fname, more_than_one, &knownsrcs);
		}
	    }
	}

      dwarf_end (dw);
    }

 next:
  elf_end (elf);
  elf_end (elf2);
  close (fd);
  if (fd2 != -1)
    close (fd2);

  free (segments);
  tdestroy (knownsrcs, noop);

  return result;
}


static int
ptrcompare (const void *p1, const void *p2)
{
  if ((uintptr_t) p1 < (uintptr_t) p2)
    return -1;
  if ((uintptr_t) p1 > (uintptr_t) p2)
    return 1;
  return 0;
}


static void
check_rel (size_t nsegments, struct segments segments[nsegments],
	   GElf_Addr addr, Elf *elf, Elf_Scn *symscn, Dwarf *dw,
	   const char *fname, bool more_than_one, void **knownsrcs)
{
  for (size_t cnt = 0; cnt < nsegments; ++cnt)
    if (segments[cnt].from <= addr && segments[cnt].to > addr)
      {
	Dwarf_Die die_mem;
	Dwarf_Die *die;
	Dwarf_Line *line;
	const char *src;

	if (more_than_one)
	  printf ("%s: ", fname);

	if ((die = dwarf_addrdie (dw, addr, &die_mem)) != NULL
	    && (line = dwarf_getsrc_die (die, addr)) != NULL
	    && (src = dwarf_linesrc (line, NULL, NULL)) != NULL)
	  {
	    /* There can be more than one relocation against one file.
	       Try to avoid multiple messages.  And yes, the code uses
	       pointer comparison.  */
	    if (tfind (src, knownsrcs, ptrcompare) == NULL)
	      {
		printf (_("%s not compiled with -fpic/-fPIC\n"), src);
		tsearch (src, knownsrcs, ptrcompare);
	      }
	    return;
	  }
	else
	  {
	    /* At least look at the symbol table to see which function
	       the modified address is in.  */
	    Elf_Data *symdata = elf_getdata (symscn, NULL);
	    GElf_Shdr shdr_mem;
	    GElf_Shdr *shdr = gelf_getshdr (symscn, &shdr_mem);
	    if (shdr != NULL)
	      {
		GElf_Addr lowaddr = 0;
		int lowidx = -1;
		GElf_Addr highaddr = ~0ul;
		int highidx = -1;
		GElf_Sym sym_mem;
		GElf_Sym *sym;
		size_t entries = (shdr->sh_entsize == 0
				  ? 0 : shdr->sh_size / shdr->sh_entsize);

		for (int i = 0; (size_t) i < entries; ++i)
		  {
		    sym = gelf_getsym (symdata, i, &sym_mem);
		    if (sym == NULL)
		      continue;

		    if (sym->st_value < addr && sym->st_value > lowaddr)
		      {
			lowaddr = sym->st_value;
			lowidx = i;
		      }
		    if (sym->st_value > addr && sym->st_value < highaddr)
		      {
			highaddr = sym->st_value;
			highidx = i;
		      }
		  }

		if (lowidx != -1)
		  {
		    sym = gelf_getsym (symdata, lowidx, &sym_mem);
		    assert (sym != NULL);

		    const char *lowstr = elf_strptr (elf, shdr->sh_link,
						     sym->st_name);

		    if (sym->st_value + sym->st_size > addr)
		      {
			/* It is this function.  */
			if (tfind (lowstr, knownsrcs, ptrcompare) == NULL)
			  {
			    printf (_("\
the file containing the function '%s' is not compiled with -fpic/-fPIC\n"),
				    lowstr);
			    tsearch (lowstr, knownsrcs, ptrcompare);
			  }
		      }
		    else if (highidx == -1)
		      printf (_("\
the file containing the function '%s' might not be compiled with -fpic/-fPIC\n"),
			      lowstr);
		    else
		      {
			sym = gelf_getsym (symdata, highidx, &sym_mem);
			assert (sym != NULL);

			printf (_("\
either the file containing the function '%s' or the file containing the function '%s' is not compiled with -fpic/-fPIC\n"),
				lowstr, elf_strptr (elf, shdr->sh_link,
						    sym->st_name));
		      }
		    return;
		  }
		else if (highidx != -1)
		  {
		    sym = gelf_getsym (symdata, highidx, &sym_mem);
		    assert (sym != NULL);

		    printf (_("\
the file containing the function '%s' might not be compiled with -fpic/-fPIC\n"),
			    elf_strptr (elf, shdr->sh_link, sym->st_name));
		    return;
		  }
	      }
	  }

	printf (_("\
a relocation modifies memory at offset %llu in a write-protected segment\n"),
		(unsigned long long int) addr);
	break;
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