# Milestone #1
add_executable(lab7_m1.elf main_m1.c testBoards.c minimax.c)
target_link_libraries(lab7_m1.elf ${330_LIBS} touchscreen)
set_target_properties(lab7_m1.elf PROPERTIES LINKER_LANGUAGE CXX)

#Milestone #2
add_executable(lab7_m2.elf main_m2.c ticTacToeDisplay.c minimax.c testBoards.c ticTacToeControl.c)
target_link_libraries(lab7_m2.elf ${330_LIBS} touchscreen intervalTimer interrupts buttons)
set_target_properties(lab7_m2.elf PROPERTIES LINKER_LANGUAGE CXX)

