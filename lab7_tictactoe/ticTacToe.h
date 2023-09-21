#ifndef TICTACTOE
#define TICTACTOE

#include <stdint.h>

// Defines the boundaries of the tic-tac-toe board.
#define TICTACTOE_BOARD_ROWS 3
#define TICTACTOE_BOARD_COLUMNS 3

// These are the values in the board to represent who is occupying what square.
typedef enum {
  MINIMAX_EMPTY_SQUARE,
  MINIMAX_O_SQUARE,
  MINIMAX_X_SQUARE,
} tictactoe_square_state_t;

// Boards contain just an array of squares. I used a struct to provide
// additional abstraction in case I wanted to add something to the board type.
typedef struct {
  tictactoe_square_state_t squares[TICTACTOE_BOARD_ROWS]
                                  [TICTACTOE_BOARD_COLUMNS];
} tictactoe_board_t;

// A board location, which is a row-column pair.
typedef struct {
  uint8_t row;
  uint8_t column;
} tictactoe_location_t;

#endif /* TICTACTOE */
