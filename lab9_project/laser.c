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

#define MAX_QUEUE_SIZE 10

enum laser_st_t {
  init_st,   // Starts here. Will feed into the moving state
  moving_st, // laser traveling
  dead_st,   // signals that this laser can be reinitialized as a new laser
};

// Place a coordinate on the queue
void on_queue(laser_t *laser, display_point_t point) {
  // printf("Point placed on the queue\n");
  if (laser->laserQueue.size == MAX_QUEUE_SIZE) {
    printf("Queue is full. Cannot enqueue.\n");
    return;
  }

  // Moves the rear one step back
  laser->laserQueue.rear = (laser->laserQueue.rear + 1) % MAX_QUEUE_SIZE;
  // Sets the end of the queue equal to our point
  laser->laserQueue.queue[laser->laserQueue.rear] = point;
  // Increment size
  laser->laserQueue.size++;
}

display_point_t off_queue(laser_t *laser) {
  if (laser->laserQueue.size == 0) {
    printf("Queue is empty. Cannot dequeue.\n");
    display_point_t empty_point = {0, 0};
    return empty_point;
  }

  display_point_t point = laser->laserQueue.queue[laser->laserQueue.front];
  laser->laserQueue.front = (laser->laserQueue.front + 1) % MAX_QUEUE_SIZE;
  laser->laserQueue.size--;

  return point;
}

////////// State Machine INIT Functions //////////

// Initialize the laser as a dead laser.
void laser_init_dead(laser_t *laser) {}

// Initialize the laser.  This will randomly choose the origin and destination
// of the laser.
void laser_init_active(laser_t *laser, double speedVal) {
  laser->length = 0;
  laser->speed = speedVal;

  // Choose which of the four sides the lase

  // TODO: Strech goal would be to add capability for lasers to spawn on all
  // four sides
  laser->up_down = rand() % ONE_OR_ZERO;

  uint16_t rand_y_origin = ((laser->up_down) ? 0 : DISPLAY_HEIGHT);
  uint16_t rand_x_origin = rand() % DISPLAY_WIDTH;

  laser->x_origin = rand_x_origin;
  laser->y_origin = rand_y_origin;

  // printf("up_down is at: %d\n", laser->up_down);
  // printf("Displaying laser origin: (%d, %d)\n", rand_x_origin,
  // rand_y_origin);

  // Set the laser destination to some point on the opposite side
  uint16_t rand_y_dest = ((laser->up_down) ? DISPLAY_HEIGHT : 0);
  uint16_t rand_x_dest = rand() % DISPLAY_WIDTH;

  laser->x_dest = rand_x_dest;
  laser->y_dest = rand_y_dest;

  // printf("up_down is at: %d\n", laser->up_down);
  // printf("Displaying laser dest: (%d, %d)\n", rand_x_dest, rand_y_dest);

  // Update total_length
  laser->total_length =
      (int)sqrt(pow((laser->y_dest - laser->y_origin), LASER_SQUARE_POWER) +
                pow((laser->x_dest - laser->x_origin), LASER_SQUARE_POWER));
  // printf("total length is %d\n", laser->total_length);

  laser->x_point = laser->x_origin;
  laser->y_point = laser->y_origin;

  laser->x_tail = laser->x_origin;
  laser->y_tail = laser->y_origin;

  // Initialize the queue

  // First point in the queue
  laser->laserQueue.front = 0;
  // Last point in the queue
  laser->laserQueue.rear = -1;
  // number of points
  laser->laserQueue.size = CONFIG_LASER_SIZE;

  display_point_t point1 = {laser->x_point, laser->y_point};

  // Fill the queue with the points of origin
  for (uint8_t i = 0; i < CONFIG_LASER_SIZE; i++) {
    on_queue(laser, point1);
  }

  laser->currentState = init_st;
}

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
  debugStatePrint_Laser(laser);

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
    // x_current = x_origin + percentage * (x_dest – x_origin)
    double percentage = laser->length / (double)laser->total_length;
    double epsilon = 0.01; // Might need to finagle this around

    // Check to see if the missle has traveled far enough. If so, don't draw a
    // new one, but instead transition to another state

    if (laser->length > laser->total_length) {

      // TODO for some reason I was getting a bug where my lasers starting from
      // the top going down would despawn before reaching the end.
      // if (laser->length > laser->total_length ||
      //     ((laser->up_down) ? (laser->length >= DISPLAY_HEIGHT)
      //                       : (laser->length <= 0))) {

      printf("Traveling complete. length: %d\ntotal_length: %d\n",
             laser->length, laser->total_length);
      laser->currentState = dead_st;

    } else {
      // Not close enough, so go ahead and draw the line and stay in the same
      // state.

      laser->x_point =
          laser->x_origin + percentage * (laser->x_dest - laser->x_origin);
      laser->y_point =
          laser->y_origin + percentage * (laser->y_dest - laser->y_origin);

      display_point_t point1 = {laser->x_point, laser->y_point};

      if (laser->laserQueue.size == MAX_QUEUE_SIZE) {
        printf("Oh dang it was full\n");
        // Need to pop before we add another one.
        off_queue(laser);
        on_queue(laser, point1);
      }

      display_point_t tailPoint =
          laser->laserQueue.queue[laser->laserQueue.front];
      laser->x_tail = tailPoint.x;
      laser->y_tail = tailPoint.y;

      // Draw the new line
      display_drawLine(laser->x_tail, laser->y_tail, laser->x_point,
                       laser->y_point, CONFIG_COLOR_LASER);
      printf("Drawing new line from (%d,%d) to (%d, %d)\n", laser->x_tail,
             laser->y_tail, laser->x_point, laser->y_point);

      laser->currentState = moving_st;
    }

    // Keeping track of all my lasers.
    /*
    Keep an array of points of length x.
    Each time the point of the laser is incremented, add a position entry to the
    array. When drawing the line, draw it between the first and last points in
    the array- the last point being the tail

    When checking for collision, see if the player has intersected with every
    nth point of the posiitons array

    Ask copilot "How do I make a queue to store 10 display_point_t structs?"

    */

    break;

    // Stay in the dead_st until you are reinitialized
  case dead_st:
    laser->currentState = dead_st;
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

// Return whether the given laser is flying.
bool laser_is_traveling(laser_t *laser) {}