# CMAKE generated file: DO NOT EDIT!
# Generated by "Unix Makefiles" Generator, CMake Version 3.23

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
CMAKE_COMMAND = /usr/local/Cellar/cmake/3.23.2/bin/cmake

# The command to remove a file.
RM = /usr/local/Cellar/cmake/3.23.2/bin/cmake -E rm -f

# Escaping for special characters.
EQUALS = =

# The top-level source directory on which CMake was run.
CMAKE_SOURCE_DIR = /Users/marlowe/workspace/silero-vad

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /Users/marlowe/workspace/silero-vad/build

# Include any dependencies generated for this target.
include utils/CMakeFiles/utils.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include utils/CMakeFiles/utils.dir/compiler_depend.make

# Include the progress variables for this target.
include utils/CMakeFiles/utils.dir/progress.make

# Include the compile flags for this target's objects.
include utils/CMakeFiles/utils.dir/flags.make

utils/CMakeFiles/utils.dir/string.cc.o: utils/CMakeFiles/utils.dir/flags.make
utils/CMakeFiles/utils.dir/string.cc.o: ../utils/string.cc
utils/CMakeFiles/utils.dir/string.cc.o: utils/CMakeFiles/utils.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/marlowe/workspace/silero-vad/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building CXX object utils/CMakeFiles/utils.dir/string.cc.o"
	cd /Users/marlowe/workspace/silero-vad/build/utils && /Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT utils/CMakeFiles/utils.dir/string.cc.o -MF CMakeFiles/utils.dir/string.cc.o.d -o CMakeFiles/utils.dir/string.cc.o -c /Users/marlowe/workspace/silero-vad/utils/string.cc

utils/CMakeFiles/utils.dir/string.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/utils.dir/string.cc.i"
	cd /Users/marlowe/workspace/silero-vad/build/utils && /Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/marlowe/workspace/silero-vad/utils/string.cc > CMakeFiles/utils.dir/string.cc.i

utils/CMakeFiles/utils.dir/string.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/utils.dir/string.cc.s"
	cd /Users/marlowe/workspace/silero-vad/build/utils && /Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/marlowe/workspace/silero-vad/utils/string.cc -o CMakeFiles/utils.dir/string.cc.s

utils/CMakeFiles/utils.dir/utils.cc.o: utils/CMakeFiles/utils.dir/flags.make
utils/CMakeFiles/utils.dir/utils.cc.o: ../utils/utils.cc
utils/CMakeFiles/utils.dir/utils.cc.o: utils/CMakeFiles/utils.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/Users/marlowe/workspace/silero-vad/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building CXX object utils/CMakeFiles/utils.dir/utils.cc.o"
	cd /Users/marlowe/workspace/silero-vad/build/utils && /Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -MD -MT utils/CMakeFiles/utils.dir/utils.cc.o -MF CMakeFiles/utils.dir/utils.cc.o.d -o CMakeFiles/utils.dir/utils.cc.o -c /Users/marlowe/workspace/silero-vad/utils/utils.cc

utils/CMakeFiles/utils.dir/utils.cc.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing CXX source to CMakeFiles/utils.dir/utils.cc.i"
	cd /Users/marlowe/workspace/silero-vad/build/utils && /Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -E /Users/marlowe/workspace/silero-vad/utils/utils.cc > CMakeFiles/utils.dir/utils.cc.i

utils/CMakeFiles/utils.dir/utils.cc.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling CXX source to assembly CMakeFiles/utils.dir/utils.cc.s"
	cd /Users/marlowe/workspace/silero-vad/build/utils && /Library/Developer/CommandLineTools/usr/bin/c++ $(CXX_DEFINES) $(CXX_INCLUDES) $(CXX_FLAGS) -S /Users/marlowe/workspace/silero-vad/utils/utils.cc -o CMakeFiles/utils.dir/utils.cc.s

# Object files for target utils
utils_OBJECTS = \
"CMakeFiles/utils.dir/string.cc.o" \
"CMakeFiles/utils.dir/utils.cc.o"

# External object files for target utils
utils_EXTERNAL_OBJECTS =

utils/libutils.a: utils/CMakeFiles/utils.dir/string.cc.o
utils/libutils.a: utils/CMakeFiles/utils.dir/utils.cc.o
utils/libutils.a: utils/CMakeFiles/utils.dir/build.make
utils/libutils.a: utils/CMakeFiles/utils.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/Users/marlowe/workspace/silero-vad/build/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX static library libutils.a"
	cd /Users/marlowe/workspace/silero-vad/build/utils && $(CMAKE_COMMAND) -P CMakeFiles/utils.dir/cmake_clean_target.cmake
	cd /Users/marlowe/workspace/silero-vad/build/utils && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/utils.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
utils/CMakeFiles/utils.dir/build: utils/libutils.a
.PHONY : utils/CMakeFiles/utils.dir/build

utils/CMakeFiles/utils.dir/clean:
	cd /Users/marlowe/workspace/silero-vad/build/utils && $(CMAKE_COMMAND) -P CMakeFiles/utils.dir/cmake_clean.cmake
.PHONY : utils/CMakeFiles/utils.dir/clean

utils/CMakeFiles/utils.dir/depend:
	cd /Users/marlowe/workspace/silero-vad/build && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /Users/marlowe/workspace/silero-vad /Users/marlowe/workspace/silero-vad/utils /Users/marlowe/workspace/silero-vad/build /Users/marlowe/workspace/silero-vad/build/utils /Users/marlowe/workspace/silero-vad/build/utils/CMakeFiles/utils.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : utils/CMakeFiles/utils.dir/depend

