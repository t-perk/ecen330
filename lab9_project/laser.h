#ifndef LASER
#define LASER

#define MAX_QUEUE_SIZE 10

#include <stdbool.h>
#include <stdint.h>
#include "display.h"

// Create a struct to contain all the points in a laser.
typedef struct {
  display_point_t queue[MAX_QUEUE_SIZE];
  int front;
  int rear;
  int size;
} Queue;

/* This struct contains all information about a laser */
typedef struct {
  // Current state (the 'enum' will be defined in your laser.c file, so it's
  // just declared as an integer type here)
  int32_t currentState;

  // Starting x,y of laser
  uint16_t x_origin;
  uint16_t y_origin;

  // Ending x,y of laser, and the total length from origin to destination.
  uint16_t x_dest;
  uint16_t y_dest;
  uint16_t total_length;

  // Used to track the point x,y of laser
  int16_t x_point;
  int16_t y_point;

  // Used to track the tail x,y of laser
  int16_t x_tail;
  int16_t y_tail;

// Used to determine whether the laser is starting from the top or bottom
  bool up_down;

  // While traveling, this tracks the current length of the flight path
  double length;

  //Changing value based on the score
  double speed;

  // Declare the queue
  Queue laserQueue;

} laser_t;

////////// State Machine INIT Functions //////////

// Initialize the laser as a dead laser.
void laser_init_dead(laser_t *laser);

// Initialize the laser.  This will randomly choose the origin and destination of the laser.
void laser_init_active(laser_t *laser, double speedVal);

////////// State Machine TICK Function //////////
void laser_tick(laser_t *laser);

// Return whether the given laser is dead.
bool laser_is_dead(laser_t *laser);

// Return whether the given laser is flying.
bool laser_is_traveling(laser_t *laser);

void incrementSpeed();

#endif /* LASER */