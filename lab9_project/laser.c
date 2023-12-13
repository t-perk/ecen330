#include "laser.h"
#include "config.h"
#include "display.h"
#include "xil_io.h"
#include "xparameters.h"

#include "gameControl.h"
#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

// All printed messages for states are provided here.
#define INIT_ST_MSG "laser_init_st\n"
#define MOVING_ST_MSG "laser_moving_st\n"
#define DEAD_ST_MSG "laser_dead_st\n"

#define LASER_SQUARE_POWER 2
#define ONE_OR_ZERO 2

enum laser_st_t {
  init_st,   // Starts here. Will feed into the moving state
  moving_st, // laser traveling
  dead_st,   // signals that this laser can be reinitialized as a new laser
};

////////// State Machine INIT Functions //////////

// Initialize the laser as a dead laser.
void laser_init_dead(laser_t *laser) {}

// Initialize the laser.  This will randomly choose the origin and destination
// of the laser.
void laser_init_active(laser_t *laser, double speedVal) {

  laser->length = 0;
  laser->speed = CONFIG_LASER_DEFAULT_SPEED_MULTIPLIER;

  laser->up_down = rand() % ONE_OR_ZERO;

  uint16_t rand_y_origin = ((laser->up_down) ? 0 : DISPLAY_HEIGHT);
  uint16_t rand_x_origin = rand() % DISPLAY_WIDTH;

  laser->x_origin = rand_x_origin;
  laser->y_origin = rand_y_origin;

  // Set the laser destination to some point on the opposite side
  int16_t rand_y_dest = ((laser->up_down) ? DISPLAY_HEIGHT : 0);
  int16_t rand_x_dest = rand() % DISPLAY_WIDTH;

  laser->x_dest = rand_x_dest;
  laser->y_dest = rand_y_dest;

  // Update total_length
  laser->total_length =
      (int)sqrt(pow((laser->y_dest - laser->y_origin), LASER_SQUARE_POWER) +
                pow((laser->x_dest - laser->x_origin), LASER_SQUARE_POWER));
  // printf("total length on spawn is is %d\n", laser->total_length);

  laser->x_point = laser->x_origin;
  laser->y_point = laser->y_origin;

  // Math for caluating what the tail point should be
  uint16_t x_vector = (laser->x_dest - laser->x_point);
  uint16_t y_vector = (laser->y_dest - laser->y_point);

  double vector_magnitude =
      (double)sqrt((x_vector * x_vector) + (y_vector * y_vector));

  laser->x_unit_vector = x_vector / vector_magnitude;
  laser->y_unit_vector = y_vector / vector_magnitude;

  laser->x_tail = laser->x_point;
  laser->y_tail = laser->y_point;

  laser->currentState = init_st;
}

// Interesting case where it pegged a collision because the player was in
// between where the missile spawned and where it wanted to go.

// This is a debug state print routine. It will print the names of the states
// each time tick() is called. It only prints states if they are different than
// the previous state.
void debugStatePrint_Laser(laser_t *laser) {
  static enum laser_st_t previousState;
  static bool firstPass = true;
  // Only print the message if:
  // 1. This the first pass and the value for previousState is unknown.
  // 2. previousState != currentState - this prevents reprinting the same state
  // name over and over.
  if (previousState != laser->currentState || firstPass) {
    firstPass = false; // previousState will be defined, firstPass is false.
    // keep track of the last state that you were in.
    previousState = laser->currentState;
    switch (laser->currentState) {
    case init_st:
      printf(INIT_ST_MSG);
      break;
    case moving_st:
      printf(MOVING_ST_MSG);
      break;
    case dead_st:
      printf(DEAD_ST_MSG);
      break;
    default:
      printf("ERROR: Unaccounted state debug.\n");
    }
  }
}

////////// State Machine TICK Function //////////
void laser_tick(laser_t *laser) {
  // debugStatePrint_Laser(laser);

  // Handle state transitions
  switch (laser->currentState) {

    // Init transition state
  case init_st:
    laser->currentState = moving_st;
    break;
    // Update and draw lasers
  case moving_st:

    // Erase the current line
    display_drawLine(laser->x_tail, laser->y_tail, laser->x_point,
                     laser->y_point, CONFIG_BACKGROUND_COLOR);

    laser->length = laser->length + (CONFIG_LASER_DISTANCE_PER_TICK *
                                     laser->speed); // SPEED HERE

    //  Calculate the new x_current and y_current given the speed at which
    //  the laser is moving. Then see if the new length is close enough to
    //  its completed path. If so, move to dead state. If not, update
    //  the line.

    double percentage = laser->length / laser->total_length;
    double epsilon = 0.01; // Might need to finagle this around

    // Check to see if the missle has traveled far enough. If so, don't draw a
    // new one, but instead transition to another state

    if ((laser->length >= laser->total_length) && (laser->total_length > 0)) {

      laser->currentState = dead_st;

    } else {
      // Not close enough, so go ahead and draw the line and stay in the same
      // state.

      laser->x_point =
          laser->x_origin + percentage * (laser->x_dest - laser->x_origin);
      laser->y_point =
          laser->y_origin + percentage * (laser->y_dest - laser->y_origin);

      //  Math for caluating what the tail point should be
      int16_t x_vector = (laser->x_dest - laser->x_point);
      int16_t y_vector = (laser->y_dest - laser->y_point);

      double vector_magnitude =
          (double)sqrt((x_vector * x_vector) + (y_vector * y_vector));

      laser->x_unit_vector = x_vector / vector_magnitude;
      laser->y_unit_vector = y_vector / vector_magnitude;

      laser->x_tail =
          laser->x_point + (-CONFIG_LASER_SIZE * laser->x_unit_vector);
      laser->y_tail =
          laser->y_point + (-CONFIG_LASER_SIZE * laser->y_unit_vector);

      // Draw the new line
      display_drawLine(laser->x_tail, laser->y_tail, laser->x_point,
                       laser->y_point, CONFIG_COLOR_LASER);

      laser->currentState = moving_st;
    }

    break;

    // Stay in the dead_st until you are reinitialized
  case dead_st:
    laser->currentState = dead_st;
    display_point_t point1 = {laser->x_point, laser->y_point};

    break;
  default:
    printf("ERROR: Unaccounted state transition.\n");
  }

  // Handle state actions
  switch (laser->currentState) {
  case init_st:
    break;
  case moving_st:
    break;
  case dead_st:
    break;
  default:
    printf("ERROR: Unaccounted state action.\n");
  }
}

// Return whether the given laser is dead.
bool laser_is_dead(laser_t *laser) { return (dead_st == laser->currentState); }