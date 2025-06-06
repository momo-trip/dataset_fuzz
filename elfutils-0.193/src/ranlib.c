/* Generate an index to speed access to archives.
   Copyright (C) 2005-2012 Red Hat, Inc.
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

#include <ar.h>
#include <argp.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <gelf.h>
#include <locale.h>
#include <obstack.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdio_ext.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/stat.h>

#include <system.h>
#include <printversion.h>

#include "arlib.h"


/* Prototypes for local functions.  */
static int handle_file (const char *fname);


/* Name and version of program.  */
ARGP_PROGRAM_VERSION_HOOK_DEF = print_version;

/* Bug report address.  */
ARGP_PROGRAM_BUG_ADDRESS_DEF = PACKAGE_BUGREPORT;


/* Definitions of arguments for argp functions.  */
static const struct argp_option options[] =
{
  { NULL, 0, NULL, 0, NULL, 0 }
};

/* Short description of program.  */
static const char doc[] = N_("Generate an index to speed access to archives.");

/* Strings for arguments in help texts.  */
static const char args_doc[] = N_("ARCHIVE");

/* Data structure to communicate with argp functions.  */
static const struct argp argp =
{
  options, NULL, args_doc, doc, arlib_argp_children, NULL, NULL
};


int
original_main (int argc, char *argv[])
{
  /* We use no threads here which can interfere with handling a stream.  */
  (void) __fsetlocking (stdin, FSETLOCKING_BYCALLER);
  (void) __fsetlocking (stdout, FSETLOCKING_BYCALLER);
  (void) __fsetlocking (stderr, FSETLOCKING_BYCALLER);

  /* Set locale.  */
  (void) setlocale (LC_ALL, "");

  /* Make sure the message catalog can be found.  */
  (void) bindtextdomain (PACKAGE_TARNAME, LOCALEDIR);

  /* Initialize the message catalog.  */
  (void) textdomain (PACKAGE_TARNAME);

  /* Parse and process arguments.  */
  int remaining;
  (void) argp_parse (&argp, argc, argv, ARGP_IN_ORDER, &remaining, NULL);

  /* Tell the library which version we are expecting.  */
  (void) elf_version (EV_CURRENT);

  /* There must at least be one more parameter specifying the archive.   */
  if (remaining == argc)
    {
      error (0, 0, _("Archive name required"));
      argp_help (&argp, stderr, ARGP_HELP_SEE, "ranlib");
      exit (EXIT_FAILURE);
    }

  /* We accept the names of multiple archives.  */
  int status = 0;
  do
    status |= handle_file (argv[remaining]);
  while (++remaining < argc);

  return status;
}


static int
copy_content (Elf *elf, int newfd, off_t off, size_t n)
{
  size_t len;
  char *rawfile = elf_rawfile (elf, &len);

  assert (off + n <= len);

  /* Tell the kernel we will read all the pages sequentially.  */
  size_t ps = sysconf (_SC_PAGESIZE);
  if (n > 2 * ps)
    posix_madvise (rawfile + (off & ~(ps - 1)), n, POSIX_MADV_SEQUENTIAL);

  return write_retry (newfd, rawfile + off, n) != (ssize_t) n;
}


/* Handle a file given on the command line.  */
static int
handle_file (const char *fname)
{
  int fd = open (fname, O_RDONLY);
  if (fd == -1)
    {
      error (0, errno, _("cannot open '%s'"), fname);
      return 1;
    }

  struct stat st;
  if (fstat (fd, &st) != 0)
    {
      error (0, errno, _("cannot stat '%s'"), fname);
      close (fd);
      return 1;
    }

  /* First we walk through the file, looking for all ELF files to
     collect symbols from.  */
  Elf *arelf = elf_begin (fd, ELF_C_READ_MMAP, NULL);
  if (arelf == NULL)
    {
      error (0, 0, _("cannot create ELF descriptor for '%s': %s"),
	     fname, elf_errmsg (-1));
      close (fd);
      return 1;
    }

  if (elf_kind (arelf) != ELF_K_AR)
    {
      error (0, 0, _("'%s' is no archive"), fname);
      elf_end (arelf);
      close (fd);
      return 1;
    }

  arlib_init ();

  /* Iterate over the content of the archive.  */
  off_t index_off = -1;
  size_t index_size = 0;
  off_t cur_off = SARMAG;
  Elf *elf;
  Elf_Cmd cmd = ELF_C_READ_MMAP;
  while ((elf = elf_begin (fd, cmd, arelf)) != NULL)
    {
      Elf_Arhdr *arhdr = elf_getarhdr (elf);
      assert (arhdr != NULL);

      /* If this is the index, remember the location.  */
      if (strcmp (arhdr->ar_name, "/") == 0)
	{
	  index_off = elf_getaroff (elf);
	  index_size = arhdr->ar_size;
	}
      else
	{
	  arlib_add_symbols (elf, fname, arhdr->ar_name, cur_off);
	  cur_off += (((arhdr->ar_size + 1) & ~((off_t) 1))
		      + sizeof (struct ar_hdr));
	}

      /* Get next archive element.  */
      cmd = elf_next (elf);
      if (elf_end (elf) != 0)
	error (0, 0, _("error while freeing sub-ELF descriptor: %s"),
	       elf_errmsg (-1));
    }

  arlib_finalize ();

  /* If the file contains no symbols we need not do anything.  */
  int status = 0;
  if (symtab.symsnamelen != 0
      /* We have to rewrite the file also if it initially had an index
	 but now does not need one anymore.  */
      || (symtab.symsnamelen == 0 && index_size != 0))
    {
      /* Create a new, temporary file in the same directory as the
	 original file.  */
      char tmpfname[strlen (fname) + 7];
      strcpy (stpcpy (tmpfname, fname), "XXXXXX");
      int newfd = mkstemp (tmpfname);
      if (unlikely (newfd == -1))
	{
	nonew:
	  error (0, errno, _("cannot create new file"));
	  status = 1;
	}
      else
	{
	  /* Create the header.  */
	  if (unlikely (write_retry (newfd, ARMAG, SARMAG) != SARMAG))
	    {
	      // XXX Use /prof/self/fd/%d ???
	    nonew_unlink:
	      unlink (tmpfname);
	      if (newfd != -1)
		close (newfd);
	      goto nonew;
	    }

	  /* Create the new file.  There are three parts as far we are
	     concerned: 1. original context before the index, 2. the
	     new index, 3. everything after the new index.  */
	  off_t rest_off;
	  if (index_off != -1)
	    rest_off = (index_off + sizeof (struct ar_hdr)
			+ ((index_size + 1) & ~1ul));
	  else
	    rest_off = SARMAG;

	  if (symtab.symsnamelen != 0
	      && ((write_retry (newfd, symtab.symsoff,
				symtab.symsofflen)
		   != (ssize_t) symtab.symsofflen)
		  || (write_retry (newfd, symtab.symsname,
				   symtab.symsnamelen)
		      != (ssize_t) symtab.symsnamelen)))
	    goto nonew_unlink;

	  /* Even if the original file had content before the
	     symbol table, we write it in the correct order.  */
	  if ((index_off > SARMAG
	       && copy_content (arelf, newfd, SARMAG, index_off - SARMAG))
	      || copy_content (arelf, newfd, rest_off, st.st_size - rest_off))
	    goto nonew_unlink;

	  /* Never complain about fchown failing.  */
	  if (fchown (newfd, st.st_uid, st.st_gid) != 0) { ; }
	  /* Set the mode of the new file to the same values the
	     original file has.  */
	  if (fchmod (newfd, st.st_mode & ALLPERMS) != 0)
	    goto nonew_unlink;
	  close (newfd);
	  newfd = -1;
	  if (rename (tmpfname, fname) != 0)
	    goto nonew_unlink;
	}
    }

  elf_end (arelf);

  arlib_fini ();

  close (fd);

  return status;
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