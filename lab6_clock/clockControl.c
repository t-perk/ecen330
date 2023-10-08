#include "clockControl.h"
#include "clockDisplay.h"
#include "touchscreen.h"
#include "xparameters.h"
#include "xil_io.h"

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

//If you hold down the arrows for over 0.5 seconds, the clock will enter a 
//fast-update mode
#define FAST_UPDATE_TIME 0.5
#define UPDATE_NUM_TICKS_TIME 0.1

// All printed messages for states are provided here.
#define INIT_ST_MSG "clock_init_st\n"
#define WAITING_ST_MSG "clock_waiting_st\n"
#define LONG_PRESS_DELAY_ST_MSG "clock_long_press_delay_st\n"
#define INC_DEC_ST_MSG "clock_inc_dec_st\n"
#define FAST_UPDATE_ST_MSG "clock_fast_update_st\n"

uint32_t delay_cnt = 0;
uint32_t update_cnt = 0;
uint32_t delay_num_ticks = 0;
uint32_t update_num_ticks = 0;

volatile static display_point_t point1;

enum clockControl_st_t {
    init_st,        //Starts here. Will feed into the Waiting state
    waiting_st,     //Remains in this state until display is touched
    long_press_delay_st,//Remains in state until we're able to register where the user touched long enough
    inc_dec_st,      //Performs the increment or decrement, given a touched point.
    fast_update_st  //updates clock at 10x speed
};

//Define currentState var
static enum clockControl_st_t currentState;

// Initialize the clock control state machine, with a given period in seconds.
void clockControl_init(double period_s){

    //Set current state
    currentState = init_st;

    //Initialize values
    delay_cnt = 0;
    update_cnt = 0;

    //Setup number of ticks needed to enter super speedy fast update mode.
    //Vrooooooooooom
    delay_num_ticks = FAST_UPDATE_TIME / period_s;
    update_num_ticks = UPDATE_NUM_TICKS_TIME / period_s;
}

// This is a debug state print routine. It will print the names of the states each
// time tick() is called. It only prints states if they are different than the
// previous state.
void debugStatePrint_clockControl() {
  static enum clockControl_st_t previousState;
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
        case long_press_delay_st:
            printf(LONG_PRESS_DELAY_ST_MSG);
            break;
        case inc_dec_st:
            printf(INC_DEC_ST_MSG);
            break;
        case fast_update_st:
            printf(FAST_UPDATE_ST_MSG);
            break;
     }
  }
}

// Tick the clock control state machine
void clockControl_tick(){
    //Debug function
    debugStatePrint_clockControl();

    //Handle transitions
    switch(currentState){

        //Move into waiting state. Initialize variables as needed
        case init_st:
            currentState = waiting_st;
            break;
        //If display touched, clear the delay_cnt and move into state to check 
        //how long the button is being held down for. Otherwise, move to the 
        //inc_dec_st to update the clock
        case waiting_st:
            if (touchscreen_get_status() == TOUCHSCREEN_RELEASED){
                currentState = inc_dec_st;
            }
            else if(touchscreen_get_status() == TOUCHSCREEN_PRESSED){
                delay_cnt = 0;
                currentState = long_press_delay_st;
            }
            break;
        //If button is held long enough, enter fast update mode
        //If button is released, moved to inc_dec_st
        //otherwise keep waiting
        case long_press_delay_st:
            if (touchscreen_get_status() == TOUCHSCREEN_RELEASED){
                currentState = inc_dec_st;
            }
            else if (delay_cnt == delay_num_ticks){
                update_cnt = 0;
                currentState = fast_update_st;
            }
            else
            {
                currentState = long_press_delay_st;
            }
            break;

        case fast_update_st:
            if (!(touchscreen_get_status() == TOUCHSCREEN_RELEASED) && (update_cnt == update_num_ticks)){
                update_cnt = 0;
                clockDisplay_performIncDec(touchscreen_get_location());
                currentState = fast_update_st;
            }
            else if (touchscreen_get_status() == TOUCHSCREEN_RELEASED){
                touchscreen_ack_touch();
                currentState = waiting_st;
            }
            else
            {
                currentState = fast_update_st;
            }
            break;

        //Note: I changed this part slightly from the given state machine
        //because I saw that touchscreen status was dipping into idle after
        //being released for a second. This likely has something to do with
        //how I setup my touchscreen driver code.
        case inc_dec_st:
            if (!(touchscreen_get_status() == TOUCHSCREEN_PRESSED)){
                touchscreen_ack_touch();
                currentState = waiting_st;
            }
            break;
        default:
            printf("ERROR: Unaccounted state transition.\n");
    }

    //Handle state actions
    switch (currentState) {
        case init_st:
            break;
        case waiting_st:
            break;
        case long_press_delay_st:
            delay_cnt++;
            break;
        case fast_update_st:
            update_cnt++;
            break;
        case inc_dec_st:
            clockDisplay_performIncDec(touchscreen_get_location());
            break;
        default:
            printf("ERROR: Unaccounted state action.\n");
    }
}