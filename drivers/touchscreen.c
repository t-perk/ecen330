/*
Written by Tyler Perkins
September 29, 2023

*/

#include "touchscreen.h"
#include "xparameters.h"
#include "xil_io.h"
#include "stdio.h"
#include "display.h"

#define ADC_SETTLE_TIME .05 //50ms needed

// All printed messages for states are provided here.
#define INIT_ST_MSG "init_st\n"
#define WAITING_ST_MSG "waiting_st\n"
#define ADC_SETTLING_ST_MSG "adc_settling_st\n"
#define PRESSED_ST_MSG "pressed_st\n"

uint8_t pressed = 0;
uint32_t adc_timer = 0;
uint32_t adc_settle_ticks = 0;

//touchscreen touch location variables
int16_t x, y;
uint8_t z;

enum touchControl_st_t {
    init_st,        //Starts here. Will feed into the Waiting state
    waiting_st,     //Remains in this state until display is touched
    adc_settling_st,//Remains in state until we're able to register where the user touched (after adc_settle_ticks)
    pressed_st      //Remains in state until the display is no longer being touched
};

//Define state types
static enum touchControl_st_t currentState;
touchscreen_status_t currentTouchscreenStatus;

// Initialize the touchscreen driver state machine, with a given tick period (in
// seconds).
void touchscreen_init(double period_seconds){

    //Lab 5:
    //There needs to be at least a 50ms settle time so we could divide
    //the 50ms by the period_seconds to get the # of ticks we need to reach
    //at least 50ms
    adc_settle_ticks = ADC_SETTLE_TIME / period_seconds;
    currentState = init_st;
}

// This is a debug state print routine. It will print the names of the states each
// time tick() is called. It only prints states if they are different than the
// previous state.
void debugStatePrint() {
  static enum touchControl_st_t previousState;
  static bool firstPass = true;
  // Only print the message if:
  // 1. This the first pass and the value for previousState is unknown.
  // 2. previousState != currentState - this prevents reprinting the same state name over and over.
  if (previousState != currentState || firstPass) {
    firstPass = false;                // previousState will be defined, firstPass is false.
    previousState = currentState;     // keep track of the last state that you were in.
    switch(currentState) {            // This prints messages based upon the state that you were in.
      case init_st:
        printf(INIT_ST_MSG);
        break;
      case waiting_st:
        printf(WAITING_ST_MSG);
        break;
      case adc_settling_st:
        printf(ADC_SETTLING_ST_MSG);
        break;
      case pressed_st:
        printf(PRESSED_ST_MSG);
        break;
     }
  }
}

// Tick the touchscreen driver state machine
void touchscreen_tick(){
    //Debug function
    debugStatePrint();
        
        //Handle transitions
        switch(currentState){

            //Initialize variables
            case init_st:
                currentState = waiting_st;
                currentTouchscreenStatus = TOUCHSCREEN_IDLE;
                break;
            //If display touched, clear old touch data and move to adc_settling_st
            case waiting_st:

                if (!display_isTouched()){
                    currentState = waiting_st;
                    currentTouchscreenStatus = TOUCHSCREEN_IDLE;
                }
                else if(display_isTouched()){
                    display_clearOldTouchData();
                    currentState = adc_settling_st;
                }
                break;
            //If display is touched & sufficient time has been given to the 
            //system to register the touch, get the data from the touchscreen
            //to know where the screen was touched.
            //If display is not touched, go back to the waiting_st
            case adc_settling_st:

                if (display_isTouched() && adc_timer == adc_settle_ticks){
                    display_getTouchedPoint(&x, &y, &z);
                    currentState = pressed_st;
                    //system recognizes the touch
                    currentTouchscreenStatus = TOUCHSCREEN_PRESSED;
                } else if (!display_isTouched()){
                    currentState = waiting_st;
                    //It never fully registered the touch, so send it back to idle
                    currentTouchscreenStatus = TOUCHSCREEN_IDLE;
                }
                break;
            //If display not touched, return to waiting_st &
            //set status of touchscreen to be released
            case pressed_st:

                if (!display_isTouched()){
                    currentState = waiting_st;
                    currentTouchscreenStatus = TOUCHSCREEN_RELEASED;
                }
            default:
                //Remaining in the current state
                currentState = currentState;
                break;
        }

    //Handle state actions
    switch(currentState){
        case init_st:
            break;
        case waiting_st:
            adc_timer = 0;//This may make the init state redundant.
            break;
        case adc_settling_st:
            adc_timer++;//Increment adc_timer
            break;
        case pressed_st:
            pressed = true;//DEFINE
            break;
        default:
            printf("ERROR: Unaccounted state action.\n");
    }
}

// Return the current status of the touchscreen
touchscreen_status_t touchscreen_get_status(){
    return currentTouchscreenStatus;
}

// Acknowledge the touchscreen touch. This function will only have effect when
// the touchscreen is in the TOUCHSCREEN_RELEASED status, and will cause it to
// switch to the TOUCHSCREEN_IDLE status.
void touchscreen_ack_touch(){
    pressed = false;
}

// Get the (x,y) location of the last touchscreen touch
// Return struct display_point_t which has properties .x and .y
display_point_t touchscreen_get_location(){
    //Create new display point and set its x and y vaulues
    display_point_t touchscreenLocation;
    touchscreenLocation.x = x;
    touchscreenLocation.y = y;
    return touchscreenLocation;
}