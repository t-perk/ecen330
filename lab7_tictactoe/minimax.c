#include "minimax.h"
#include "ticTacToe.h"
#include <stdio.h>

//Question. Is there any benefit to defining these variables in my init 
//function vs. right here?

uint8_t currRow = 0;
uint8_t currColumn = 0;

#define NUM_POSSIBLE_MOVES 8 // 9-1

struct MoveInfo {
    tictactoe_location_t moveLocation;
    minimax_score_t scoreInformation;
}

struct MoveInfo moveScoreTable[NUM_POSSIBLE_MOVES];

//Define a multidimensional array with movelocation, score pairs

minimax_score_t minimax(tictactoe_board_t *board, bool is_Xs_turn){
    if(minimax_isGameOver){
        //The & is passing the address of the board.
        return minimax_computeBoardScore(board, !is_Xs_turn); //Might need to use a &board
    }

    //Loop through each row and then each column
    for (currRow = 0; currRow < TICTACTOE_BOARD_ROWS; currRow++){
        for (currColumn = 0; currColumn < TICTACTOE_BOARD_COLUMNS; currColumn++){
            if(board[currRow][currColumn] == MINIMAX_EMPTY_SQUARE){

                // Simulate playing at this location
                board[currRow][currColumn] = is_Xs_turn ? MINIMAX_X_SQUARE : MINIMAX_O_SQUARE;

                // Recursively call minimax to get the best score, assuming player
                // choses to play at this location.
                minimax_score_t score = minimax(board, !is_Xs_turn);

                // add score to move - score table;
                // add move to move - score table;

                // tictactoe_location_t moveLocation;
                // moveLocation.column = currColumn;
                // moveLocation.row = currRow;
                moveScoreTable[currColumn + currRow].moveLocation.column = currColumn;
                moveScoreTable[currColumn + currRow].moveLocation.row = currRow;
                moveScoreTable[currColumn + currRow].scoreInformation = score;

                // Undo the change to the board
                board[currRow][currColumn] = MINIMAX_EMPTY_SQUARE;
            }
        }
    }

    //One idea is to create a choice array and a score array separate from one another. Then fill it as needed
    tictactoe_location_t choice = moveScoreTable[0].moveLocation;

    //Grab first value for comparison
    minimax_score_t chosenScore = moveScoreTable[0].scoreInformation;


    if(is_Xs_turn){
        for (uint8_t i = 0; i < NUM_POSSIBLE_MOVES; i++){
            //Get the highest score with it's associated move
            if (moveScoreTable[i] > chosenScore){
                chosenScore = moveScoreTable[i];
                choice = moveScoreTable[i];
            }
        }
    }
    else
    {
        for (uint8_t i = 0; i < NUM_POSSIBLE_MOVES; i++){
            //Get the highest score with it's associated move
            if (moveScoreTable[i] < chosenScore){
                chosenScore = moveScoreTable[i];
                choice = moveScoreTable[i];
            }
        }
    }

    return chosenScore;
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
                                             bool is_Xs_turn){
      
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
                                          bool is_Xs_turn){


    minimax_score_t computedScore = MINIMAX_NOT_ENDGAME;

    //Check if rows are the same
    for (currRow = 0; currRow < TICTACTOE_BOARD_ROWS; currRow++){
        bool isComplete = true;

        for (currColumn = 0; currColumn < TICTACTOE_BOARD_COLUMNS; currColumn++){
            if(board[currRow][currColumn] != MINIMAX_X_SQUARE){
                isComplete = false;
            }
        }

        if (isComplete){
            computedScore = MINIMAX_X_WINNING_SCORE;
        }
    }

}

// Init the board to all empty squares.
void minimax_initBoard(tictactoe_board_t *board){

    //Reset the MoveInfo struct
    
}

// Determine that the game is over by looking at the score.
bool minimax_isGameOver(minimax_score_t score){
    if (score == MINIMAX_DRAW_SCORE || score == MINIMAX_X_WINNING_SCORE || score == MINIMAX_O_WINNING_SCORE){
        return true;
    }else{
        return false;
    }
}
