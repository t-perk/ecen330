#include <math.h>
#include <stdio.h>

#include "config.h"
#include "display.h"
#include "interrupts.h"
#include "intervalTimer.h"
#include "laser.h"
#include "touchscreen.h"
#include <stdbool.h>

#include "gameControl.h"

// State messages
#define INIT_ST_MSG "gameControl_init_st\n"
#define WAIT_TOUCH_ST_MSG "gameControl_wait_touch_st\n"
#define WAIT_RELEASE_ST_MSG "gameControl_wait_release_st\n"

// Player defines
#define PLAYER_RADIUS 6

#define TWO_MOD 2

// Stat display defines
#define STATS_TEXT_SIZE 1.8
#define TEXT_CURSOR_X 8
#define TEXT_CURSOR_Y 2
#define STRING_MAX_LENGTH 100

#define WIN_TEXT_SIZE 6
#define WIN_TEXT_CURSOR_X 30
#define WIN_TEXT_CURSOR_Y 100

#define LOSE_TEXT_SIZE 6
#define LOSE_TEXT_CURSOR_X 20
#define LOSE_TEST_CURSOR_Y 100

// State vars
enum gameControl_st_t {
  init_st,
  wait_touch_st,
  wait_release_st,
};

// Delcare one big array for all the lasers
laser_t lasers[CONFIG_MAX_LASERS];

// Player variables
display_point_t playerLocation;

// Scoring variables
uint8_t score;
bool answerKeyActive;
display_point_t answerKeyLocation;
bool winCondition;
bool loseCondition;
bool tickOddLasers = true;
double laserSpeed = CONFIG_LASER_DEFAULT_SPEED_MULTIPLIER;

// Declare variables
uint32_t lose_reset_delay = 0;
uint32_t delay_num_ticks = 0;

static enum gameControl_st_t currentState;

void drawWin(bool draw) {
  // Configure display text settings
  display_setTextColor((draw) ? DISPLAY_YELLOW : CONFIG_BACKGROUND_COLOR);
  display_setTextSize(WIN_TEXT_SIZE); // Resize the text.

  char output_string[STRING_MAX_LENGTH];

  // Set the cursor location and print to the LCD
  display_setCursor(WIN_TEXT_CURSOR_X, WIN_TEXT_CURSOR_Y);

  // Format the stuff you want to print as a string so its compatible with the
  // display_println function
  sprintf(output_string, "YOU WIN\n");
  display_println(output_string);
}

void drawLose(bool draw) {
  // Configure display text settings
  display_setTextColor((draw) ? DISPLAY_RED : CONFIG_BACKGROUND_COLOR);
  display_setTextSize(LOSE_TEXT_SIZE); // Resize the text.

  char output_string[STRING_MAX_LENGTH];

  // Set the cursor location and print to the LCD
  display_setCursor(LOSE_TEXT_CURSOR_X, LOSE_TEST_CURSOR_Y);

  // Format the stuff you want to print as a string so its compatible with the
  // display_println function
  sprintf(output_string, "YOU LOSE\n");
  display_println(output_string);
}

// Draws or erases the stats given the state of bool draw
void drawStats_helper(bool draw) {
  // Configure display text settings
  display_setTextColor(
      (draw) ? DISPLAY_WHITE : CONFIG_BACKGROUND_COLOR); // Make the text white.
  display_setTextSize(STATS_TEXT_SIZE);                  // Resize the text.

  char output_string[STRING_MAX_LENGTH];

  // Set the cursor location and print to the LCD
  display_setCursor(TEXT_CURSOR_X, TEXT_CURSOR_Y);

  // Format the stuff you want to print as a string so its compatible with the
  // display_println function
  sprintf(output_string, "Score: %d\n", score);
  display_println(output_string);
}

// Initialize the game control logic
// This function will initialize everything
void gameControl_init() {
  display_fillScreen(CONFIG_BACKGROUND_COLOR);

  playerLocation.x = DISPLAY_WIDTH / TWO_MOD;
  playerLocation.y = DISPLAY_HEIGHT / TWO_MOD;

  display_drawCircle(playerLocation.x, playerLocation.y, PLAYER_RADIUS,
                     CONFIG_COLOR_PLAYER);

  score = 0;
  drawStats_helper(true);
  answerKeyActive = false;
  winCondition = false;
  loseCondition = false;

  // Initialize values
  lose_reset_delay = 0;

  // Setup number of ticks needed to enter super speedy fast update mode.
  // Vrooooooooooom
  delay_num_ticks = CONFIG_LOSE_RESET_TIME / CONFIG_GAME_TIMER_PERIOD;

  // Initialize lasers
  for (uint16_t i = 0; i < CONFIG_MAX_LASERS; i++) {
    laser_init_active(&lasers[i], CONFIG_LASER_DEFAULT_SPEED_MULTIPLIER);
  }

  currentState = init_st;
}

// This is a debug state print routine. It will print the names of the states
// each time tick() is called. It only prints states if they are different than
// the previous state.
void debugStatePrint_gameControl() {
  static enum gameControl_st_t previousState;
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
    case wait_touch_st:
      printf(WAIT_TOUCH_ST_MSG);
      break;
    case wait_release_st:
      printf(WAIT_RELEASE_ST_MSG);
      break;
    default:
      printf("ERROR: Unaccounted gameControl state action.\n");
    }
  }
}

// Tick the game control logic
//
// This function should tick the lasers, handle screen touches, collisions,
// and updating statistics.
void gameControl_tick() {

  // debugStatePrint_gameControl();
  // Update stats to keep a clean look
  drawStats_helper(true);

  // Don't do anything if there player has lost
  if (loseCondition) {
    // Wait for a while, then reset everything
    if (lose_reset_delay == delay_num_ticks) {
      gameControl_init();
    } else {
      lose_reset_delay++;
    }
    return;
  }

  // Check for game win condition
  if (score >= 10 && !winCondition && !loseCondition) {
    winCondition = true;
    drawWin(true);
  } else { // Ticked the lasers

    for (uint16_t i = (tickOddLasers ? 0 : 1); i < CONFIG_MAX_LASERS; i += 2) {
      laser_tick(&lasers[i]);
    }

    tickOddLasers = !tickOddLasers;
  }

  /*
  To calculate a collision between the line segment and a circle:
    - Find the line from the circle that goes perpendicular to the laser line
    segment
    - If the distance between the center circle to the line segment is less than
  the radius, you’ve got a collision.
  */

  // Check for laser collision with the player
  // Loop through each laser
  bool playerHit = false;
  for (uint16_t i = 0; i < CONFIG_MAX_LASERS; i++) {
    double segmentLength = sqrt(((lasers[i].x_point - lasers[i].x_tail) *
                                 (lasers[i].x_point - lasers[i].x_tail)) +
                                ((lasers[i].y_point - lasers[i].y_tail) *
                                 (lasers[i].y_point - lasers[i].y_tail)));

    // SegmentLength sits at around 20

    double distanceToLine =
        fabs((lasers[i].y_tail - lasers[i].y_point) * playerLocation.x -
             (lasers[i].x_tail - lasers[i].x_point) * playerLocation.y +
             lasers[i].x_tail * lasers[i].y_point -
             lasers[i].y_tail * lasers[i].x_point) /
        segmentLength;

    double distanceToCenter = sqrt(
        ((lasers[i].x_point + lasers[i].x_tail) / TWO_MOD - playerLocation.x) *
            ((lasers[i].x_point + lasers[i].x_tail) / TWO_MOD -
             playerLocation.x) +
        ((lasers[i].y_point + lasers[i].y_tail) / TWO_MOD - playerLocation.y) *
            ((lasers[i].y_point + lasers[i].y_tail) / TWO_MOD -
             playerLocation.y));

    // Check if the circle is within half of the length of the line segment
    if (distanceToLine <= CONFIG_RADIUS_ANSWERKEY &&
        distanceToCenter <= (segmentLength / TWO_MOD)) {
      // printf("Collision detected!\n");
      playerHit = true;
    }
  }

  // The player loses when the player has been hit by a
  //     laser and has not already won.
  if (playerHit && !winCondition) {
    loseCondition = true;
    drawLose(true);
  }

  // Check for player collision with answer key piece
  // Update location of the newest answer key piece if the previous
  // one was already collected.
  if (answerKeyActive) { // Check for collision with player
    uint16_t playerDistanceFromAnswerKeySquared =
        ((playerLocation.y - answerKeyLocation.y) *
         (playerLocation.y - answerKeyLocation.y)) +
        ((playerLocation.x - answerKeyLocation.x) *
         (playerLocation.x - answerKeyLocation.x));

    if (playerDistanceFromAnswerKeySquared <
        (CONFIG_RADIUS_ANSWERKEY * CONFIG_RADIUS_ANSWERKEY)) {

      // Erase answer key
      display_fillCircle(answerKeyLocation.x, answerKeyLocation.y,
                         CONFIG_RADIUS_ANSWERKEY, CONFIG_BACKGROUND_COLOR);

      // Increment score and update stats
      drawStats_helper(false);
      score++;
      laserSpeed = laserSpeed + CONFIG_LASER_SPEED_MULTIPLIER_PER_SCORE;
      drawStats_helper(true);
      answerKeyActive = false;
    }
  } else { // Spawn in new answer key
    answerKeyActive = true;

    uint16_t answerKeyLocation_x = rand() % DISPLAY_WIDTH;
    uint16_t answerKeyLocation_y = rand() % DISPLAY_HEIGHT;

    answerKeyLocation.x = answerKeyLocation_x;
    answerKeyLocation.y = answerKeyLocation_y;

    display_fillCircle(answerKeyLocation_x, answerKeyLocation_y,
                       CONFIG_RADIUS_ANSWERKEY, CONFIG_COLOR_ANSWERKEY);
  }

  // Check for dead lasers and re-initialize
  for (uint16_t i = 0; i < CONFIG_MAX_LASERS; i++) {
    if (laser_is_dead(&lasers[i])) {
      laser_init_active(&lasers[i], laserSpeed);
    }
  }

  // Update score as needed

  // Handle transitions
  switch (currentState) {
  case init_st:
    currentState = wait_touch_st;
    break;

  // Waiting state until the there is player input. Player will move towards
  // specified location.
  case wait_touch_st:
    if (touchscreen_get_status() == TOUCHSCREEN_PRESSED) {
      // Send to a state that waits for the player to let go of the
      // screen
      currentState = wait_release_st;
    } else {
      currentState = wait_touch_st;
    }
    break;

  // Stops player movement
  case wait_release_st:
    // I was running into a weird issue where my touchscreen status was
    // skipping released and going straight to idle, so I just used this line
    // instead.
    if (touchscreen_get_status() != TOUCHSCREEN_PRESSED) {

      touchscreen_ack_touch();

      currentState = wait_touch_st;

    } else {
      // Update the touchPoint. Could be an expensive function. If so, change to
      // increment every nth cycle
      display_point_t targetPlayerPosition = touchscreen_get_location();

      // Increment the location of the player to go closer to the location.
      // Erase player
      display_drawCircle(playerLocation.x, playerLocation.y, PLAYER_RADIUS,
                         CONFIG_BACKGROUND_COLOR);

      // consider adding some buffer space
      // Increment player location x coord
      if (playerLocation.x > targetPlayerPosition.x) {
        // Decrement the x coord
        playerLocation.x = playerLocation.x - CONFIG_PLAYER_MOVEMENT_PER_TICK;
      } else if (playerLocation.x < targetPlayerPosition.x) {
        playerLocation.x = playerLocation.x + CONFIG_PLAYER_MOVEMENT_PER_TICK;
      }

      // Increment player location y coord
      if (playerLocation.y > targetPlayerPosition.y) {
        // Decrement the y coord
        playerLocation.y = playerLocation.y - CONFIG_PLAYER_MOVEMENT_PER_TICK;
      } else if (playerLocation.y < targetPlayerPosition.y) {
        playerLocation.y = playerLocation.y + CONFIG_PLAYER_MOVEMENT_PER_TICK;
      }

      // Draw player
      display_drawCircle(playerLocation.x, playerLocation.y, PLAYER_RADIUS,
                         CONFIG_COLOR_PLAYER);

      currentState = wait_release_st;
    }
    break;
  default:
    printf("ERROR: Unaccounted state transition.\n");
  }

  // Handle actions
  switch (currentState) {
  case init_st:
    break;
  case wait_touch_st:
    break;
  case wait_release_st:
    break;
  default:
    printf("ERROR: Unaccounted state action.\n");
  }
}