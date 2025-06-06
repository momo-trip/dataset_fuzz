/*
 * Copyright (C)2022-2023 D. R. Commander.  All Rights Reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *
 * - Redistributions of source code must retain the above copyright notice,
 *   this list of conditions and the following disclaimer.
 * - Redistributions in binary form must reproduce the above copyright notice,
 *   this list of conditions and the following disclaimer in the documentation
 *   and/or other materials provided with the distribution.
 * - Neither the name of the libjpeg-turbo Project nor the names of its
 *   contributors may be used to endorse or promote products derived from this
 *   software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS",
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE COPYRIGHT HOLDERS OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include "jinclude.h"
#include <errno.h>


#define CHECK_VALUE(actual, expected, desc) \
  if (actual != expected) { \
    printf("ERROR in line %d: " desc " is %d, should be %d\n", \
           __LINE__, actual, expected); \
    return -1; \
  }

#define CHECK_ERRNO(errno_return, expected_errno) \
  CHECK_VALUE(errno_return, expected_errno, "Return value") \
  CHECK_VALUE(errno, expected_errno, "errno") \


#ifdef _MSC_VER

void invalid_parameter_handler(const wchar_t *expression,
                               const wchar_t *function, const wchar_t *file,
                               unsigned int line, uintptr_t pReserved)
{
}

#endif


int original_main(int argc, char **argv)
{
#if !defined(NO_GETENV) || !defined(NO_PUTENV)
  int err;
#endif
#ifndef NO_GETENV
  char env[3];
#endif

#ifdef _MSC_VER
  _set_invalid_parameter_handler(invalid_parameter_handler);
#endif

  /***************************************************************************/

#ifndef NO_PUTENV

  printf("PUTENV_S():\n");

  errno = 0;
  err = PUTENV_S(NULL, "12");
  CHECK_ERRNO(err, EINVAL);

  errno = 0;
  err = PUTENV_S("TESTENV", NULL);
  CHECK_ERRNO(err, EINVAL);

  errno = 0;
  err = PUTENV_S("TESTENV", "12");
  CHECK_ERRNO(err, 0);

  printf("SUCCESS!\n\n");

#endif

  /***************************************************************************/

#ifndef NO_GETENV

  printf("GETENV_S():\n");

  errno = 0;
  env[0] = 1;
  env[1] = 2;
  env[2] = 3;
  err = GETENV_S(env, 3, NULL);
  CHECK_ERRNO(err, 0);
  CHECK_VALUE(env[0], 0, "env[0]");
  CHECK_VALUE(env[1], 2, "env[1]");
  CHECK_VALUE(env[2], 3, "env[2]");

  errno = 0;
  env[0] = 1;
  env[1] = 2;
  env[2] = 3;
  err = GETENV_S(env, 3, "TESTENV2");
  CHECK_ERRNO(err, 0);
  CHECK_VALUE(env[0], 0, "env[0]");
  CHECK_VALUE(env[1], 2, "env[1]");
  CHECK_VALUE(env[2], 3, "env[2]");

  errno = 0;
  err = GETENV_S(NULL, 3, "TESTENV");
  CHECK_ERRNO(err, EINVAL);

  errno = 0;
  err = GETENV_S(NULL, 0, "TESTENV");
  CHECK_ERRNO(err, 0);

  errno = 0;
  env[0] = 1;
  err = GETENV_S(env, 0, "TESTENV");
  CHECK_ERRNO(err, EINVAL);
  CHECK_VALUE(env[0], 1, "env[0]");

  errno = 0;
  env[0] = 1;
  env[1] = 2;
  env[2] = 3;
  err = GETENV_S(env, 1, "TESTENV");
  CHECK_VALUE(err, ERANGE, "Return value");
  CHECK_VALUE(errno, 0, "errno");
  CHECK_VALUE(env[0], 0, "env[0]");
  CHECK_VALUE(env[1], 2, "env[1]");
  CHECK_VALUE(env[2], 3, "env[2]");

  errno = 0;
  env[0] = 1;
  env[1] = 2;
  env[2] = 3;
  err = GETENV_S(env, 2, "TESTENV");
  CHECK_VALUE(err, ERANGE, "Return value");
  CHECK_VALUE(errno, 0, "errno");
  CHECK_VALUE(env[0], 0, "env[0]");
  CHECK_VALUE(env[1], 2, "env[1]");
  CHECK_VALUE(env[2], 3, "env[2]");

  errno = 0;
  env[0] = 1;
  env[1] = 2;
  env[2] = 3;
  err = GETENV_S(env, 3, "TESTENV");
  CHECK_ERRNO(err, 0);
  CHECK_VALUE(env[0], '1', "env[0]");
  CHECK_VALUE(env[1], '2', "env[1]");
  CHECK_VALUE(env[2], 0, "env[2]");

  printf("SUCCESS!\n\n");

#endif

  /***************************************************************************/

  return 0;
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

