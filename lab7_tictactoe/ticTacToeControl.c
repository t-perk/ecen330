#include "ticTacToeControl.h"
#include "xparameters.h"
#include "xil_io.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define DISPLAY_TEXT_TIME 3
#define COMPUTER_BEGINNING_WAIT_TIME 3

//State messages
#define INIT_ST_MSG "ticTacToe_init_st\n"

uint32_t disp_text_cnt = 0;
uint32_t disp_text_num_ticks = 0;

enum ticTacToeControl_st_t {
    init_st,        //Starts here. Init buttons and clear screen
    draw_txt_st,    //Draw opening message
    delay_txt_st,   //Necessary?
    erase_txt_st,   //Erase opening message
    draw_lines_st,  //Draw ticTacToe lines

};
//Declare currentState var
static enum ticTacToeControl_st_t currentState;

// Initialize the tic-tac-toe controller state machine,
// providing the tick period, in seconds.
void ticTacToeControl_init(double period_s){

    //Set current state
    currentState = init_st;

    //Initialize values
    disp_text_cnt = 0;

    //Setup number of ticks
    disp_text_num_ticks = DISPLAY_TEXT_TIME / period_s;
}

// This is a debug state print routine. It will print the names of the states each
// time tick() is called. It only prints states if they are different than the
// previous state.
void debugStatePrint_ticTacToeControl() {
  static enum ticTacToeControl_st_t previousState;
  static bool firstPass = true;
  // Only print the message if:
  // 1. This the first pass and the value for previousState is unknown.
  // 2. previousState != currentState - this prevents reprinting the same state name over and over.
  if (previousState != currentState || firstPass) {
    firstPass = false;                // previousState will be defined, firstPass is false.
    previousState = currentState;     // keep track of the last state that you were in.
    switch(currentState) {            // This prints messages based upon the state that you were in.
        case init_st:
            printf(INIT_ST_MSG);
            break;
     }
  }
}

// Tick the tic-tac-toe controller state machine
void ticTacToeControl_tick(){
    //Debug function
    debugStatePrint_ticTacToeControl();

    //Handle transitions
    switch (currentState){

    }

    //Hanlde state actions
    switch (currentState){
        
    }

}


