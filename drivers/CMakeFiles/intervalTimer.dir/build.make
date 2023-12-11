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
CMAKE_SOURCE_DIR = /fsf/tylerp16/myECEN330/ecen330

# The top-level build directory on which CMake was run.
CMAKE_BINARY_DIR = /fsf/tylerp16/myECEN330/ecen330

# Include any dependencies generated for this target.
include drivers/CMakeFiles/intervalTimer.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include drivers/CMakeFiles/intervalTimer.dir/compiler_depend.make

# Include the progress variables for this target.
include drivers/CMakeFiles/intervalTimer.dir/progress.make

# Include the compile flags for this target's objects.
include drivers/CMakeFiles/intervalTimer.dir/flags.make

drivers/CMakeFiles/intervalTimer.dir/intervalTimer.c.o: drivers/CMakeFiles/intervalTimer.dir/flags.make
drivers/CMakeFiles/intervalTimer.dir/intervalTimer.c.o: drivers/intervalTimer.c
drivers/CMakeFiles/intervalTimer.dir/intervalTimer.c.o: drivers/CMakeFiles/intervalTimer.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/fsf/tylerp16/myECEN330/ecen330/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object drivers/CMakeFiles/intervalTimer.dir/intervalTimer.c.o"
	cd /fsf/tylerp16/myECEN330/ecen330/drivers && arm-none-eabi-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT drivers/CMakeFiles/intervalTimer.dir/intervalTimer.c.o -MF CMakeFiles/intervalTimer.dir/intervalTimer.c.o.d -o CMakeFiles/intervalTimer.dir/intervalTimer.c.o -c /fsf/tylerp16/myECEN330/ecen330/drivers/intervalTimer.c

drivers/CMakeFiles/intervalTimer.dir/intervalTimer.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/intervalTimer.dir/intervalTimer.c.i"
	cd /fsf/tylerp16/myECEN330/ecen330/drivers && arm-none-eabi-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /fsf/tylerp16/myECEN330/ecen330/drivers/intervalTimer.c > CMakeFiles/intervalTimer.dir/intervalTimer.c.i

drivers/CMakeFiles/intervalTimer.dir/intervalTimer.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/intervalTimer.dir/intervalTimer.c.s"
	cd /fsf/tylerp16/myECEN330/ecen330/drivers && arm-none-eabi-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /fsf/tylerp16/myECEN330/ecen330/drivers/intervalTimer.c -o CMakeFiles/intervalTimer.dir/intervalTimer.c.s

# Object files for target intervalTimer
intervalTimer_OBJECTS = \
"CMakeFiles/intervalTimer.dir/intervalTimer.c.o"

# External object files for target intervalTimer
intervalTimer_EXTERNAL_OBJECTS =

drivers/libintervalTimer.a: drivers/CMakeFiles/intervalTimer.dir/intervalTimer.c.o
drivers/libintervalTimer.a: drivers/CMakeFiles/intervalTimer.dir/build.make
drivers/libintervalTimer.a: drivers/CMakeFiles/intervalTimer.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/fsf/tylerp16/myECEN330/ecen330/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Linking C static library libintervalTimer.a"
	cd /fsf/tylerp16/myECEN330/ecen330/drivers && $(CMAKE_COMMAND) -P CMakeFiles/intervalTimer.dir/cmake_clean_target.cmake
	cd /fsf/tylerp16/myECEN330/ecen330/drivers && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/intervalTimer.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
drivers/CMakeFiles/intervalTimer.dir/build: drivers/libintervalTimer.a
.PHONY : drivers/CMakeFiles/intervalTimer.dir/build

drivers/CMakeFiles/intervalTimer.dir/clean:
	cd /fsf/tylerp16/myECEN330/ecen330/drivers && $(CMAKE_COMMAND) -P CMakeFiles/intervalTimer.dir/cmake_clean.cmake
.PHONY : drivers/CMakeFiles/intervalTimer.dir/clean

drivers/CMakeFiles/intervalTimer.dir/depend:
	cd /fsf/tylerp16/myECEN330/ecen330 && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /fsf/tylerp16/myECEN330/ecen330 /fsf/tylerp16/myECEN330/ecen330/drivers /fsf/tylerp16/myECEN330/ecen330 /fsf/tylerp16/myECEN330/ecen330/drivers /fsf/tylerp16/myECEN330/ecen330/drivers/CMakeFiles/intervalTimer.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : drivers/CMakeFiles/intervalTimer.dir/depend
