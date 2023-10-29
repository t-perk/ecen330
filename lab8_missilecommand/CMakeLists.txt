# Milestone #1
add_executable(lab8_m1.elf main_m1.c missile.c)
target_link_libraries(lab8_m1.elf ${330_LIBS} touchscreen intervalTimer interrupts)
set_target_properties(lab8_m1.elf PROPERTIES LINKER_LANGUAGE CXX)