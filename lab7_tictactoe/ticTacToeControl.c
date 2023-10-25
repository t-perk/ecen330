#include "ticTacToeControl.h"
#include "xparameters.h"
#include "xil_io.h"
#include "minimax.h"
#include "ticTacToeDisplay.h"
#include "ticTacToe.h"
#include "touchscreen.h"
#include "buttons.h"
#include "display.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define DISPLAY_TEXT_TIME 3
#define COMPUTER_BEGINNING_WAIT_TIME 3

//Display defines
#define TEXT_SIZE 2
#define CURSOR_X 10
#define CURSOR_Y (DISPLAY_HEIGHT / 2)

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
    comp_move_st,
    player_move_st,
    player_wait_st,
    wait_for_release_st,
    game_over_st,
    erase_board_st,
};
//Declare currentState var
static enum ticTacToeControl_st_t currentState;

// Initialize the tic-tac-toe controller state machine,
// providing the tick period, in seconds.
void ticTacToeControl_init(double period_s){
    buttons_init();

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

            // Initialize display driver, and fill scren with black
            display_init();
            
            display_fillScreen(DISPLAY_BLACK); // Blank the screen.

            // Configure display text settings
            display_setTextColor(DISPLAY_LIGHT_GRAY); // Make the text white.
            display_setTextSize(TEXT_SIZE);    // Make the text a little larger.

            // Set the cursor location and print to the LCD
            display_setCursor(CURSOR_X, CURSOR_Y);
            // display_println("Touch board to play X\n-or-\nwait for the computer and play 0.\n");
            
            break;
        case delay_txt_st:
            if (disp_text_cnt == disp_text_num_ticks){

                // Configure display text settings
                display_setTextColor(DISPLAY_BLACK); // Make the text black (erase the text).
                display_setTextSize(TEXT_SIZE);    // Make the text a little larger.

                // Set the cursor location and print to the LCD
                display_setCursor(CURSOR_X, CURSOR_Y);
                display_println("Touch board to play X\n-or-\nwait for the computer and play 0.\n");
                
                //erase the txt
                
                //display the lines
                ticTacToeDisplay_init();
                currentState = comp_beg_wait_st;
            }else{
                currentState = delay_txt_st;
            }
            break;
        case comp_beg_wait_st:
            if (comp_beg_wait_cnt = comp_beg_wait_num_ticks){
                currentState = comp_move_st;
            }else if(touchscreen_get_status() == TOUCHSCREEN_PRESSED){
                //Send to a state that waits for the player to let go of the
                //screen
                // is_Xs_turn = false;//We are going to have player be X
                currentState = wait_for_release_st;
            }else{
                currentState = comp_beg_wait_st;
            }
            break;
        case comp_move_st:
            printf("Get location\n");
            //Do computer's turn
            tictactoe_location_t move = minimax_computeNextMove(&board, is_Xs_turn);
            
            ticTacToeDisplay_drawX(move, false);
            //Update the record of the board we have to refelct the 
            //visual changes that we made.
            board.squares[move.row][move.column] = MINIMAX_X_SQUARE;

            //Check if gameOver()
            if (minimax_isGameOver(minimax_computeBoardScore(&board, is_Xs_turn))){
                currentState = game_over_st;
            }else{
                currentState = player_wait_st;
            }
            break;
        //Listen for a BTN0 press before erasing the board and
        //starting over again.
        case game_over_st:
            if(buttons_read() & BUTTONS_BTN0_MASK){
                //button was pressed. Erase board
                currentState = erase_board_st;
            }else{
                //remain in state
                currentState = game_over_st;
            }
            break;

        //Wait here until the player makes a valid move
        case player_wait_st:
            if(touchscreen_get_status() == TOUCHSCREEN_PRESSED){
                //Send to a state that waits for the player to let go of the
                //screen
                currentState = wait_for_release_st;
            }else{
                currentState = player_wait_st;
            }
            break;
        //Waits for the player to release the button, then verifies if
        //The location is valid and performs player's move
        case wait_for_release_st:
            if (touchscreen_get_status() == TOUCHSCREEN_RELEASED){
                currentState = player_move_st;
            }else{
                currentState = wait_for_release_st;
            }
            break;

        case player_move_st:
            //Verify valid location and perform player move.
            printf("Get location\n");
            tictactoe_location_t moveLocation = ticTacToeDisplay_getLocationFromPoint(touchscreen_get_location());

            if (board.squares[moveLocation.row][moveLocation.column] == MINIMAX_EMPTY_SQUARE){
                //Valid location. Put a O there
                //Do player's turn
                ticTacToeDisplay_drawO(moveLocation, false);
                //Update the record of the board we have to refelct the 
                //visual changes that we made.
                board.squares[moveLocation.row][moveLocation.column] = MINIMAX_X_SQUARE;
                
                //Acknowledge the touch so it can be used again
                touchscreen_ack_touch();
                if (minimax_isGameOver(minimax_computeBoardScore(&board, !is_Xs_turn))){
                    currentState = game_over_st;
                }else{
                    currentState = comp_move_st;
                }
            }else{
                //We'll need to wait for another touch because move is invalid
                touchscreen_ack_touch();
                currentState = player_wait_st;
            
            }
            break;

        //Remove all O's and X's from display and board array
        case erase_board_st:
            //Move to function later
            for (uint8_t row = 0; row < TICTACTOE_BOARD_ROWS; row++){
                for (uint8_t col = 0; col < TICTACTOE_BOARD_COLUMNS; col++){
                    tictactoe_location_t location;
                    location.row = row;
                    location.column = col;
                    
                    //Erase the stuff that's there.
                    if (board.squares[row][col] == MINIMAX_O_SQUARE){
                        ticTacToeDisplay_drawO(location, true);
                        board.squares[location.row][location.column] = MINIMAX_EMPTY_SQUARE;
                    } else if (board.squares[row][col] == MINIMAX_X_SQUARE){
                        ticTacToeDisplay_drawX(location, true);
                        board.squares[location.row][location.column] = MINIMAX_EMPTY_SQUARE;
                    }
                }
            }
            currentState = comp_beg_wait_cnt;
            break;
        
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
            break;
        case comp_move_st:
            break;
        default:
            printf("ERROR: Unaccounted state action.\n");
    }

}


