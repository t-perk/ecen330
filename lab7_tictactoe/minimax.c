#include "minimax.h"
#include "ticTacToe.h"
#include <stdio.h>

#define BOARD_SIZE (TICTACTOE_BOARD_ROWS * TICTACTOE_BOARD_COLUMNS)
#define POSITION_0 0
#define POSITION_1 1
#define POSITION_2 2

//Static meaning that it's global but only accessible within this file.
static tictactoe_location_t bestMove;
static uint32_t moveCounter;//Depth tracker

// Define a multidimensional array with movelocation, score pairs
minimax_score_t minimax(tictactoe_board_t *board, bool is_Xs_turn);

//Recursively calls itself to calculate the optimal move associated
//with a player's turn.
//Returns a score associated with a move
minimax_score_t minimax(tictactoe_board_t *board, bool is_Xs_turn) {
  
  uint8_t moveIndex = 0;

  //Create a score board for each move
  minimax_score_t moveScores[BOARD_SIZE];
  tictactoe_location_t moveLocations[BOARD_SIZE];

  minimax_score_t myScore = minimax_computeBoardScore(board, !is_Xs_turn);

  uint8_t currRow = 0;
  uint8_t currColumn = 0;

  if (minimax_isGameOver(myScore)) {
    if (myScore < -1){
      // The & is passing the address of the board.
      myScore -= BOARD_SIZE - moveCounter;
    }else if(myScore > 1){
       myScore += BOARD_SIZE - moveCounter;
    }
    
    return myScore;
  }

  // Loop through each row and then each column
  for (currRow = 0; currRow < TICTACTOE_BOARD_ROWS; currRow++) {
    for (currColumn = 0; currColumn < TICTACTOE_BOARD_COLUMNS; currColumn++) {
      if ((*board).squares[currRow][currColumn] == MINIMAX_EMPTY_SQUARE) {

        // Simulate playing at this location
        (*board).squares[currRow][currColumn] =
            is_Xs_turn ? MINIMAX_X_SQUARE : MINIMAX_O_SQUARE;

        // Recursively call minimax to get the best score, assuming player
        // choses to play at this location.
        moveCounter++;
        myScore = minimax(board, !is_Xs_turn);
        moveCounter--;

        moveLocations[moveIndex].column = currColumn;
        moveLocations[moveIndex].row = currRow;
        moveScores[moveIndex] = myScore;
        moveIndex++;

        // Undo the change to the board
        (*board).squares[currRow][currColumn] = MINIMAX_EMPTY_SQUARE;
      }
    }
  }

  //This is the "potential" bestScore that we use to compare against
  minimax_score_t bestScore = moveScores[0];
  bestMove = moveLocations[0];

  if (is_Xs_turn) {
    for (uint8_t i = 1; i < moveIndex; i++) {
      // Get the highest score with it's associated move
      if (moveScores[i] > bestScore) {
        bestScore = moveScores[i];
        bestMove = moveLocations[i];
      }
    }
  } else if (!is_Xs_turn){
    for (uint8_t i = 1; i < moveIndex; i++) {
      // Get the highest score with it's associated move
      if (moveScores[i] < bestScore) {
        bestScore = moveScores[i];
        bestMove = moveLocations[i];
      }
    }
  }

  return bestScore;
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
  //Reset the overall move counter. we track the scores for every move.
  moveCounter = 0;
  minimax(board, is_Xs_turn);

  return bestMove;
    
}

//Prints out a visual representation of the board at each step of the process
void displayBoard(tictactoe_board_t *board) {
  printf("Board details:\n");
  printf("%d|%d|%d\n", (*board).squares[POSITION_0][POSITION_0], (*board).squares[POSITION_0][POSITION_1],
         (*board).squares[POSITION_0][POSITION_2]);
  printf("-+-+-\n");
  printf("%d|%d|%d\n", (*board).squares[POSITION_1][POSITION_0], (*board).squares[POSITION_1][POSITION_1],
         (*board).squares[POSITION_1][POSITION_2]);
  printf("-+-+-\n");
  printf("%d|%d|%d\n", (*board).squares[POSITION_2][POSITION_0], (*board).squares[POSITION_2][POSITION_1],
         (*board).squares[POSITION_2][POSITION_2]);
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
  // displayBoard(board);

  tictactoe_square_state_t charToLookFor =
      is_Xs_turn ? MINIMAX_X_SQUARE : MINIMAX_O_SQUARE;

  minimax_score_t computedScore = MINIMAX_NOT_ENDGAME;

  bool isRowComplete = false;
  bool isColumnComplete = false;
  bool isDiagonal = false;
  bool isBoardFilled = true;

  // Right now my code is structured so that any row that is not complete causes
  // the isRowComplete to be false. Same is true for column completeness

  // Look in every row
  for (uint8_t row = 0; row < TICTACTOE_BOARD_ROWS; row++) {
    if ((*board).squares[row][POSITION_0] == charToLookFor &&
        (*board).squares[row][POSITION_1] == charToLookFor &&
        (*board).squares[row][POSITION_2] == charToLookFor) {

      isRowComplete = true;
      break;
    }
  }

  // Look in every column
  for (uint8_t column = 0; column < TICTACTOE_BOARD_COLUMNS; column++) {
    if ((*board).squares[POSITION_0][column] == charToLookFor &&
        (*board).squares[POSITION_1][column] == charToLookFor &&
        (*board).squares[POSITION_2][column] == charToLookFor) {

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
  if ((*board).squares[POSITION_0][POSITION_0] == charToLookFor &&
      (*board).squares[POSITION_1][POSITION_1] == charToLookFor &&
      (*board).squares[POSITION_2][POSITION_2] == charToLookFor) {
    isDiagonal = true;
  } else if ((*board).squares[POSITION_0][POSITION_2] == charToLookFor &&
             (*board).squares[POSITION_1][POSITION_1] == charToLookFor &&
             (*board).squares[POSITION_2][POSITION_0] == charToLookFor) {
    isDiagonal = true;
  }

  // check is there has been three in a row, column, or diagonal.

  if (isRowComplete || isColumnComplete || isDiagonal) {
    switch (charToLookFor) {
    case MINIMAX_X_SQUARE:
      return (MINIMAX_X_WINNING_SCORE);
    case MINIMAX_O_SQUARE:
      return (MINIMAX_O_WINNING_SCORE);
    default:
      break;
    }
  }
  // Not in win state, but isFilled
  else if (isBoardFilled) {
    return MINIMAX_DRAW_SCORE;
  } else if (!isBoardFilled) {
    return MINIMAX_NOT_ENDGAME;
  } else {
    printf("ERROR: Unaccounted option.\n");
  }
  // So I really only need to check if all of the spots are filled or not.
}

// Determine that the game is over by looking at the score.
bool minimax_isGameOver(minimax_score_t score) {

  bool returnValue;
  if (score == MINIMAX_DRAW_SCORE || score == MINIMAX_X_WINNING_SCORE ||
      score == MINIMAX_O_WINNING_SCORE) {
    returnValue = true;
  } else if (score == MINIMAX_NOT_ENDGAME) {
    returnValue = false;
  } else {
    printf("ERROR: Unaccounted for case in isGameOver!\n");
  }

  return returnValue;
}

// Init the board to all empty squares.
void minimax_initBoard(tictactoe_board_t *board) {

  for (int row = 0; row < TICTACTOE_BOARD_ROWS; row++) {
        for (int col = 0; col < TICTACTOE_BOARD_COLUMNS; col++) {
            (*board).squares[row][col] = MINIMAX_EMPTY_SQUARE;
        }
    }
}