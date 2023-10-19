#include "minimax.h"
#include "ticTacToe.h"
#include <stdio.h>

// Question. Is there any benefit to defining these variables in my init
// function vs. right here?

uint8_t currRow = 0;
uint8_t currColumn = 0;

#define NUM_POSSIBLE_MOVES 9 // 9-1

struct MoveInfo {
  tictactoe_location_t moveLocation;
  minimax_score_t scoreInformation;
};

struct MoveInfo moveScoreTable[NUM_POSSIBLE_MOVES];

// Define a multidimensional array with movelocation, score pairs

minimax_score_t minimax(tictactoe_board_t *board, bool is_Xs_turn,
                        uint8_t depth);
uint8_t getSquareIndex(uint8_t currRow, uint8_t currColumn);

// Using the currRow an currColumn, getSquareIndex returns a unique int
// corresponding to the the referenced square 0-8.
uint8_t getSquareIndex(uint8_t currRow, uint8_t currColumn) {
  return ((currRow * 3) + (currColumn));
}

minimax_score_t minimax(tictactoe_board_t *board, bool is_Xs_turn,
                        uint8_t depth) {
  printf("In minimax with Xs turn being %d and depth is %d\n", is_Xs_turn,
         depth);

  // minimax_score_t checkIsGameOver =
  // minimax_isGameOver(minimax_computeBoardScore(board, is_Xs_turn));
  // printf("checkIsGameOver: %d\n", checkIsGameOver);

  uint8_t myScore = minimax_computeBoardScore(board, !is_Xs_turn);

  if (minimax_isGameOver(myScore)) {
    // The & is passing the address of the board.
    printf("game over condition met\n");
    return myScore; // Might need to use a &board
  } else {
    printf("Game is not over. Continuing on\n\n");
  }

  // Loop through each row and then each column
  for (currRow = 0; currRow < TICTACTOE_BOARD_ROWS; currRow++) {
    for (currColumn = 0; currColumn < TICTACTOE_BOARD_COLUMNS; currColumn++) {
      if ((*board).squares[currRow][currColumn] == MINIMAX_EMPTY_SQUARE) {
        printf("Empty square found at: (%d,%d) \n", currRow, currColumn);

        // Simulate playing at this location
        (*board).squares[currRow][currColumn] =
            is_Xs_turn ? MINIMAX_X_SQUARE : MINIMAX_O_SQUARE;

        // Recursively call minimax to get the best score, assuming player
        // choses to play at this location.
        minimax_score_t score = minimax(board, !is_Xs_turn, ++depth);

        // add score to move - score table;
        // add move to move - score table;

        // tictactoe_location_t moveLocation;
        // moveLocation.column = currColumn;
        // moveLocation.row = currRow;

        // Get square index
        uint8_t currIndex = getSquareIndex(currRow, currColumn);

        moveScoreTable[currIndex].moveLocation.column = currColumn;
        moveScoreTable[currIndex].moveLocation.row = currRow;
        moveScoreTable[currIndex].scoreInformation = score;

        // Undo the change to the board
        (*board).squares[currRow][currColumn] = MINIMAX_EMPTY_SQUARE;
      }
    }
  }

  tictactoe_location_t choice = moveScoreTable[0].moveLocation;
  // Grab first value for comparison
  minimax_score_t chosenScore = moveScoreTable[0].scoreInformation;

  if (is_Xs_turn) {
    for (uint8_t i = 0; i < NUM_POSSIBLE_MOVES; i++) {
      // Get the highest score with it's associated move
      if (moveScoreTable[i].scoreInformation > chosenScore) {
        chosenScore = moveScoreTable[i].scoreInformation;
        choice = moveScoreTable[i].moveLocation;
      }
    }
  } else {
    for (uint8_t i = 0; i < NUM_POSSIBLE_MOVES; i++) {
      // Get the highest score with it's associated move
      if (moveScoreTable[i].scoreInformation < chosenScore) {
        chosenScore = moveScoreTable[i].scoreInformation;
        choice = moveScoreTable[i].moveLocation;
      }
    }
  }

  printf("Chosen row is going to be:%d, %d\n", choice.row, choice.column);

  return chosenScore;
}

void displayMoveScoreTable(){
  for (uint8_t i = 0; i < NUM_POSSIBLE_MOVES; i++) {
    
    printf("Entry: %d with move of: (%d,%d) and score of: %d.\n\n\n", i, moveScoreTable[i].moveLocation.row, moveScoreTable[i].moveLocation.column, moveScoreTable[i].scoreInformation);
  }
}

// This routine is not recursive but will invoke the recursive minimax function.
// You will call this function from the controlling state machine that you will
// implement in a later milestone. It computes the row and column of the next
// move based upon: the current board and player.
//
// When called from the controlling state machine, you will call this function
// as follows:
// 1. If the computer is playing as X, you will call this function with
// is_Xs_turn = true.
// 2. If the computer is playing as O, you will call this function with
// is_Xs_turn = false.
// This function directly passes the  is_Xs_turn argument into the minimax()
// (helper) function.
tictactoe_location_t minimax_computeNextMove(tictactoe_board_t *board,
                                             bool is_Xs_turn) {
  printf("\n\nIn minimax_computeNextMove\n");
  //Clear out the moveScore table.
  minimax_initBoard(board);

  uint8_t depth = 0;
  minimax(board, is_Xs_turn, depth);

  int8_t scoreToLookFor = is_Xs_turn ? MINIMAX_X_WINNING_SCORE : MINIMAX_O_WINNING_SCORE;
  displayMoveScoreTable();

  //Search through the moveScoreTable and get the move corresponding to the max/min score to turn
  for (uint8_t i = 0; i < NUM_POSSIBLE_MOVES; i++) {
    
    printf("Move number: %d looking for score %d\n", i, scoreToLookFor);

    if (moveScoreTable[i].scoreInformation == scoreToLookFor){
        printf("Matching score found, returning it's location!\n");
        return moveScoreTable[i].moveLocation;
    }
  }
}

void displayBoard(tictactoe_board_t *board) {
  printf("Board details:\n");
  printf("%d|%d|%d\n", (*board).squares[0][0], (*board).squares[0][1],
         (*board).squares[0][2]);
  printf("-+-+-\n");
  printf("%d|%d|%d\n", (*board).squares[1][0], (*board).squares[1][1],
         (*board).squares[1][2]);
  printf("-+-+-\n");
  printf("%d|%d|%d\n", (*board).squares[2][0], (*board).squares[2][1],
         (*board).squares[2][2]);
  printf("\n");
}

// Returns the score of the board.
// This returns one of 4 values: MINIMAX_X_WINNING_SCORE,
// MINIMAX_O_WINNING_SCORE, MINIMAX_DRAW_SCORE, MINIMAX_NOT_ENDGAME
// Note: the is_Xs_turn argument indicates which player just took their
// turn and makes it possible to speed up this function.
// Assumptions:
// (1) if is_Xs_turn == true, the last thing played was an 'X'.
// (2) if is_Xs_turn == false, the last thing played was an 'O'.
// Hint: If you know the game was not over when an 'X' was played,
// you don't need to look for 'O's, and vice-versa.
minimax_score_t minimax_computeBoardScore(tictactoe_board_t *board,
                                          bool is_Xs_turn) {

  printf("in minimax_computerBoardScore\n");
  displayBoard(board);

  tictactoe_square_state_t charToLookFor =
      is_Xs_turn ? MINIMAX_X_SQUARE : MINIMAX_O_SQUARE;

  printf("charToLookFor is: %d\n", charToLookFor);

  minimax_score_t computedScore = MINIMAX_NOT_ENDGAME;

  bool isRowComplete = false;
  bool isColumnComplete = false;
  bool isDiagonal = false;
  bool isBoardFilled = true;

  // Right now my code is structured so that any row that is not complete causes
  // the isRowComplete to be false. Same is true for column completeness

  // Look in every row
  for (uint8_t row = 0; row < TICTACTOE_BOARD_ROWS; row++) {
    if ((*board).squares[row][0] == charToLookFor &&
        (*board).squares[row][1] == charToLookFor &&
        (*board).squares[row][2] == charToLookFor) {

      isRowComplete = true;
      break;
    }
  }

  // Look in every column
  for (uint8_t column = 0; column < TICTACTOE_BOARD_COLUMNS; column++) {
    if ((*board).squares[0][column] == charToLookFor &&
        (*board).squares[1][column] == charToLookFor &&
        (*board).squares[2][column] == charToLookFor) {

      isColumnComplete = true;
      break;
    }
  }

  // Check if the board is filled
  for (uint8_t column = 0; column < TICTACTOE_BOARD_COLUMNS; column++) {
    for (uint8_t row = 0; row < TICTACTOE_BOARD_ROWS; row++) {
      // Also check if the board is filled
      if ((*board).squares[row][column] == MINIMAX_EMPTY_SQUARE) {
        isBoardFilled = false;
        break;
      }
    }
  }

  // Check if top left to bottom right diagonal exist
  if ((*board).squares[0][0] == charToLookFor &&
      (*board).squares[1][1] == charToLookFor &&
      (*board).squares[2][2] == charToLookFor) {
    isDiagonal = true;
  } else if ((*board).squares[0][2] == charToLookFor &&
             (*board).squares[1][1] == charToLookFor &&
             (*board).squares[2][0] == charToLookFor) {
    isDiagonal = true;
    printf("HERE Status of squares: %d, %d, %d\n", (*board).squares[0][2],
           (*board).squares[1][1], (*board).squares[2][0]);
  }

  printf("Status of board: \n");
  printf("isRowComplete: %d\n", isRowComplete);
  printf("isColumn: %d\n", isColumnComplete);
  printf("isDiagonal: %d\n", isDiagonal);
  printf("isBoardFilled: %d\n", isBoardFilled);
  printf("\n");

  // check is there has been three in a row, column, or diagonal.

  if (isRowComplete || isColumnComplete || isDiagonal) {
    switch (charToLookFor) {
    case MINIMAX_X_SQUARE:
      return (MINIMAX_X_WINNING_SCORE);
    case MINIMAX_O_SQUARE:
      return (MINIMAX_O_WINNING_SCORE);
    default:
      printf("Error: default case entered with charToLookFor val: %d\n",
             charToLookFor);
      break;
    }
  }
  // Not in win state, but isFilled
  else if (isBoardFilled) {
    printf("board is filled with no win condition. Returning "
           "MINIMAX_DRAW_SCORE\n");
    return MINIMAX_DRAW_SCORE;
  } else if (!isBoardFilled) {
    printf("board is not filled with no win condition. Returning "
           "MINIMAX_NOT_ENDGAME\n");
    return MINIMAX_NOT_ENDGAME;
  } else {
    printf("ERROR: Unaccounted option.\n");
  }

  // TODO Account for when there is a DRAW or NOT ENDGAME
  /*
  DRAW Case
  - There are available spots
  - X does not have win condition
  - O does not have win condition
  NOT END GAME Case
  - When there are available spots
  - X does not have win condition
  - O does not have win condition

  When is_Xs_turn == true, we only need to check Xs because it's impossible
  for O to be in a win state during X's turn.
  */

  // So I really only need to check if all of the spots are filled or not.
}

// Init the board to all empty squares.
void minimax_initBoard(tictactoe_board_t *board) {

  // Reset the MoveInfo struct
  for (uint8_t i = 0; i < NUM_POSSIBLE_MOVES; i++) {
    // Get the highest score with it's associated move

    moveScoreTable[i].moveLocation.column = 0;
    moveScoreTable[i].moveLocation.row = 0;
    moveScoreTable[i].scoreInformation = 0;
  }
}

// Determine that the game is over by looking at the score.
bool minimax_isGameOver(minimax_score_t score) {
  printf("In isGameOver with a score of: %d\n", score);

  bool returnValue;
  if (score == MINIMAX_DRAW_SCORE || score == MINIMAX_X_WINNING_SCORE ||
      score == MINIMAX_O_WINNING_SCORE) {
    returnValue = true;
  } else if (score == MINIMAX_NOT_ENDGAME) {
    returnValue = false;
  } else {
    printf("ERROR: Unaccounted for case in isGameOver!\n");
  }

  printf("Returning %d from isGameOver\n", returnValue);
  return returnValue;
}