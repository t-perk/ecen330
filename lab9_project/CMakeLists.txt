# Milestone #1
add_executable(lab9.elf main.c gameControl.c)
target_link_libraries(lab9.elf ${330_LIBS} touchscreen intervalTimer interrupts)
set_target_properties(lab9.elf PROPERTIES LINKER_LANGUAGE CXX)