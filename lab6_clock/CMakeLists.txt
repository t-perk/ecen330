add_executable(lab6.elf main.c clockDisplay.c clockControl.c)
target_link_libraries(lab6.elf ${330_LIBS} intervalTimer interrupts touchscreen)
set_target_properties(lab6.elf PROPERTIES LINKER_LANGUAGE CXX)