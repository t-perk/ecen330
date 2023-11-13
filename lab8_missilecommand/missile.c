#include "missile.h"
#include "config.h"
#include "display.h"
#include "plane.h"
#include "xil_io.h"
#include "xparameters.h"

#include <math.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#define MISSILE_TOP_LIMIT_HEIGHT 16
#define MISSILE_BOTTOM_LIMIT_HEIGHT DISPLAY_HEIGHT

#define MISSILE_PLAYER_NUMBER_SPAWN_LOCATIONS 3
#define MISSILE_PLAYER_MISSILE_SPAWN_LEFT DISPLAY_WIDTH * .25
#define MISSILE_PLAYER_MISSILE_SPAWN_CENTER DISPLAY_WIDTH * .5
#define MISSILE_PLAYER_MISSILE_SPAWN_RIGHT DISPLAY_WIDTH * .75

#define MISSILE_CLOSEST_DISTANCE_DEFAULT 500

#define MISSILE_SQUARE_POWER 2

// Currently set to 2 because each tick function inside of gameControl only
// ticks half of the missiles, so we need everything to be mulitplied by 2
#define MISSILE_SPEED_MULTIPLIER 2

// All printed messages for states are provided here.
#define INIT_ST_MSG "missile_init_st\n"
#define MOVING_ST_MSG "missile_moving_st\n"
#define EXPLODE_GROW_ST_MSG "missile_explode_grow_st\n"
#define EXPLODE_SHRINK_ST_MSG "missile_explode_shrink_st\n"
#define DEAD_ST_MSG "missile_dead_st\n"

enum missile_st_t {
  init_st,           // Starts here. Will feed into the moving state
  moving_st,         // missile traveling
  explode_grow_st,   // Missile reached target and is exploding
  explode_shrink_st, // Missle done exploding and is now shrinking
  dead_st,           // not sure what this does- yet
};

////////// State Machine INIT Functions //////////
// Unlike most state machines that have a single `init` function, our missile
// will have different initializers depending on the missile type.
void missile_init_helper(missile_t *missile) {

  missile->length = 0;
  missile->explode_me = false;
  // TODO replace the sqrt to speed up process if dropping.
  missile->total_length =
      sqrt(pow((missile->y_dest - missile->y_origin), MISSILE_SQUARE_POWER) +
           pow((missile->x_dest - missile->x_origin), MISSILE_SQUARE_POWER));
  missile->x_current = missile->x_origin;
  missile->y_current = missile->y_origin;
  missile->impacted = false;
}

// Initialize the missile as a dead missile.  This is useful at the start of the
// game to ensure that player and plane missiles aren't moving before they
// should.
void missile_init_dead(missile_t *missile) {
  missile_init_helper(missile);

  missile->currentState = dead_st;
}

// Initialize the missile as an enemy missile.  This will randomly choose the
// origin and destination of the missile.  The origin should be somewhere near
// the top of the screen, and the destination should be the very bottom of the
// screen.
void missile_init_enemy(missile_t *missile) {

  missile_init_helper(missile);

  missile->type = MISSILE_TYPE_ENEMY;

  // Grab a random x coord between x and the right side of the screen.
  uint16_t enemy_rand_x_origin = rand() % DISPLAY_WIDTH;
  uint16_t enemy_y_origin = MISSILE_TOP_LIMIT_HEIGHT;

  missile->x_origin = enemy_rand_x_origin;
  missile->y_origin = enemy_y_origin;

  uint16_t enemy_rand_x_dest = rand() % DISPLAY_WIDTH;
  uint16_t enemy_y_dest = DISPLAY_HEIGHT;

  missile->x_dest = enemy_rand_x_dest;
  missile->y_dest = enemy_y_dest;

  // Update total_length
  missile->total_length = (int)sqrt(
      pow((missile->y_dest - missile->y_origin), MISSILE_SQUARE_POWER) +
      pow((missile->x_dest - missile->x_origin), MISSILE_SQUARE_POWER));

  missile->currentState = init_st;
}

// Initialize the missile as a player missile.  This function takes an (x, y)
// destination of the missile (where the user touched on the touchscreen).  The
// origin should be the closest "firing location" to the destination (there are
// three firing locations evenly spaced along the bottom of the screen).
void missile_init_player(missile_t *missile, uint16_t x_dest, uint16_t y_dest) {

  missile_init_helper(missile);

  missile->type = MISSILE_TYPE_PLAYER;

  // Default spawn location
  uint8_t chosenSpawnLocation_x = MISSILE_PLAYER_MISSILE_SPAWN_LEFT;
  uint16_t closestDistance = MISSILE_CLOSEST_DISTANCE_DEFAULT;

  // Find the closest spawn point
  // Loop through each of the three spawn points and see if it's the closest

  // Left spawn point
  // Normally I would take the sqrt of all these
  uint16_t distToSpawnPoint =
      sqrt(((y_dest - MISSILE_BOTTOM_LIMIT_HEIGHT) *
            (y_dest - MISSILE_BOTTOM_LIMIT_HEIGHT)) +
           ((x_dest - MISSILE_PLAYER_MISSILE_SPAWN_LEFT) *
            (x_dest - MISSILE_PLAYER_MISSILE_SPAWN_LEFT)));

  // By default, we are putting the spawn point at the left and setting the
  // starting closestDistance
  closestDistance = distToSpawnPoint;

  // Center spawn point
  distToSpawnPoint = sqrt(((y_dest - MISSILE_BOTTOM_LIMIT_HEIGHT) *
                           (y_dest - MISSILE_BOTTOM_LIMIT_HEIGHT)) +
                          ((x_dest - MISSILE_PLAYER_MISSILE_SPAWN_CENTER) *
                           (x_dest - MISSILE_PLAYER_MISSILE_SPAWN_CENTER)));
  if (distToSpawnPoint < closestDistance) {
    chosenSpawnLocation_x = MISSILE_PLAYER_MISSILE_SPAWN_CENTER;
    closestDistance = distToSpawnPoint;
  }

  // Right spawn point
  distToSpawnPoint = sqrt(((y_dest - MISSILE_BOTTOM_LIMIT_HEIGHT) *
                           (y_dest - MISSILE_BOTTOM_LIMIT_HEIGHT)) +
                          ((x_dest - MISSILE_PLAYER_MISSILE_SPAWN_RIGHT) *
                           (x_dest - MISSILE_PLAYER_MISSILE_SPAWN_RIGHT)));
  if (distToSpawnPoint < closestDistance) {
    chosenSpawnLocation_x = MISSILE_PLAYER_MISSILE_SPAWN_RIGHT;
    closestDistance = distToSpawnPoint;
  }

  missile->x_origin = chosenSpawnLocation_x;
  missile->y_origin = MISSILE_BOTTOM_LIMIT_HEIGHT;

  missile->x_dest = x_dest;
  missile->y_dest = y_dest;

  // Update total_length
  missile->total_length = (int)sqrt(((missile->y_dest - missile->y_origin) *
                                     (missile->y_dest - missile->y_origin)) +
                                    ((missile->x_dest - missile->x_origin) *
                                     (missile->x_dest - missile->x_origin)));

  missile->currentState = init_st;
}

// Initialize the missile as a plane missile.  This function takes an (x, y)
// location of the plane which will be used as the origin.  The destination can
// be randomly chosed along the bottom of the screen.
void missile_init_plane(missile_t *missile, int16_t plane_x, int16_t plane_y) {

  missile_init_helper(missile);

  missile->type = MISSILE_TYPE_PLANE;

  missile->x_origin = plane_x;
  missile->y_origin = plane_y;

  uint16_t plane_rand_x_dest = rand() % DISPLAY_WIDTH;
  uint16_t plane_y_dest = DISPLAY_HEIGHT;

  missile->x_dest = plane_rand_x_dest;
  missile->y_dest = plane_y_dest;

  // Update total_length
  missile->total_length = (int)sqrt(
      pow((missile->y_dest - missile->y_origin), MISSILE_SQUARE_POWER) +
      pow((missile->x_dest - missile->x_origin), MISSILE_SQUARE_POWER));

  missile->currentState = init_st;
}

// This is a debug state print routine. It will print the names of the states
// each time tick() is called. It only prints states if they are different than
// the previous state.
void debugStatePrint_Missile(missile_t *missile) {
  static enum missile_st_t previousState;
  static bool firstPass = true;
  // Only print the message if:
  // 1. This the first pass and the value for previousState is unknown.
  // 2. previousState != currentState - this prevents reprinting the same state
  // name over and over.
  if (previousState != missile->currentState || firstPass) {
    firstPass = false; // previousState will be defined, firstPass is false.
    // keep track of the last state that you were in.
    previousState = missile->currentState;
    switch (missile->currentState) {
    case init_st:
      printf(INIT_ST_MSG);
      break;
    case moving_st:
      printf(MOVING_ST_MSG);
      break;
    case explode_grow_st:
      printf(EXPLODE_GROW_ST_MSG);
      break;
    case explode_shrink_st:
      printf(EXPLODE_SHRINK_ST_MSG);
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
void missile_tick(missile_t *missile) {

  // debugStatePrint_Missile(missile);

  // Handle state transitions
  switch (missile->currentState) {

  case init_st:
    missile->currentState = moving_st;
    break;
  // Update and draw missile lines
  case moving_st:

    // Erase the current line
    display_drawLine(missile->x_origin, missile->y_origin, missile->x_current,
                     missile->y_current, CONFIG_BACKGROUND_COLOR);

    if (MISSILE_TYPE_PLAYER == missile->type) {
      missile->length =
          missile->length +
          (CONFIG_PLAYER_MISSILE_DISTANCE_PER_TICK * MISSILE_SPEED_MULTIPLIER);
    } else if ((MISSILE_TYPE_ENEMY == missile->type) ||
               (MISSILE_TYPE_PLANE == missile->type)) {
      missile->length =
          missile->length +
          (CONFIG_ENEMY_MISSILE_DISTANCE_PER_TICK * MISSILE_SPEED_MULTIPLIER);
    }

    //  Calculate the new x_current and y_current given the speed at which
    //  the missile is moving. Then see if the new length is close enough to
    //  its completed path. If so, move to exploding state. If not, update
    //  the line.
    // x_current = x_origin + percentage * (x_dest – x_origin)
    double percentage = missile->length / (double)missile->total_length;
    double epsilon = 0.01; // Might need to finagle this around

    // Check to see if the missle has traveled far enough. If so, don't draw a
    // new one, but instead transition to another state
    if (missile->length > missile->total_length ||
        missile->length >= DISPLAY_HEIGHT) {
      // It has reached it final destination.
      // If it's a player missile. Make it explode.
      if (missile->type == MISSILE_TYPE_PLAYER) {
        missile->explode_me = true;
        missile->currentState = explode_grow_st;
        break;
      } else {
        // If it's not a player missile.
        // If it's at the bottom of the screen don't make it explode, but
        // incremement impact.
        missile->impacted = true;
        missile->currentState = dead_st;
        break;
      }
    } else if (missile->explode_me) {
      // Execute order 66
      missile->explode_me = true;
      missile->currentState = explode_grow_st;

    } else {
      // Not close enough, so go ahead and draw the line and stay in the same
      // state.
      missile->x_current = missile->x_origin +
                           percentage * (missile->x_dest - missile->x_origin);
      missile->y_current = missile->y_origin +
                           percentage * (missile->y_dest - missile->y_origin);

      // Display the new line dependant on the missile type
      if (MISSILE_TYPE_PLAYER == missile->type) {
        display_drawLine(missile->x_origin, missile->y_origin,
                         missile->x_current, missile->y_current,
                         CONFIG_COLOR_PLAYER);
      } else if (MISSILE_TYPE_ENEMY == missile->type) {
        display_drawLine(missile->x_origin, missile->y_origin,
                         missile->x_current, missile->y_current,
                         CONFIG_COLOR_ENEMY);
      } else if (MISSILE_TYPE_PLANE == missile->type) {
        display_drawLine(missile->x_origin, missile->y_origin,
                         missile->x_current, missile->y_current,
                         CONFIG_COLOR_PLANE);
      } else {
        printf("Type error. Please resolve\n");
      }

      missile->currentState = moving_st;
    }

    break;

  // Progressively grow the radius of the explosion
  case explode_grow_st:
    // Increase radius
    missile->radius =
        missile->radius +
        (CONFIG_EXPLOSION_RADIUS_CHANGE_PER_TICK * MISSILE_SPEED_MULTIPLIER);

    // If circle radius is beneath its max, draw the bigger circle. If not
    // then transition states to explode_shrink_st
    if (missile->radius >= CONFIG_EXPLOSION_MAX_RADIUS) {
      missile->currentState = explode_shrink_st;
    } else {
      if (MISSILE_TYPE_PLAYER == missile->type) {
        display_fillCircle(missile->x_current, missile->y_current,
                           missile->radius, CONFIG_COLOR_PLAYER);
      } else if (MISSILE_TYPE_ENEMY == missile->type) {
        display_fillCircle(missile->x_current, missile->y_current,
                           missile->radius, CONFIG_COLOR_ENEMY);
      } else if (MISSILE_TYPE_PLANE == missile->type) {
        display_fillCircle(missile->x_current, missile->y_current,
                           missile->radius, CONFIG_COLOR_PLANE);
      }

      missile->currentState = explode_grow_st;
    }

    break;

  // Progressively shrink the radius of the explosion
  case explode_shrink_st:
    // Erase circle
    display_fillCircle(missile->x_current, missile->y_current, missile->radius,
                       CONFIG_BACKGROUND_COLOR);

    // Decrease radius
    missile->radius =
        missile->radius -
        (CONFIG_EXPLOSION_RADIUS_CHANGE_PER_TICK * MISSILE_SPEED_MULTIPLIER);

    // Draw circle - TEST
    if (missile->radius > 2) {
      // draw smaller circle
      if (MISSILE_TYPE_PLAYER == missile->type) {
        display_fillCircle(missile->x_current, missile->y_current,
                           missile->radius, CONFIG_COLOR_PLAYER);
      } else if (MISSILE_TYPE_ENEMY == missile->type) {
        display_fillCircle(missile->x_current, missile->y_current,
                           missile->radius, CONFIG_COLOR_ENEMY);
      } else if (MISSILE_TYPE_PLANE == missile->type) {
        display_fillCircle(missile->x_current, missile->y_current,
                           missile->radius, CONFIG_COLOR_PLANE);
      }
      missile->currentState = explode_shrink_st;
    } else {
      // Do not draw the circle but rather go to the dead state
      missile->currentState = dead_st;
    }
    break;

  // Stay in the dead_st until you are reinitialized
  case dead_st:
    missile->currentState = dead_st;
    break;
  default:
    printf("ERROR: Unaccounted state transition.\n");
  }

  // Handle state actions
  switch (missile->currentState) {
  case init_st:
    break;
  case moving_st:
    break;
  case explode_grow_st:
    break;
  case explode_shrink_st:
    break;
  case dead_st:
    break;
  default:
    printf("ERROR: Unaccounted state action.\n");
  }
}

// Returns whether the given missile is dead.
bool missile_is_dead(missile_t *missile) {
  return (dead_st == missile->currentState);
}

// Return whether the given missile is exploding.  This is needed when
// detecting whether a missile hits another exploding missile.
bool missile_is_exploding(missile_t *missile) {
  return (true == missile->explode_me);
}

// Return whether the given missile is flying.
bool missile_is_flying(missile_t *missile) {

  double percentage = missile->length / (double)missile->total_length;
  double epsilon = 0.01; // Might need to finagle this around

  if (fabs(percentage - 1) > epsilon) {
    // It has not reached its destination and it has not exploded
    return (false == missile->explode_me);
  }
  // The missile has reached the end of its life either by being blown to
  // smitherines or by reaching its final destination.
  return false;
}

// Used to indicate that a flying missile should be detonated.  This occurs
// when an enemy or plane missile is located within an explosion zone.
void missile_trigger_explosion(missile_t *missile) {
  missile->explode_me = true;
}