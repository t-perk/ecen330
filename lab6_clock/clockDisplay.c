/*
This software is provided for student assignment use in the Department of
Electrical and Computer Engineering, Brigham Young University, Utah, USA.

Users agree to not re-host, or redistribute the software, in source or binary
form, to other persons or other institutions. Users may modify and use the
source code for personal or educational use.

For questions, contact Brad Hutchings or Jeff Goeders, https://ece.byu.edu/
*/
#include "clockDisplay.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "display.h"
#include "utils.h"

// Startup with this time.
#define STARTUP_SECONDS 59
#define STARTUP_MINUTES 59
#define STARTUP_HOURS 12

/*
 * The text size is 6 which multiplies the font size (6 x 8 including space
 * around char) to be 36 wide. A total of 8 characters will be written to the
 * screen, 8 x 36-wide = 288. This is the total width of the clock numbers. The
 * display is 320 wide, subtract 288 = 32, so space over 16 to center the number
 * horizontally. The display is 240 wide, with a text size of 6, the char height
 * is 48, to center vertically, subtract 1/2 of 48 from 120, should get you
 * close.
 */

// two characters for each field + separators.
#define CLOCK_TOTAL_CHAR_COUNT 8

// There are two characters per hour, minute, second field.
#define CLOCK_CHARS_PER_FIELD 2

// Compute vertical center for characters.
#define CLOCK_TEXT_VERTICAL_CENTER                                             \
  ((DISPLAY_HEIGHT / 2) - ((DISPLAY_CHAR_HEIGHT * CLOCKDISPLAY_TEXT_SIZE) / 2))

// Compute the total width of clock char display.
#define CLOCK_TEXT_TOTAL_WIDTH                                                 \
  (CLOCKDISPLAY_TEXT_SIZE * DISPLAY_CHAR_WIDTH * CLOCK_TOTAL_CHAR_COUNT)

// Move over far enough to center everything.
#define CLOCK_DISPLAY_ORIGIN_X ((DISPLAY_WIDTH - CLOCK_TEXT_TOTAL_WIDTH) / 2)
#define CLOCK_DISPLAY_ORIGIN_Y CLOCK_TEXT_VERTICAL_CENTER

#define CLOCK_BACKGROUND_COLOR DISPLAY_BLACK   // Black background.
#define CLOCK_FOREGROUND_COLOR DISPLAY_YELLOW  // Green display for time.
#define CLOCK_DISPLAY_BUTTON_COLOR DISPLAY_RED // Green buttons.

// The amount of gap (in pixels) between the triangle button and the clock text.
#define TRIANGLE_Y_GAP 20

// The height of the triangle looking from the top of the clock display.
#define TRIANGLE_HEIGHT 40

// Added to clearly show intent instead of just true/false.
#define FORCE_REDRAW true
#define NO_FORCE_REDRAW false
#define INCREMENT true  // Easily see if doing increment
#define DECREMENT false // Easily see if doing decrement.

#define CLOCKDISPLAY_TEXT_SIZE 6

#define MIDPOINT_DIVISOR 2

#define RIGHT_TRIANGLE_NUM_CHARS 6

// Define maximum/minimum values for hours, minutes, seconds.
// Used to make it easier to parameterize the math functions.
#define MAX_HOURS 12
#define MIN_HOURS 1
#define MAX_MINUTES 59
#define MIN_MINUTES 0
#define MAX_SECONDS 59
#define MIN_SECONDS 0

// Numbering scheme for the regions. See the figure below.
#define TOUCH_REGION_UP_HOURS 0
#define TOUCH_REGION_UP_MINUTES 1
#define TOUCH_REGION_UP_SECONDS 2
#define TOUCH_REGION_DOWN_HOURS 3
#define TOUCH_REGION_DOWN_MINUTES 4
#define TOUCH_REGION_DOWN_SECONDS 5
// The touch pad is divided into 6 regions so that you can increment/decrement
// the time by touching the screen.
/*
 *    |------------------------------------------------------------------------|
 *    |                    |                            |                      |
 *    |                    |                            |                      |
 *    |         0          |             1              |          2           |
 *    |                    |                            |                      |
 *    |                    |                            |                      |
 *    -------------------------------------------------------------------------|
 *    |                    |                            |                      |
 *    |                    |                            |                      |
 *    |         3          |             4              |          5           |
 *    |                    |                            |                      |
 *    |                    |                            |                      |
 *    |------------------------------------------------------------------------|
 */

// Origins of all of the triangles.
#define LEFT_TRIANGLE_BUTTON_X_ORIGIN CLOCK_DISPLAY_ORIGIN_X
#define MIDDLE_TRIANGLE_BUTTON_X_ORIGIN                                        \
  CLOCK_DISPLAY_ORIGIN_X + DISPLAY_CHAR_WIDTH *CLOCKDISPLAY_TEXT_SIZE * 3
#define RIGHT_TRIANGLE_BUTTON_X_ORIGIN                                         \
  CLOCK_DISPLAY_ORIGIN_X + DISPLAY_CHAR_WIDTH *CLOCKDISPLAY_TEXT_SIZE * 6

// Use these to determine what region the user is touching.
#define TOUCH_SCREEN_UPPER_HALF (DISPLAY_HEIGHT / 2)
#define TOUCH_SCREEN_LEFT_THIRD (DISPLAY_WIDTH / 3)
#define TOUCH_SCREEN_MIDDLE_THIRD (2 * (DISPLAY_WIDTH / 3))

///////////////////////////////////////////////////////////////////////////////
///////////////////////////// Globals /////////////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// These three globals contain the current time.
static uint8_t currentSeconds = STARTUP_SECONDS;
static uint8_t currentMinutes = STARTUP_MINUTES;
static uint8_t currentHours = STARTUP_HOURS;

///////////////////////////////////////////////////////////////////////////////
///////////////////////////// Local Functions /////////////////////////////////
///////////////////////////////////////////////////////////////////////////////
static void drawUpDownButtons(int16_t color);
static bool incDecValue(bool inc, uint8_t *value, uint8_t minValue,
                        uint8_t maxValue);
static void updateTimeDisplay(bool forceUpdateAll);
static uint8_t touchscreenComputeRegion(display_point_t touched_point);

///////////////////////////////////////////////////////////////////////////////
///////////////////////////// Fuction Definitions /////////////////////////////
///////////////////////////////////////////////////////////////////////////////
// Initialize clock display
void clockDisplay_init() {
  // Initialize display
  display_init();
  display_fillScreen(DISPLAY_BLACK);

  // Draw the buttons.
  drawUpDownButtons(CLOCK_DISPLAY_BUTTON_COLOR);

  // Force a draw of the clock display (true = force).
  updateTimeDisplay(FORCE_REDRAW);
}

// Performs the increment or decrement, given a touched point.
void clockDisplay_performIncDec(display_point_t touched_point) {
  uint8_t currentTouchedRegion = touchscreenComputeRegion(touched_point);

  switch (currentTouchedRegion) {
  case TOUCH_REGION_UP_HOURS:
    // touching up-hours, increment hours.
    incDecValue(INCREMENT, &currentHours, MIN_HOURS, MAX_HOURS);
    break;
  case TOUCH_REGION_UP_MINUTES:
    // touch up-minutes, increment minutes.
    incDecValue(INCREMENT, &currentMinutes, MIN_MINUTES, MAX_MINUTES);
    break;
  case TOUCH_REGION_UP_SECONDS:
    // touch up-seconds, increment seconds.
    incDecValue(INCREMENT, &currentSeconds, MIN_SECONDS, MAX_SECONDS);
    break;
  case TOUCH_REGION_DOWN_HOURS:
    // touch down-hours, decrement hours
    incDecValue(DECREMENT, &currentHours, MIN_HOURS, MAX_HOURS);
    break;
  case TOUCH_REGION_DOWN_MINUTES:
    // touch down-minutes, decrement minutes
    incDecValue(DECREMENT, &currentMinutes, MIN_MINUTES, MAX_MINUTES);
    break;
  case TOUCH_REGION_DOWN_SECONDS:
    // touch down-seconds, decrement seconds
    incDecValue(DECREMENT, &currentSeconds, MIN_SECONDS, MAX_SECONDS);
    break;
  default:
    printf("unknown touch region number in clockDisplay_performIncDecAction(): "
           "%d\n",
           currentTouchedRegion);
    break;
  }

  // Now, reflect the update on the display.
  updateTimeDisplay(NO_FORCE_REDRAW);
}

// Adds a second to the time, advancing the time and updating the display.
void clockDisplay_advanceTimeOneSecond() {
  // Increment seconds, looking for roll-over.
  if (incDecValue(INCREMENT, &currentSeconds, MIN_SECONDS, MAX_SECONDS))

    // Increment minutes (if roll-over), looking for roll-over.
    if (incDecValue(INCREMENT, &currentMinutes, MIN_SECONDS, MAX_MINUTES))

      // Increment hours, if roll-over.
      incDecValue(INCREMENT, &currentHours, MIN_HOURS, MAX_HOURS);

  // Now, reflect the update on the display.
  updateTimeDisplay(NO_FORCE_REDRAW);
}

///////////////////////////////////////////////////////////////////////////////
///////////////////////////// Helper Function Definitions /////////////////////
///////////////////////////////////////////////////////////////////////////////

// Updates the time display with latest time, making sure to update only those
// digits that have changed since the last update. if forceUpdateAll is true,
// update all digits.
static void updateTimeDisplay(bool forceUpdateAll) {
  // Keep this between invocations.
  static char currentClockDisplayString[CLOCK_TOTAL_CHAR_COUNT + 1];

  // This gets updated each time.
  char nextClockDisplayString[CLOCK_TOTAL_CHAR_COUNT + 1];

  // Convert the current hours, minutes, seconds into chars in the
  // nextClockDisplayString. Make sure to handle leading zeros or spaces
  // (hours). Hours are stored in indices 0-1. Separator at index 2. Minutes are
  // stored in indicies 3-4. Separator at index 5. Seconds are stored in indices
  // 6-7. Let sprintf do all of the formatting work. %2hd = printf 2 digits with
  // leading space for short. (hours) %02hd = printf 2 digits with leading 0 for
  // short. (minutes and seconds)
  sprintf(nextClockDisplayString, "%2hd:%02hd:%02hd", currentHours,
          currentMinutes, currentSeconds);

  // Loop through each character
  for (uint8_t i = 0; i < CLOCK_TOTAL_CHAR_COUNT; i++) {
    // Redraw any character that has changed.
    if (nextClockDisplayString[i] != currentClockDisplayString[i] ||
        forceUpdateAll) {
      // First, erase the character by drawing as background.
      display_drawChar(CLOCK_DISPLAY_ORIGIN_X +
                           (i * DISPLAY_CHAR_WIDTH * CLOCKDISPLAY_TEXT_SIZE),
                       CLOCK_DISPLAY_ORIGIN_Y, nextClockDisplayString[i],
                       CLOCK_BACKGROUND_COLOR, CLOCK_BACKGROUND_COLOR,
                       CLOCKDISPLAY_TEXT_SIZE);

      // Now, draw the character by drawing as foreground.
      display_drawChar(CLOCK_DISPLAY_ORIGIN_X +
                           (i * DISPLAY_CHAR_WIDTH * CLOCKDISPLAY_TEXT_SIZE),
                       CLOCK_DISPLAY_ORIGIN_Y, nextClockDisplayString[i],
                       CLOCK_FOREGROUND_COLOR, CLOCK_BACKGROUND_COLOR,
                       CLOCKDISPLAY_TEXT_SIZE);

      // Copy characters from the next string into the current string to prep
      // for next call.
      currentClockDisplayString[i] = nextClockDisplayString[i];
    }
  }
}

// Increments/decrement value, with given minValue and maxValue.
// Returns whether rollover occurred.
static bool incDecValue(bool inc, uint8_t *value, uint8_t minValue,
                        uint8_t maxValue) {
  // Keep track of roll-over.
  bool rollOver = false;

  if (inc) {
    // incrementing
    if (*value == maxValue) {
      // Handle the roll-over.
      *value = minValue;
      rollOver = true;
    } else
      (*value)++;
  } else {
    // decrementing
    if (*value == minValue)
      // Handle the roll-under.
      *value = maxValue;
    else
      (*value)--;
  }
  return rollOver;
}

// Draw a single triangle up/down button.
// The origin of the triangle is the left-most point of the triangle.
/*
 *           up                           down
 *       triangle            *          triangle   origin->  ***********
 *                         *   *                              *       *
 *                       *       *                              *   *
 *             origin-> ***********                               *
 */
// if up == true, draw the up triangle, else draw the down triangle.
static void drawTriangleUpDownButton(int16_t clockCharOriginX,
                                     int16_t clockCharOriginY, bool up,
                                     int16_t color) {
  // Drawing the up triangle.
  if (up) {
    int16_t t_x0 = clockCharOriginX;
    int16_t t_y0 = clockCharOriginY - TRIANGLE_Y_GAP;

    // Full width of the text field.
    int16_t t_x1 =
        clockCharOriginX +
        (CLOCK_CHARS_PER_FIELD * DISPLAY_CHAR_WIDTH * CLOCKDISPLAY_TEXT_SIZE);
    int16_t t_x2 = ((t_x1 - t_x0) / MIDPOINT_DIVISOR) + t_x0;
    int16_t t_y2 = t_y0 - TRIANGLE_HEIGHT;
    display_fillTriangle(t_x0, t_y0, t_x1, t_y0, t_x2, t_y2, color);
  } else {
    // Drawing the down triangle.

    int16_t t_x0 = clockCharOriginX;
    // Start at the text y-origin, add the total clock height, add the triangle
    // gap, subtract 1 character because bottom of character has CLOCK_TEXT_SIZE
    // empty pixels (built in space).
    int16_t t_y0 = clockCharOriginY +
                   (DISPLAY_CHAR_HEIGHT * CLOCKDISPLAY_TEXT_SIZE) +
                   TRIANGLE_Y_GAP - DISPLAY_CHAR_HEIGHT;
    // Full width of text field.
    int16_t t_x1 =
        clockCharOriginX +
        (CLOCK_CHARS_PER_FIELD * DISPLAY_CHAR_WIDTH * CLOCKDISPLAY_TEXT_SIZE);
    int16_t t_x2 = ((t_x1 - t_x0) / MIDPOINT_DIVISOR) + t_x0;
    int16_t t_y2 = t_y0 + TRIANGLE_HEIGHT;
    display_fillTriangle(t_x0, t_y0, t_x1, t_y0, t_x2, t_y2, color);
  }
}

// Draw all 6 of the triangle up/down buttons.
static void drawUpDownButtons(int16_t color) {
  // Draw the three up buttons.
  // left-most button starts above first character.
  drawTriangleUpDownButton(LEFT_TRIANGLE_BUTTON_X_ORIGIN,
                           CLOCK_DISPLAY_ORIGIN_Y, true, color);
  // Moving right, next button over three characters from origin.
  drawTriangleUpDownButton(MIDDLE_TRIANGLE_BUTTON_X_ORIGIN,
                           CLOCK_DISPLAY_ORIGIN_Y, true, color);
  // Moving right, last button is over 6 characters from origin.
  drawTriangleUpDownButton(RIGHT_TRIANGLE_BUTTON_X_ORIGIN,
                           CLOCK_DISPLAY_ORIGIN_Y, true, color);

  // Draw the three down buttons.
  // left-most button starts above first character.
  drawTriangleUpDownButton(LEFT_TRIANGLE_BUTTON_X_ORIGIN,
                           CLOCK_DISPLAY_ORIGIN_Y, false, color);
  // Moving right, next button over three characters from origin.
  drawTriangleUpDownButton(MIDDLE_TRIANGLE_BUTTON_X_ORIGIN,
                           CLOCK_DISPLAY_ORIGIN_Y, false, color);
  // Moving right, last button is over 6 characters from origin.
  drawTriangleUpDownButton(CLOCK_DISPLAY_ORIGIN_X +
                               DISPLAY_CHAR_WIDTH * CLOCKDISPLAY_TEXT_SIZE *
                                   RIGHT_TRIANGLE_NUM_CHARS,
                           CLOCK_DISPLAY_ORIGIN_Y, false, color);
}

// Only calls this when the touch screen is touched.
static uint8_t touchscreenComputeRegion(display_point_t touched_point) {

  // Use process of elimination. Check if in upper half or lower half.
  if (touched_point.y < TOUCH_SCREEN_UPPER_HALF) {
    if (touched_point.x <= TOUCH_SCREEN_LEFT_THIRD)
      return TOUCH_REGION_UP_HOURS;
    else if (touched_point.x <= TOUCH_SCREEN_MIDDLE_THIRD)
      return TOUCH_REGION_UP_MINUTES;
    else
      return TOUCH_REGION_UP_SECONDS;
  } else {
    if (touched_point.x <= TOUCH_SCREEN_LEFT_THIRD)
      return TOUCH_REGION_DOWN_HOURS;
    else if (touched_point.x <= TOUCH_SCREEN_MIDDLE_THIRD)
      return TOUCH_REGION_DOWN_MINUTES;
    else
      return TOUCH_REGION_DOWN_SECONDS;
  }
}
