/*
This software is provided for student assignment use in the Department of
Electrical and Computer Engineering, Brigham Young University, Utah, USA.

Users agree to not re-host, or redistribute the software, in source or binary
form, to other persons or other institutions. Users may modify and use the
source code for personal or educational use.

For questions, contact Brad Hutchings or Jeff Goeders, https://ece.byu.edu/
*/
#include "ticTacToeDisplay.h"

#include "display.h"

#define PADY 5
#define XO_WIDTH (DISPLAY_HEIGHT / 3 - 2 * PADY)
#define PADX ((DISPLAY_WIDTH - 3 * XO_WIDTH) / 6)

#define COLOR_GRID DISPLAY_WHITE
#define COLOR_X DISPLAY_RED
#define COLOR_O DISPLAY_WHITE

#define BACKGROUND_COLOR DISPLAY_DARK_BLUE

#define NUM_COLS 3
#define DIV_MID 2

#define TOP_LINE_Y (DISPLAY_HEIGHT / 3)
#define BOT_LINE_Y (DISPLAY_HEIGHT * 2 / 3)
#define LEFT_LINE_X (DISPLAY_WIDTH / 3)
#define RIGHT_LINE_X (DISPLAY_WIDTH * 2 / 3)

#define LINE_START_X 0
#define LINE_START_Y 0

///////////////////////////////////////////////////////////////////////////////
///////////////////////////// Globals /////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
///////////////////////////// Local Functions /////////////////////////////////
///////////////////////////////////////////////////////////////////////////////

///////////////////////////////////////////////////////////////////////////////
///////////////////////////// Fuction Definitions /////////////////////////////
///////////////////////////////////////////////////////////////////////////////

// Inits the tic-tac-toe display, draws the lines that form the board.
void ticTacToeDisplay_init() {
  display_drawFastHLine(LINE_START_X, TOP_LINE_Y, DISPLAY_WIDTH, COLOR_GRID);
  display_drawFastHLine(LINE_START_X, BOT_LINE_Y, DISPLAY_WIDTH, COLOR_GRID);
  display_drawFastVLine(LEFT_LINE_X, LINE_START_Y, DISPLAY_HEIGHT, COLOR_GRID);
  display_drawFastVLine(RIGHT_LINE_X, LINE_START_Y, DISPLAY_HEIGHT, COLOR_GRID);
}

// Draws an X at the specified location
// erase == true means to erase the X by redrawing it as background.
void ticTacToeDisplay_drawX(tictactoe_location_t location, bool erase) {
  uint16_t x1 = DISPLAY_WIDTH * location.column / NUM_COLS + PADX;
  uint16_t x2 = x1 + XO_WIDTH;
  uint16_t y1 = DISPLAY_HEIGHT * location.row / NUM_COLS + PADY;
  uint16_t y2 = y1 + XO_WIDTH;

  display_drawLine(x1, y1, x2, y2, erase ? BACKGROUND_COLOR : COLOR_X);
  display_drawLine(x1, y2, x2, y1, erase ? BACKGROUND_COLOR : COLOR_X);
}

// Draws an O at the specified row and column.
// erase == true means to erase the X by redrawing it as background.
void ticTacToeDisplay_drawO(tictactoe_location_t location, bool erase) {
  uint16_t x =
      DISPLAY_WIDTH * location.column / NUM_COLS + PADX + XO_WIDTH / DIV_MID;
  uint16_t y =
      DISPLAY_HEIGHT * location.row / NUM_COLS + PADY + XO_WIDTH / DIV_MID;

  display_drawCircle(x, y, XO_WIDTH / DIV_MID,
                     erase ? BACKGROUND_COLOR : COLOR_O);
}

#define TOP 0
#define MID 1
#define BOTTOM 2
#define LEFT 0
#define RIGHT 2

// For a given touch location on the touchscreen, this function returns the
// corresponding tictactoe board location.
tictactoe_location_t
ticTacToeDisplay_getLocationFromPoint(display_point_t point) {
  tictactoe_location_t location;

  int16_t x = point.x;
  int16_t y = point.y;

  uint16_t x1 = LEFT_LINE_X;
  uint16_t x2 = RIGHT_LINE_X;
  uint16_t y1 = TOP_LINE_Y;
  uint16_t y2 = BOT_LINE_Y;

  if (x < x1)
    location.column = LEFT;
  else if (x < x2)
    location.column = MID;
  else
    location.column = RIGHT;

  if (y < y1)
    location.row = TOP;
  else if (y < y2)
    location.row = MID;
  else
    location.row = BOTTOM;

  return location;
}
