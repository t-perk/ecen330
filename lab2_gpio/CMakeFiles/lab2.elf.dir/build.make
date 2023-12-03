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
include lab2_gpio/CMakeFiles/lab2.elf.dir/depend.make
# Include any dependencies generated by the compiler for this target.
include lab2_gpio/CMakeFiles/lab2.elf.dir/compiler_depend.make

# Include the progress variables for this target.
include lab2_gpio/CMakeFiles/lab2.elf.dir/progress.make

# Include the compile flags for this target's objects.
include lab2_gpio/CMakeFiles/lab2.elf.dir/flags.make

lab2_gpio/CMakeFiles/lab2.elf.dir/main.c.o: lab2_gpio/CMakeFiles/lab2.elf.dir/flags.make
lab2_gpio/CMakeFiles/lab2.elf.dir/main.c.o: lab2_gpio/main.c
lab2_gpio/CMakeFiles/lab2.elf.dir/main.c.o: lab2_gpio/CMakeFiles/lab2.elf.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/fsf/tylerp16/myECEN330/ecen330/CMakeFiles --progress-num=$(CMAKE_PROGRESS_1) "Building C object lab2_gpio/CMakeFiles/lab2.elf.dir/main.c.o"
	cd /fsf/tylerp16/myECEN330/ecen330/lab2_gpio && arm-none-eabi-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT lab2_gpio/CMakeFiles/lab2.elf.dir/main.c.o -MF CMakeFiles/lab2.elf.dir/main.c.o.d -o CMakeFiles/lab2.elf.dir/main.c.o -c /fsf/tylerp16/myECEN330/ecen330/lab2_gpio/main.c

lab2_gpio/CMakeFiles/lab2.elf.dir/main.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/lab2.elf.dir/main.c.i"
	cd /fsf/tylerp16/myECEN330/ecen330/lab2_gpio && arm-none-eabi-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /fsf/tylerp16/myECEN330/ecen330/lab2_gpio/main.c > CMakeFiles/lab2.elf.dir/main.c.i

lab2_gpio/CMakeFiles/lab2.elf.dir/main.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/lab2.elf.dir/main.c.s"
	cd /fsf/tylerp16/myECEN330/ecen330/lab2_gpio && arm-none-eabi-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /fsf/tylerp16/myECEN330/ecen330/lab2_gpio/main.c -o CMakeFiles/lab2.elf.dir/main.c.s

lab2_gpio/CMakeFiles/lab2.elf.dir/gpioTest.c.o: lab2_gpio/CMakeFiles/lab2.elf.dir/flags.make
lab2_gpio/CMakeFiles/lab2.elf.dir/gpioTest.c.o: lab2_gpio/gpioTest.c
lab2_gpio/CMakeFiles/lab2.elf.dir/gpioTest.c.o: lab2_gpio/CMakeFiles/lab2.elf.dir/compiler_depend.ts
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --progress-dir=/fsf/tylerp16/myECEN330/ecen330/CMakeFiles --progress-num=$(CMAKE_PROGRESS_2) "Building C object lab2_gpio/CMakeFiles/lab2.elf.dir/gpioTest.c.o"
	cd /fsf/tylerp16/myECEN330/ecen330/lab2_gpio && arm-none-eabi-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -MD -MT lab2_gpio/CMakeFiles/lab2.elf.dir/gpioTest.c.o -MF CMakeFiles/lab2.elf.dir/gpioTest.c.o.d -o CMakeFiles/lab2.elf.dir/gpioTest.c.o -c /fsf/tylerp16/myECEN330/ecen330/lab2_gpio/gpioTest.c

lab2_gpio/CMakeFiles/lab2.elf.dir/gpioTest.c.i: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Preprocessing C source to CMakeFiles/lab2.elf.dir/gpioTest.c.i"
	cd /fsf/tylerp16/myECEN330/ecen330/lab2_gpio && arm-none-eabi-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -E /fsf/tylerp16/myECEN330/ecen330/lab2_gpio/gpioTest.c > CMakeFiles/lab2.elf.dir/gpioTest.c.i

lab2_gpio/CMakeFiles/lab2.elf.dir/gpioTest.c.s: cmake_force
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green "Compiling C source to assembly CMakeFiles/lab2.elf.dir/gpioTest.c.s"
	cd /fsf/tylerp16/myECEN330/ecen330/lab2_gpio && arm-none-eabi-gcc $(C_DEFINES) $(C_INCLUDES) $(C_FLAGS) -S /fsf/tylerp16/myECEN330/ecen330/lab2_gpio/gpioTest.c -o CMakeFiles/lab2.elf.dir/gpioTest.c.s

# Object files for target lab2.elf
lab2_elf_OBJECTS = \
"CMakeFiles/lab2.elf.dir/main.c.o" \
"CMakeFiles/lab2.elf.dir/gpioTest.c.o"

# External object files for target lab2.elf
lab2_elf_EXTERNAL_OBJECTS =

lab2_gpio/lab2.elf: lab2_gpio/CMakeFiles/lab2.elf.dir/main.c.o
lab2_gpio/lab2.elf: lab2_gpio/CMakeFiles/lab2.elf.dir/gpioTest.c.o
lab2_gpio/lab2.elf: lab2_gpio/CMakeFiles/lab2.elf.dir/build.make
lab2_gpio/lab2.elf: drivers/libbuttons_switches.a
lab2_gpio/lab2.elf: lab2_gpio/CMakeFiles/lab2.elf.dir/link.txt
	@$(CMAKE_COMMAND) -E cmake_echo_color --switch=$(COLOR) --green --bold --progress-dir=/fsf/tylerp16/myECEN330/ecen330/CMakeFiles --progress-num=$(CMAKE_PROGRESS_3) "Linking CXX executable lab2.elf"
	cd /fsf/tylerp16/myECEN330/ecen330/lab2_gpio && $(CMAKE_COMMAND) -E cmake_link_script CMakeFiles/lab2.elf.dir/link.txt --verbose=$(VERBOSE)

# Rule to build all files generated by this target.
lab2_gpio/CMakeFiles/lab2.elf.dir/build: lab2_gpio/lab2.elf
.PHONY : lab2_gpio/CMakeFiles/lab2.elf.dir/build

lab2_gpio/CMakeFiles/lab2.elf.dir/clean:
	cd /fsf/tylerp16/myECEN330/ecen330/lab2_gpio && $(CMAKE_COMMAND) -P CMakeFiles/lab2.elf.dir/cmake_clean.cmake
.PHONY : lab2_gpio/CMakeFiles/lab2.elf.dir/clean

lab2_gpio/CMakeFiles/lab2.elf.dir/depend:
	cd /fsf/tylerp16/myECEN330/ecen330 && $(CMAKE_COMMAND) -E cmake_depends "Unix Makefiles" /fsf/tylerp16/myECEN330/ecen330 /fsf/tylerp16/myECEN330/ecen330/lab2_gpio /fsf/tylerp16/myECEN330/ecen330 /fsf/tylerp16/myECEN330/ecen330/lab2_gpio /fsf/tylerp16/myECEN330/ecen330/lab2_gpio/CMakeFiles/lab2.elf.dir/DependInfo.cmake --color=$(COLOR)
.PHONY : lab2_gpio/CMakeFiles/lab2.elf.dir/depend

