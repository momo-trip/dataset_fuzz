# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.22

# Delete rule output on recipe failure.
.DELETE_ON_ERROR:

#=============================================================================
# Special targets provided by cmake.

# Disable implicit rules so canonical targets will work.
.SUFFIXES:

# Disable VCS-based implicit rules.
% : %,v

# Disable VCS-based implicit rules.
% : RCS/%

# Disable VCS-based implicit rules.
% : RCS/%,v

# Disable VCS-based implicit rules.
% : SCCS/s.%

# Disable VCS-based implicit rules.
% : s.%

.SUFFIXES: .hpux_make_needs_suffix_list

# Command-line flag to silence nested $(MAKE).
$(VERBOSE)MAKESILENT = -s

#Suppress display of executed commands.
$(VERBOSE).SILENT:

# A target that is always out of date.
cmake_force:
.PHONY : cmake_force

#=============================================================================
# Set environment variables for the build.

# The shell in which to execute make rules.
SHELL = /bin/sh

# The CMake executable.
CMAKE_COMMAND = /usr/bin/cmake

# The command to remove a file.
RM = /usr/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /root/dataset_fuzz/libjpeg-turbo-3.1.0

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /root/dataset_fuzz/libjpeg-turbo-3.1.0/build

# Include any dependencies generated for this target.
include CMakeFiles/tjcomp.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include CMakeFiles/tjcomp.dir/compiler_depend.make

# Include the progress variables for this target.
include CMakeFiles/tjcomp.dir/progress.make

# Include the compile flags for this target's objects.
include CMakeFiles/tjcomp.dir/flags.make

CMakeFiles/tjcomp.dir/src/tjcomp.c.o: CMakeFiles/tjcomp.dir/flags.make
CMakeFiles/tjcomp.dir/src/tjcomp.c.o: ../src/tjcomp.c
CMakeFiles/tjcomp.dir/src/tjcomp.c.o: CMakeFiles/tjcomp.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/root/dataset_fuzz/libjpeg-turbo-3.1.0/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object CMakeFiles/tjcomp.dir/src/tjcomp.c.o"
	/usr/bin/afl-clang-fast $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT CMakeFiles/tjcomp.dir/src/tjcomp.c.o -MF CMakeFiles/tjcomp.dir/src/tjcomp.c.o.d -o CMakeFiles/tjcomp.dir/src/tjcomp.c.o -c /root/dataset_fuzz/libjpeg-turbo-3.1.0/src/tjcomp.c

CMakeFiles/tjcomp.dir/src/tjcomp.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/tjcomp.dir/src/tjcomp.c.i"
	/usr/bin/afl-clang-fast $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /root/dataset_fuzz/libjpeg-turbo-3.1.0/src/tjcomp.c > CMakeFiles/tjcomp.dir/src/tjcomp.c.i

CMakeFiles/tjcomp.dir/src/tjcomp.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/tjcomp.dir/src/tjcomp.c.s"
	/usr/bin/afl-clang-fast $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /root/dataset_fuzz/libjpeg-turbo-3.1.0/src/tjcomp.c -o CMakeFiles/tjcomp.dir/src/tjcomp.c.s

# Object files for target tjcomp
tjcomp_OBJECTS = \
"CMakeFiles/tjcomp.dir/src/tjcomp.c.o"

# External object files for target tjcomp
tjcomp_EXTERNAL_OBJECTS =

tjcomp: CMakeFiles/tjcomp.dir/src/tjcomp.c.o
tjcomp: CMakeFiles/tjcomp.dir/build.make
tjcomp: libturbojpeg.so.0.4.0
tjcomp: CMakeFiles/tjcomp.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/root/dataset_fuzz/libjpeg-turbo-3.1.0/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C executable tjcomp"
	$(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/tjcomp.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
CMakeFiles/tjcomp.dir/build: tjcomp
.PHONY : CMakeFiles/tjcomp.dir/build

CMakeFiles/tjcomp.dir/clean:
	$(CMAKE_COMMAND) -P CMakeFiles/tjcomp.dir/cmake_clean.cmake
.PHONY : CMakeFiles/tjcomp.dir/clean

CMakeFiles/tjcomp.dir/depend:
	cd /root/dataset_fuzz/libjpeg-turbo-3.1.0/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /root/dataset_fuzz/libjpeg-turbo-3.1.0 /root/dataset_fuzz/libjpeg-turbo-3.1.0 /root/dataset_fuzz/libjpeg-turbo-3.1.0/build /root/dataset_fuzz/libjpeg-turbo-3.1.0/build /root/dataset_fuzz/libjpeg-turbo-3.1.0/build/CMakeFiles/tjcomp.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : CMakeFiles/tjcomp.dir/depend

