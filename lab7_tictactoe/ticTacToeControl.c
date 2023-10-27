#include "ticTacToeControl.h"
#include "buttons.h"
#include "display.h"
#include "minimax.h"
#include "ticTacToe.h"
#include "ticTacToeDisplay.h"
#include "touchscreen.h"
#include "xil_io.h"
#include "xparameters.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define DISPLAY_TEXT_TIME 3
#define COMPUTER_BEGINNING_WAIT_TIME 3

// Display defines
#define TEXT_SIZE 2
#define CURSOR_X 10
#define CURSOR_Y (DISPLAY_HEIGHT / 2)

// Values that should be reset at the end of each game
static tictactoe_board_t board;
static bool is_Xs_turn = true;
static bool player_is_X = false;
static bool board_isEmpty = true;

static tictactoe_location_t default_comp_move;
static tictactoe_location_t move;

// State messages
#define INIT_ST_MSG "ticTacToe_init_st\n"
#define DELAY_TXT_MSG "ticTacToe_delay_txt_st\n"
#define COMP_BEG_WAIT_MSG "ticTacToe_comp_beg_wait_st\n"
#define COMP_MOVE_MSG "ticTacToe_comp_move_st\n"
#define PLAYER_MOVE_MSG "ticTacToe_player_move_st\n"
#define PLAYER_WAIT_MSG "ticTacToe_player_wait_st\n"
#define WAIT_FOR_RELEASE_MSG "ticTacToe_wait_for_release_st\n"
#define GAME_OVER_MSG "ticTacToe_game_over_st\n"
#define ERASE_BOARD_MSG "ticTacToe_erase_board_st\n"

uint32_t disp_text_cnt = 0;
uint32_t disp_text_num_ticks = 0;

uint32_t comp_beg_wait_cnt = 0;
uint32_t comp_beg_wait_num_ticks = 0;

enum ticTacToeControl_st_t {
  init_st,          // Starts here. Init buttons and clear screen
  delay_txt_st,     // Necessary?
  comp_beg_wait_st, // Wait for player to play or begin computer
  comp_move_st,
  player_move_st,      // Could be mealy?
  player_wait_st,      // Could be mealy?
  wait_for_release_st, // Could be mealy?
  game_over_st,
  erase_board_st,
};
// Declare currentState var
static enum ticTacToeControl_st_t currentState;

// Initialize the tic-tac-toe controller state machine,
// providing the tick period, in seconds.
void ticTacToeControl_init(double period_s) {
  buttons_init();
  display_init();
  display_fillScreen(DISPLAY_DARK_BLUE); // Blank the screen.

  // Reset player_is_x to be false
  player_is_X = false;

  // Set current state
  currentState = init_st;

  // Initialize values
  disp_text_cnt = 0;
  comp_beg_wait_cnt = 0;

  // Setup number of ticks
  disp_text_num_ticks = DISPLAY_TEXT_TIME / period_s;
  comp_beg_wait_num_ticks = COMPUTER_BEGINNING_WAIT_TIME / period_s;

  default_comp_move.row = 0;
  default_comp_move.column = 0;

  move.row = 0;
  move.column = 0;
}

// This is a debug state print routine. It will print the names of the states
// each time tick() is called. It only prints states if they are different than
// the previous state.
void debugStatePrint_ticTacToeControl() {
  static enum ticTacToeControl_st_t previousState;
  static bool firstPass = true;
  // Only print the message if:
  // 1. This the first pass and the value for previousState is unknown.
  // 2. previousState != currentState - this prevents reprinting the same state
  // name over and over.
  if (previousState != currentState || firstPass) {
    firstPass = false; // previousState will be defined, firstPass is false.
    previousState =
        currentState; // keep track of the last state that you were in.

    switch (currentState) {
    case init_st:
      printf(INIT_ST_MSG);
      break;
    case delay_txt_st:
      printf(DELAY_TXT_MSG);
      break;
    case comp_beg_wait_st:
      printf(COMP_BEG_WAIT_MSG);
      break;
    case comp_move_st:
      printf(COMP_MOVE_MSG);
      break;
    case player_move_st:
      printf(PLAYER_MOVE_MSG);
      break;
    case player_wait_st:
      printf(PLAYER_WAIT_MSG);
      break;
    case wait_for_release_st:
      printf(WAIT_FOR_RELEASE_MSG);
      break;
    case game_over_st:
      printf(GAME_OVER_MSG);
      break;
    case erase_board_st:
      printf(ERASE_BOARD_MSG);
      break;
    default:
      printf("ERROR: Unaccounted state action.\n");
    }
  }
}

// Tick the tic-tac-toe controller state machine
void ticTacToeControl_tick() {
  // Debug function
  //  debugStatePrint_ticTacToeControl();

  // Handle transitions
  switch (currentState) {
  // Display beginning text
  case init_st:

    // Configure display text settings
    display_setTextColor(DISPLAY_RED); // Make the text white.
    display_setTextSize(TEXT_SIZE);    // Make the text a little larger.

    // Set the cursor location and print to the LCD
    display_setCursor(CURSOR_X, CURSOR_Y);
    display_println(
        "Touch board to play X\n-or-\nwait for the computer and play 0.\n");

    currentState = delay_txt_st;
    break;
  // Start timer to delay text for a certain time
  case delay_txt_st:
    if (disp_text_cnt == disp_text_num_ticks) {

      // erase the txt
      //  Configure display text settings
      display_setTextColor(
          DISPLAY_DARK_BLUE);         // Make the text pop (erase the text).
      display_setTextSize(TEXT_SIZE); // Make the text a little larger.

      // Set the cursor location and print to the LCD
      display_setCursor(CURSOR_X, CURSOR_Y);
      display_println(
          "Touch board to play X\n-or-\nwait for the computer and play 0.\n");

      // display the lines
      ticTacToeDisplay_init();
      currentState = comp_beg_wait_st;
    } else {
      // printf("Waiting in delay_txt_st w/ disp_text_cnt of: %d\n",
      // disp_text_cnt);
      currentState = delay_txt_st;
    }
    break;
  // Timer to see if the player will do anything. If not, let the computer play
  case comp_beg_wait_st:
    if (comp_beg_wait_cnt == comp_beg_wait_num_ticks) {
      currentState = comp_move_st;
    } else if (touchscreen_get_status() == TOUCHSCREEN_PRESSED) {
      // Send to a state that waits for the player to let go of the
      // screen
      player_is_X = true;
      currentState = wait_for_release_st;
    } else {
      // printf("Waiting in comp_beg_wait_st w/ comp_beg_wait_cnt of: %d\n",
      // comp_beg_wait_cnt);
      currentState = comp_beg_wait_st;
    }
    break;
  // Performs computer move
  case comp_move_st:

    if (board_isEmpty) {
      board_isEmpty = false;
      move = default_comp_move;
    } else {
      move = minimax_computeNextMove(&board, is_Xs_turn);
    }

    if (player_is_X) {
      ticTacToeDisplay_drawO(move, false);
      // Update the record of the board we have to refelct the
      // visual changes that we made.
      board.squares[move.row][move.column] = MINIMAX_O_SQUARE;
    } else {
      ticTacToeDisplay_drawX(move, false);
      // Update the record of the board we have to refelct the
      // visual changes that we made.
      board.squares[move.row][move.column] = MINIMAX_X_SQUARE;
    }

    // Check if gameOver()
    if (minimax_isGameOver(minimax_computeBoardScore(&board, is_Xs_turn))) {
      currentState = game_over_st;
    } else {
      currentState = player_wait_st;
    }
    break;
  // Listen for a BTN0 press before erasing the board and
  // starting over again.
  case game_over_st:
    if (buttons_read() & BUTTONS_BTN0_MASK) {
      // button was pressed. Erase board
      currentState = erase_board_st;
    } else {
      // remain in state
      currentState = game_over_st;
    }
    break;

  // Wait here until the player makes a valid move
  case player_wait_st:
    if (touchscreen_get_status() == TOUCHSCREEN_PRESSED) {
      // Send to a state that waits for the player to let go of the
      // screen
      currentState = wait_for_release_st;
    } else {
      currentState = player_wait_st;
    }
    break;
  // Waits for the player to release the button, then verifies if
  // The location is valid and performs player's move
  case wait_for_release_st:
    if (touchscreen_get_status() == TOUCHSCREEN_RELEASED) {
      currentState = player_move_st;
    } else {
      currentState = wait_for_release_st;
    }
    break;

  // Perform player's move
  case player_move_st:
    // Verify valid location and perform player move.
    move = ticTacToeDisplay_getLocationFromPoint(touchscreen_get_location());

    // Board should always be non-empty when player has played
    board_isEmpty = false;

    if (board.squares[move.row][move.column] == MINIMAX_EMPTY_SQUARE) {

      if (player_is_X) {
        ticTacToeDisplay_drawX(move, false);
        // Update the record of the board we have to refelct the
        // visual changes that we made.
        board.squares[move.row][move.column] = MINIMAX_X_SQUARE;
      } else {
        ticTacToeDisplay_drawO(move, false);
        // Update the record of the board we have to refelct the
        // visual changes that we made.
        board.squares[move.row][move.column] = MINIMAX_O_SQUARE;
      }

      // Acknowledge the touch so it can be used again
      touchscreen_ack_touch();
      if (minimax_isGameOver(minimax_computeBoardScore(&board, !is_Xs_turn))) {
        currentState = game_over_st;
      } else {
        currentState = comp_move_st;
      }
    } else {
      // We'll need to wait for another touch because move is invalid
      touchscreen_ack_touch();
      currentState = player_wait_st;
    }
    break;

  // Remove all O's and X's from display and board array
  case erase_board_st:
    // Move to function later
    for (uint8_t row = 0; row < TICTACTOE_BOARD_ROWS; row++) {
      for (uint8_t col = 0; col < TICTACTOE_BOARD_COLUMNS; col++) {
        tictactoe_location_t location;
        location.row = row;
        location.column = col;

        // Erase the stuff that's there.
        if (board.squares[row][col] == MINIMAX_O_SQUARE) {
          ticTacToeDisplay_drawO(location, true);
          board.squares[location.row][location.column] = MINIMAX_EMPTY_SQUARE;
        } else if (board.squares[row][col] == MINIMAX_X_SQUARE) {
          ticTacToeDisplay_drawX(location, true);
          board.squares[location.row][location.column] = MINIMAX_EMPTY_SQUARE;
        }
      }
    }

    // Reset values
    player_is_X = false;
    disp_text_cnt = 0;
    comp_beg_wait_cnt = 0;
    board_isEmpty = true;

    currentState = comp_beg_wait_st;
    break;

  default:
    printf("ERROR: Unaccounted state transition.\n");
  }

  // Hanlde state actions
  switch (currentState) {
  case init_st:
    break;
  // Increment disp_text_cnt for beginning text display
  case delay_txt_st:
    disp_text_cnt++;
    break;
  // Increment comp_beg_wait_cnt for waiting to see if the player plays
  //  first
  case comp_beg_wait_st:
    comp_beg_wait_cnt++;
    break;
  case comp_move_st:
    break;
  case player_move_st:
    break;
  case player_wait_st:
    break;
  case wait_for_release_st:
    break;
  case game_over_st:
    break;
  case erase_board_st:
    break;
  default:
    printf("ERROR: Unaccounted state action.\n");
  }
}
