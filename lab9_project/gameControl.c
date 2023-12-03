#include <math.h>
#include <stdio.h>

#include "config.h"
#include "display.h"
#include "interrupts.h"
#include "intervalTimer.h"
#include "touchscreen.h"
#include <stdbool.h>

#include "gameControl.h"

// Delcare one big array for all the lasers
// laser_t missiles[CONFIG_MAX_LASERS];

// State messages
#define INIT_ST_MSG "gameControl_init_st\n"
#define WAIT_TOUCH_ST_MSG "gameControl_wait_touch_st\n"
#define WAIT_RELEASE_ST_MSG "gameControl_wait_release_st\n"

// State vars
enum gameControl_st_t {
  init_st,
  wait_touch_st,
  wait_release_st,
};

static enum gameControl_st_t currentState;

// Initialize the game control logic
// This function will initialize everything
void gameControl_init() {
  display_fillScreen(CONFIG_BACKGROUND_COLOR);

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

  // Tick the lasers

  // Check for laser collision with the player

  // Update score as needed

  // Update location of the newest answer key piece if the previous
  // one was already collected.

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