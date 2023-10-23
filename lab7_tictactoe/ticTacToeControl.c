#include "ticTacToeControl.h"
#include "xparameters.h"
#include "xil_io.h"
#include "minimax.h"
#include "ticTacToeDisplay.h"
#include "ticTacToe.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define DISPLAY_TEXT_TIME 3
#define COMPUTER_BEGINNING_WAIT_TIME 3

static tictactoe_board_t board;
static bool is_Xs_turn = true;

//State messages
#define INIT_ST_MSG "ticTacToe_init_st\n"

uint32_t disp_text_cnt = 0;
uint32_t disp_text_num_ticks = 0;

uint32_t comp_beg_wait_cnt = 0;
uint32_t comp_beg_wait_num_ticks = 0;

enum ticTacToeControl_st_t {
    init_st,        //Starts here. Init buttons and clear screen
    draw_txt_st,    //Draw opening message
    delay_txt_st,   //Necessary?
    erase_txt_st,   //Erase opening message
    draw_lines_st,  //Draw ticTacToe lines
    comp_beg_wait_st,//Wait for player to play or begin computer
    computer_move_st,
    player_move_st,

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
    comp_beg_wait_cnt = 0;

    //Setup number of ticks
    disp_text_num_ticks = DISPLAY_TEXT_TIME / period_s;
    comp_beg_wait_num_ticks = COMPUTER_BEGINNING_WAIT_TIME / period_s;
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
        case init_st:
            currentState = delay_txt_st;
            //Incorporate mealy state here for drawing starting text
            //drawText();
            break;
        case delay_txt_st:
            if (disp_text_cnt == disp_text_num_ticks){
                //erase the txt
                //eraseBoard();
                //display the lines
                //dispLines();
                currentState = comp_beg_wait_st;
            }else{
                currentState = delay_txt_st;
            }
            break;
        case comp_beg_wait_st:
            if (comp_beg_wait_cnt = comp_beg_wait_num_ticks){
                //Do computer's turn
                tictactoe_location_t move = minimax_computeNextMove(&board, is_Xs_turn);
                //Need to validate the there is nothing at that location 
                //at the board?
                ticTacToeDisplay_drawX(move, false);
                board.squares[move.row][move.column] = MINIMAX_X_SQUARE;
                //currentState = computer_move_st;
            // }else if(receivedPlayerInput()){
            //     //Check for input
            //     currentState = player_move_st;
            }else{
                currentState = comp_beg_wait_st;
            }
        default:
            printf("ERROR: Unaccounted state transition.\n");
    }

    //Hanlde state actions
    switch (currentState){
        case init_st:
            break;
        case delay_txt_st:
            disp_text_cnt++;
            break;
        case comp_beg_wait_st:
            comp_beg_wait_cnt++;
        default:
            printf("ERROR: Unaccounted state action.\n");
    }

}


