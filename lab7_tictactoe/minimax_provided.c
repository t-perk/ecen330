#include "minimax.h"
#include <stdio.h>

#define BOARD_SIZE (TICTACTOE_BOARD_ROWS * TICTACTOE_BOARD_COLUMNS)

static tictactoe_location_t bestMove;
static uint32_t moveCounter;//Depth tracker

void printBoard(tictactoe_board_t *board) {
    for (int row = 0; row < TICTACTOE_BOARD_ROWS; row++) {
        for (int col = 0; col < TICTACTOE_BOARD_COLUMNS; col++) {
            if (board->squares[row][col] == MINIMAX_X_SQUARE)
                printf("X");
            else if (board->squares[row][col] == MINIMAX_O_SQUARE)
                printf("O");
            else
                printf(" ");
        }
        printf("\n");
    }
}

static minimax_score_t minimax(tictactoe_board_t *board, bool isXsTurn) {
    uint8_t moveIndex = 0;
    minimax_score_t moveScores[BOARD_SIZE];
    tictactoe_location_t moveLocations[BOARD_SIZE];

    minimax_score_t boardScore = minimax_computeBoardScore(board, !isXsTurn);

    //Implementing board functionality on score that we return.
    if (minimax_isGameOver(boardScore)) {
        if (boardScore < -1)
        //This is the amount that we are going to subtract: 9 - depth/move
        //This makes sense because the more moves it takes, the closer to 0
        //It will be for this O player who is trying to minimize their score
            boardScore -= BOARD_SIZE - moveCounter;
        else if (boardScore > 1)
            boardScore += BOARD_SIZE - moveCounter;
        return boardScore;
    }

    for (int row = 0; row < TICTACTOE_BOARD_ROWS; row++) {
        for (int col = 0; col < TICTACTOE_BOARD_COLUMNS; col++) {
            if (board->squares[row][col] == MINIMAX_EMPTY_SQUARE) {
                if (isXsTurn)
                    board->squares[row][col] = MINIMAX_X_SQUARE;
                else
                    board->squares[row][col] = MINIMAX_O_SQUARE;

                //Move the moveCounter up so the next iteration uses the
                //Proper index
                moveCounter++;
                boardScore = minimax(board, !isXsTurn);
                //Move the moveCounter up so the current iteration uses the
                //proper index
                moveCounter--;

                //It's important that this take place after the recursive call
                //so that score and locations are only being updated on the way
                //up the rabbit hole and things don't get overwritten.
                moveScores[moveIndex] = boardScore;
                moveLocations[moveIndex].row = row;
                moveLocations[moveIndex].column = col;
                moveIndex++;

                board->squares[row][col] = MINIMAX_EMPTY_SQUARE;
            }
        }
    }

    minimax_score_t bestScore = moveScores[0];
    bestMove = moveLocations[0];

    for (int i = 1; i < moveIndex; i++) {
        if ((isXsTurn && moveScores[i] > bestScore) || (!isXsTurn && moveScores[i] < bestScore)) {
            bestScore = moveScores[i];
            bestMove = moveLocations[i];
        }
    }

    return bestScore;
}

tictactoe_location_t minimax_computeNextMove(tictactoe_board_t *board, bool isXsTurn) {
    moveCounter = 0;
    minimax(board, isXsTurn);
    return bestMove;
}

bool minimax_isGameOver(minimax_score_t score) {
    return score != MINIMAX_NOT_ENDGAME;
}

minimax_score_t minimax_computeBoardScore(tictactoe_board_t *board, bool isXsTurn) {
    tictactoe_square_state_t playerSquare = isXsTurn ? MINIMAX_X_SQUARE : MINIMAX_O_SQUARE;
    minimax_score_t winningScore = isXsTurn ? MINIMAX_X_WINNING_SCORE : MINIMAX_O_WINNING_SCORE;
    bool allSquaresFilled = true;

    for (int col = 0; col < TICTACTOE_BOARD_COLUMNS; col++) {
        bool colComplete = true;
        for (int row = 0; row < TICTACTOE_BOARD_ROWS; row++) {
            if (board->squares[row][col] == MINIMAX_EMPTY_SQUARE) {
                allSquaresFilled = false;
                colComplete = false;
                break;
            } else if (board->squares[row][col] != playerSquare) {
                colComplete = false;
            }
        }
        if (colComplete) {
            return winningScore;
        }
    }

    for (int row = 0; row < TICTACTOE_BOARD_ROWS; row++) {
        bool rowComplete = true;
        for (int col = 0; col < TICTACTOE_BOARD_COLUMNS; col++) {
            if (board->squares[row][col] != playerSquare) {
                rowComplete = false;
                break;
            }
        }
        if (rowComplete) {
            return winningScore;
        }
    }

    bool diagonalComplete = true;
    for (int i = 0; i < TICTACTOE_BOARD_ROWS; i++) {
        if (board->squares[i][i] != playerSquare) {
            diagonalComplete = false;
            break;
        }
    }
    if (diagonalComplete) {
        return winningScore;
    }

    diagonalComplete = true;
    for (int i = 0; i < TICTACTOE_BOARD_ROWS; i++) {
        if (board->squares[i][TICTACTOE_BOARD_ROWS - 1 - i] != playerSquare) {
            diagonalComplete = false;
            break;
        }
    }
    if (diagonalComplete) {
        return winningScore;
    }

    if (allSquaresFilled) {
        return MINIMAX_DRAW_SCORE;
    } else {
        return MINIMAX_NOT_ENDGAME;
    }
}

void minimax_initBoard(tictactoe_board_t *board) {
    for (int row = 0; row < TICTACTOE_BOARD_ROWS; row++) {
        for (int col = 0; col < TICTACTOE_BOARD_COLUMNS; col++) {
            board->squares[row][col] = MINIMAX_EMPTY_SQUARE;
        }
    }
}