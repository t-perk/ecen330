#include <math.h>
#include <stdio.h>

#include "config.h"
#include "display.h"
#include "interrupts.h"
#include "intervalTimer.h"
#include "missile.h"
#include "plane.h"
#include "touchscreen.h"
#include <stdbool.h>

#include "gameControl.h"

// Delcare one big array for all the missiles
missile_t missiles[CONFIG_MAX_TOTAL_MISSILES]; // 11 total
// Make a pointer that stores all the enemy missiles at the beginning of the
// missiles array
missile_t *enemy_missiles = &(missiles[0]); // 0-6
// Make a pointer that stores all the player missiles after the end of the space
// allocated for the enemy missiles
missile_t *player_missiles = &(missiles[CONFIG_MAX_ENEMY_MISSILES]); // 7-10
// Make a pointer that stores the plane missile after the end of the space
// allocated for the plane missiles
missile_t *plane_missile = &(missiles[CONFIG_MAX_TOTAL_MISSILES - 1]); // 11

uint16_t impacted_cnt;
uint16_t shots_cnt;
uint16_t old_impacted_cnt;
uint16_t old_shots_cnt;

bool tickOddMissiles = true;

#define GAMECONTROL_SQUARE_POWER 2
#define STATS_TEXT_SIZE 2
#define TEXT_CURSOR_X 8
#define TEXT_CURSOR_Y 5

// State messages
#define INIT_ST_MSG "gameControl_init_st\n"
#define WAIT_TOUCH_ST_MSG "gameControl_wait_touch_st\n"
#define WAIT_RELEASE_ST_MSG "gameControl_wait_release_st\n"
#define RELEASED_ST_MSG "gameControl_released_st\n"

enum gameControl_st_t {
  init_st,
  wait_touch_st,
  wait_release_st,
  released_st,
};

static enum gameControl_st_t currentState;

// Draws or erases the stats given the state of bool draw
void drawStats_helper(bool draw) {
  printf("drawStats_helper: %d\n", draw);
  // Configure display text settings
  display_setTextColor(
      (draw) ? DISPLAY_WHITE : CONFIG_BACKGROUND_COLOR); // Make the text white.
  display_setTextSize(STATS_TEXT_SIZE);                  // Resize the text.

  char output_string[100];

  // Set the cursor location and print to the LCD
  display_setCursor(TEXT_CURSOR_X, TEXT_CURSOR_Y);
  sprintf(output_string, "Shot: %d   Impacted: %d\n",
          ((draw) ? shots_cnt : old_shots_cnt),
          ((draw) ? impacted_cnt : old_impacted_cnt));
  display_println(output_string);
}

// Initialize the game control logic
// This function will initialize all missiles, stats, plane, etc.
void gameControl_init() {

  display_fillScreen(CONFIG_BACKGROUND_COLOR);

  impacted_cnt = 0;
  shots_cnt = 0;
  old_impacted_cnt = 0;
  old_shots_cnt = 0;

  drawStats_helper(true);

  // Initialize missiles
  for (uint16_t i = 0; i < CONFIG_MAX_TOTAL_MISSILES; i++) {
    missile_init_dead(&missiles[i]);
  }

  plane_init(plane_missile);

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
    case released_st:
      printf(RELEASED_ST_MSG);
      break;
    default:
      printf("ERROR: Unaccounted gameControl state action.\n");
    }
  }
}

// Tick the game control logic
//
// This function should tick the missiles, handle screen touches, collisions,
// and updating statistics.
void gameControl_tick() {
  debugStatePrint_gameControl();
  // Do the "every tick" stuff

  // Tick every other missile

  for (uint16_t i = 0; i < CONFIG_MAX_TOTAL_MISSILES; i++) {
    missile_t *lastMissile = &missiles[i];
  }

  for (uint16_t i = (tickOddMissiles ? 0 : 1); i < CONFIG_MAX_TOTAL_MISSILES;
       i += 2) {
    missile_tick(&missiles[i]);
  }

  tickOddMissiles = !tickOddMissiles;

  // If enemy missile is dead, relaunch it (call init again)
  // Check for dead enemy missiles and re-initialize
  for (uint16_t i = 0; i < CONFIG_MAX_ENEMY_MISSILES; i++) {
    if (missile_is_dead(&enemy_missiles[i])) {
      missile_init_enemy(&enemy_missiles[i]);
    }
  }

  // Check if missile i should explode, caused by an exploding missile j

  for (uint16_t i = 0; i < CONFIG_MAX_ENEMY_MISSILES; i++) {
    int16_t enemyMissileLocation_x = missiles[i].x_current;
    int16_t enemyMissileLocation_y = missiles[i].y_current;

    for (uint16_t j = 0; j < CONFIG_MAX_TOTAL_MISSILES; j++) {
      if (!missile_is_flying(&missiles[i])) {
        continue;
      }
      if (!missile_is_exploding(&missiles[j])) {
        continue;
      }

      // At this point, we know that the other missile is exploding and we can
      // compare the location of the enemy missile with the radius of the
      // explosion from the other missile.

      int16_t otherMissileLocation_x = missiles[j].x_current;
      int16_t otherMissileLocation_y = missiles[j].y_current;
      int16_t otherMissileRadius = missiles[j].radius;

      bool isInRadius =
          (((otherMissileLocation_y - enemyMissileLocation_y) *
            (otherMissileLocation_y - enemyMissileLocation_y)) +
           abs(otherMissileLocation_x - enemyMissileLocation_x) *
               abs(otherMissileLocation_x - enemyMissileLocation_x)) <
          otherMissileRadius * otherMissileRadius;

      if (isInRadius) {
        printf("is in radius!\n\n");
        missile_trigger_explosion(&missiles[i]);
      }

      display_point_t planeLocation = plane_getXY();

      bool planeIsInRadius =
          (((otherMissileLocation_y - planeLocation.y) *
            (otherMissileLocation_y - planeLocation.y)) +
           abs(otherMissileLocation_x - planeLocation.x) *
               abs(otherMissileLocation_x - planeLocation.x)) <
          otherMissileRadius * otherMissileRadius;

      if (planeIsInRadius) {
        // Increment impact here?
        plane_explode();
      }
    }
  }

  // Tick the plane state machine
  plane_tick();

  for (uint16_t i = 0; i < CONFIG_MAX_TOTAL_MISSILES; i++) {
    if (missiles[i].impacted == true) {
      impacted_cnt++;
      missiles[i].impacted = false;
    }
  }

  // Update stats
  // IF there was a change in the stats go through the draw process.
  if (old_impacted_cnt != impacted_cnt || old_shots_cnt != shots_cnt) {
    // Erase the old values.
    drawStats_helper(false);
    // Update the old to the new values
    old_impacted_cnt = impacted_cnt;
    old_shots_cnt = shots_cnt;
    // Display the new values
    drawStats_helper(true);
  }

  // Used state machine logic to handle the touchscreen/player interaction.
  //  If touchscreen touched, launch player missile (if one is available)

  // Handle transitions
  switch (currentState) {
  case init_st:
    currentState = wait_touch_st;
    break;
  case wait_touch_st:
    if (touchscreen_get_status() == TOUCHSCREEN_PRESSED) {
      // Send to a state that waits for the player to let go of the
      // screen
      printf("pressed with state: %d\n", touchscreen_get_status());
      currentState = wait_release_st;
    } else {
      currentState = wait_touch_st;
    }
    break;
  case wait_release_st:
    // I was running into a weird issue where my touchscreen status was
    // skipping released and going straight to idle, so I just used this line
    // instead.
    if (touchscreen_get_status() != TOUCHSCREEN_PRESSED) {

      // Launch player missile (if one is available)

      // Find the first instance of a player missile that is dead. If you find
      // one, we can assume that there is an available missile to fire.
      printf("First player missile is: %d\n",
             missile_is_dead(&player_missiles[CONFIG_MAX_ENEMY_MISSILES]));
      for (uint16_t i = 0; i < CONFIG_MAX_PLAYER_MISSILES; i++) {
        printf("Yo: %d\n", &player_missiles[i]);
        if (missile_is_dead(&player_missiles[i])) {
          display_point_t touchPoint = touchscreen_get_location();
          printf("Initializing missile at: %d, %d\n", touchPoint.x,
                 touchPoint.y);
          missile_init_player(&player_missiles[i], touchPoint.x, touchPoint.y);
          shots_cnt++; // Increment the number of shots that the player has
                       // taken.
          break;
        }
      }
      touchscreen_ack_touch();

      currentState = wait_touch_st;

    } else {
      currentState = wait_release_st;
    }
    break;
  case released_st:
    currentState = wait_touch_st;
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