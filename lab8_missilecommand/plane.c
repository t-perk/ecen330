#include "plane.h"
#include "config.h"
#include "display.h"
#include <stdbool.h>
#include <stdio.h>

// All printed messages for states are provided here.
#define INIT_ST_MSG "plane_init_st\n"
#define WAIT_SPAWN_MSG "plane_wait_spawn_st\n"
#define MOVING_ST_MSG "plane_moving_st\n"
#define REMOVE_ST_MSG "plane_remove_st\n"

// in seconds
#define PLANE_SPAWN_TIME 3

// Defines for location of the plane for drawing and spawning
#define PLANE_X_OFFSET_BACK 6
#define PLANE_Y_OFFSET 4
#define PLANE_X_OFFSET_FRONT 12
#define PLANE_SPAWN_Y_OFFSET 60

// Delcare plane stats
int16_t x_current;
int16_t y_current;

// One one missile should fire each time the plane passes by
bool missileFired;
// Distance after which the plane should fire its missile
uint32_t plane_missile_fire_dist;

// While flying, this flag is used to indicate the plane should be removed
bool explode_me;

// Setup for timer to spawn in plane
uint32_t wait_spawn_cnt = 0;
uint32_t wait_num_ticks = 0;

// Point to whatever the missile_pointer is pointing to.
missile_t *plane_missile_pointer;

// States for the plane state machine
enum plane_st_t {
  init_st,
  wait_spawn_st,
  moving_st,
};

static enum plane_st_t currentState;

// Takes in draw to decided whether to draw or erase triangle based off of the
// plane's current position.
void drawPlane_helper(bool draw) {
  // Calculate the points of the triangle based off of x_current & y_current

  int16_t x0 = x_current - PLANE_X_OFFSET_FRONT;
  int16_t y0 = y_current;

  int16_t x1 = x_current + PLANE_X_OFFSET_BACK;
  int16_t y1 = y_current - PLANE_Y_OFFSET;

  int16_t x2 = x_current + PLANE_X_OFFSET_BACK;
  int16_t y2 = y_current + PLANE_Y_OFFSET;

  display_fillTriangle(x0, y0, x1, y1, x2, y2,
                       (draw) ? DISPLAY_WHITE : CONFIG_BACKGROUND_COLOR);
}

// Initialize the plane state machine
// Pass in a pointer to the missile struct (the plane will only have one
// missile)
void plane_init(missile_t *plane_missile) {

  plane_missile->type = MISSILE_TYPE_PLANE;
  plane_missile_pointer = plane_missile;

  x_current = 0;
  y_current = 0;
  explode_me = false;
  plane_missile_fire_dist = 0;

  missileFired = false;

  // Set current state
  currentState = init_st;

  // Initialize values
  wait_spawn_cnt = 0;

  // number of ticks needs before spawning the plane again.
  wait_num_ticks = PLANE_SPAWN_TIME / CONFIG_GAME_TIMER_PERIOD;
}

// This is a debug state print routine. It will print the names of the states
// each time tick() is called. It only prints states if they are different than
// the previous state.
void debugStatePrint_plane() {
  static enum plane_st_t previousState;
  static bool firstPass = true;
  // Only print the message if:
  // 1. This the first pass and the value for previousState is unknown.
  // 2. previousState != currentState - this prevents reprinting the same state
  // name over and over.
  if (previousState != currentState || firstPass) {
    firstPass = false; // previousState will be defined, firstPass is false.
    previousState =
        currentState;       // keep track of the last state that you were in.
    switch (currentState) { // This prints messages based upon the state that
                            // you were in.
    case init_st:
      printf(INIT_ST_MSG);
      break;
    case wait_spawn_st:
      printf(WAIT_SPAWN_MSG);
      break;
    case moving_st:
      printf(MOVING_ST_MSG);
      break;
    default:
      printf("ERROR: Unaccounted plane state action.\n");
    }
  }
}

// State machine tick function
void plane_tick() {
  debugStatePrint_plane();

  // Handle state transitions
  switch (currentState) {
  case init_st:
    currentState = wait_spawn_st;
    break;
  case wait_spawn_st:
    if (wait_spawn_cnt == wait_num_ticks) {
      wait_spawn_cnt = 0;
      // spawnPlane on right-hand side
      x_current = DISPLAY_WIDTH + PLANE_X_OFFSET_FRONT;
      y_current = PLANE_SPAWN_Y_OFFSET;
      missileFired = false;

      drawPlane_helper(true);
      // Choose a random x point at which the plane will fire it's missile
      plane_missile_fire_dist = rand() % DISPLAY_WIDTH;

      currentState = moving_st;
    } else {
      currentState = wait_spawn_st;
    }
    break;
  case moving_st:

    // Erase past position
    drawPlane_helper(false);
    // Update location
    x_current -= CONFIG_PLANE_DISTANCE_PER_TICK;
    // Redraw current position if the plane is still onscreen
    if (explode_me) {
      // Increment impact
      explode_me = false;
      currentState = wait_spawn_cnt;
      break;
    }

    // If the plane has not gone offscreen
    if (x_current > (0 - PLANE_X_OFFSET_BACK)) {
      drawPlane_helper(true);

      if (x_current < plane_missile_fire_dist && !missileFired) {
        // Fire missile
        missileFired = true;

        missile_init_plane(&plane_missile_pointer[0], x_current, y_current);
        // missile_init_plane($pl);
      }

      currentState = moving_st;
      break;
    } else {
      // Plane moved offscreen
      currentState = wait_spawn_st;
      break;
    }
    break;
  default:
    printf("ERROR: Unaccounted state transition.\n");
  }

  // Handle actions
  switch (currentState) {
  case init_st:
    break;
  case wait_spawn_st:
    wait_spawn_cnt++;
    break;
  case moving_st:
    break;
  default:
    printf("ERROR: Unaccounted state action.\n");
  }
}

// Enables the plane's explode condition
void plane_explode() { explode_me = true; }

// Get the XY location of the plane
display_point_t plane_getXY() {
  display_point_t planeLocation;
  planeLocation.x = x_current;
  planeLocation.y = y_current;

  return planeLocation;
}