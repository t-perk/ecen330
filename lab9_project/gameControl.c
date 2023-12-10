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

// Delcare one big array for all the lasers
laser_t lasers[CONFIG_MAX_LASERS];

// State messages
#define INIT_ST_MSG "gameControl_init_st\n"
#define WAIT_TOUCH_ST_MSG "gameControl_wait_touch_st\n"
#define WAIT_RELEASE_ST_MSG "gameControl_wait_release_st\n"

// Player defines
#define PLAYER_SIZE_OFFSET 6
#define PLAYER_SIZE 12
#define TWO_MOD 2

// Stat display defines
#define STATS_TEXT_SIZE 1.8
#define TEXT_CURSOR_X 8
#define TEXT_CURSOR_Y 2
#define STRING_MAX_LENGTH 100

#define WIN_TEXT_SIZE 6
#define WIN_TEXT_CURSOR_X 30
#define WIN_TEXT_CURSOR_Y 100

// State vars
enum gameControl_st_t {
  init_st,
  wait_touch_st,
  wait_release_st,
};

// Player variables
display_point_t playerLocation;

// Scoring variables
uint8_t score;
bool answerKeyActive;
display_point_t answerKeyLocation;
bool winCondition;
bool tickOddLasers = true;

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

  display_drawRect(playerLocation.x - PLAYER_SIZE_OFFSET,
                   playerLocation.y + PLAYER_SIZE_OFFSET, PLAYER_SIZE,
                   PLAYER_SIZE, CONFIG_COLOR_PLAYER);

  score = 0;
  drawStats_helper(true);
  answerKeyActive = false;
  winCondition = false;

  // Initialize lasers
  for (uint16_t i = 0; i < CONFIG_MAX_LASERS; i++) {
    laser_init_active(&lasers[i]);
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

  debugStatePrint_gameControl();

  // Check for game win condition
  if (score >= 10 && !winCondition) {
    winCondition = true;
    drawWin(true);
  } else { // Ticked the lasers
    // Tick every other missile
    for (uint16_t i = (tickOddLasers ? 0 : 1); i < CONFIG_MAX_LASERS; i += 2) {
      laser_tick(&lasers[i]);
    }

    tickOddLasers = !tickOddLasers;
  }

  // Check for laser collision with the player

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
      laser_init_active(&lasers[i]);
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
      display_drawRect(playerLocation.x - PLAYER_SIZE_OFFSET,
                       playerLocation.y + PLAYER_SIZE_OFFSET, PLAYER_SIZE,
                       PLAYER_SIZE, CONFIG_BACKGROUND_COLOR);

      //**TODO** If the player is jittering back and forth a bit, you might
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
      display_drawRect(playerLocation.x - PLAYER_SIZE_OFFSET,
                       playerLocation.y + PLAYER_SIZE_OFFSET, PLAYER_SIZE,
                       PLAYER_SIZE, CONFIG_COLOR_PLAYER);

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