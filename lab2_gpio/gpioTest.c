#include "display.h"
#include "stdio.h"
#include "buttons.h"
#include "switches.h"
#include "gpioTest.h"
#include "leds.h"

//Blue rectangle defines
#define BLUE_TOP_LEFT_COORD_X 0
#define BLUE_TOP_LEFT_COORD_Y 0

//Orange rectangle defines
#define RED_TOP_LEFT_COORD_X 80
#define RED_TOP_LEFT_COORD_Y 0

//Green rectangle defines
#define GREEN_TOP_LEFT_COORD_X 160
#define GREEN_TOP_LEFT_COORD_Y 0

//White rectangle defines
#define WHITE_TOP_LEFT_COORD_X 240
#define WHITE_TOP_LEFT_COORD_Y 0

//Button size defines
#define RECT_WIDTH 80
#define RECT_HEIGHT 120

//Text size defines
#define TEXT_SIZE 2
#define CURSOR_X_OFFSET RECT_WIDTH/4
#define CURSOR_Y_OFFSET RECT_HEIGHT/4

#define ALL_ON 0xF

#define TRUE 1
#define FALSE 0

#define UINT8_T_ALL_OFF 0x00

uint8_t drawButtons(uint8_t buttons_drawn);

// Runs a test of the buttons. As you push the buttons, graphics and messages
// will be written to the LCD panel. The test will run until all 4 pushbuttons 
// are simultaneously pressed.
void gpioTest_buttons(){
    buttons_init();

    //Initialize text color for the buttons (Black)
    display_setTextColor(DISPLAY_BLACK);
    display_setTextSize(TEXT_SIZE);

    //Create a display the corresponds with the buttons that have are being
    //pressed
    display_init();
    display_fillScreen(DISPLAY_BLACK);

    uint8_t buttons_drawn = UINT8_T_ALL_OFF;

    //Setup text
    //Blue square text
    display_setCursor(BLUE_TOP_LEFT_COORD_X + CURSOR_X_OFFSET, BLUE_TOP_LEFT_COORD_Y + CURSOR_Y_OFFSET);
    display_println("BTN3");

    //Red square text
    display_setCursor(RED_TOP_LEFT_COORD_X + CURSOR_X_OFFSET, RED_TOP_LEFT_COORD_Y + CURSOR_Y_OFFSET);
    display_println("BTN2");

    //Green square text
    display_setCursor(GREEN_TOP_LEFT_COORD_X + CURSOR_X_OFFSET, GREEN_TOP_LEFT_COORD_Y + CURSOR_Y_OFFSET);
    display_println("BTN1");

    //White square text
    display_setCursor(WHITE_TOP_LEFT_COORD_X + CURSOR_X_OFFSET, WHITE_TOP_LEFT_COORD_Y + CURSOR_Y_OFFSET);
    display_println("BTN0");

    // //Display screen size on the LCD is 320x240

    //repeat loop to check state while buttons are not all pressed
    while (buttons_read() != ALL_ON){
        buttons_drawn = drawButtons(buttons_drawn);
    }

    //Catches the case where all buttons are pressed at the same time
    buttons_drawn = drawButtons(buttons_drawn);
}

//Draws the buttons on the screen
//returns the buttons_drawn value to be updated in the gpioTest_buttons 
//function.
uint8_t drawButtons(uint8_t buttons_drawn){
    //Apply masks to see if each button is pressed.

        //See if the first bit from buttons_read() is 1
        if (buttons_read() & BUTTONS_BTN0_MASK){
            if (!(buttons_drawn & BUTTONS_BTN0_MASK)){
                //Update buttons_drawn bit 0 to be 1
                buttons_drawn = buttons_drawn | BUTTONS_BTN0_MASK;
                display_fillRect(WHITE_TOP_LEFT_COORD_X,WHITE_TOP_LEFT_COORD_Y,RECT_WIDTH,RECT_HEIGHT,DISPLAY_WHITE);
                display_setCursor(WHITE_TOP_LEFT_COORD_X + CURSOR_X_OFFSET, WHITE_TOP_LEFT_COORD_Y + CURSOR_Y_OFFSET);
                display_println("BTN0");
            }
        }
        else//Take away the white square if BTN0 is not pressed
        {
            //Update buttons_drawn bit 0 to be 0
            buttons_drawn = buttons_drawn & ~BUTTONS_BTN0_MASK;
            display_fillRect(WHITE_TOP_LEFT_COORD_X,WHITE_TOP_LEFT_COORD_Y,RECT_WIDTH,RECT_HEIGHT,DISPLAY_BLACK);
        }
        
        //See if the second bit from buttons_read() is 1
        if (buttons_read() & BUTTONS_BTN1_MASK){
            if (!(buttons_drawn & BUTTONS_BTN1_MASK)){
                //Update buttons_drawn bit 0 to be 1
                buttons_drawn = buttons_drawn | BUTTONS_BTN1_MASK;
                display_fillRect(GREEN_TOP_LEFT_COORD_X,GREEN_TOP_LEFT_COORD_Y,RECT_WIDTH,RECT_HEIGHT,DISPLAY_GREEN);
                display_setCursor(GREEN_TOP_LEFT_COORD_X + CURSOR_X_OFFSET, GREEN_TOP_LEFT_COORD_Y + CURSOR_Y_OFFSET);
                display_println("BTN1");
            }
        }
        else//Take away the green square if BTN1 is not pressed
        {
            //Update buttons_drawn bit 0 to be 0
            buttons_drawn = buttons_drawn & ~BUTTONS_BTN1_MASK;
            display_fillRect(GREEN_TOP_LEFT_COORD_X,GREEN_TOP_LEFT_COORD_Y,RECT_WIDTH,RECT_HEIGHT,DISPLAY_BLACK);
        }
        
        //See if the third bit from buttons_read() is 1
        if (buttons_read() & BUTTONS_BTN2_MASK){
            if (!(buttons_drawn & BUTTONS_BTN2_MASK)){
                //Update buttons_drawn bit 0 to be 1
                buttons_drawn = buttons_drawn | BUTTONS_BTN2_MASK;
                display_fillRect(RED_TOP_LEFT_COORD_X,RED_TOP_LEFT_COORD_Y,RECT_WIDTH,RECT_HEIGHT,DISPLAY_RED);
                display_setCursor(RED_TOP_LEFT_COORD_X + CURSOR_X_OFFSET, RED_TOP_LEFT_COORD_Y + CURSOR_Y_OFFSET);
                display_println("BTN2");
            }
        }
        else//Take away the red square if BTN2 is not pressed
        {
            //Update buttons_drawn bit 0 to be 0
            buttons_drawn = buttons_drawn & ~BUTTONS_BTN2_MASK;
            display_fillRect(RED_TOP_LEFT_COORD_X,RED_TOP_LEFT_COORD_Y,RECT_WIDTH,RECT_HEIGHT,DISPLAY_BLACK);
        }

        //See if the fourth bit from buttons_read() is 1
        if (buttons_read() & BUTTONS_BTN3_MASK){
            if (!(buttons_drawn & BUTTONS_BTN3_MASK)){
                //Update buttons_drawn bit 0 to be 1
                buttons_drawn = buttons_drawn | BUTTONS_BTN3_MASK;
                display_fillRect(BLUE_TOP_LEFT_COORD_X,BLUE_TOP_LEFT_COORD_Y,RECT_WIDTH,RECT_HEIGHT,DISPLAY_BLUE);
                display_setCursor(BLUE_TOP_LEFT_COORD_X + CURSOR_X_OFFSET, BLUE_TOP_LEFT_COORD_Y + CURSOR_Y_OFFSET);
                display_println("BTN3");
            }
        }
        else//Take away the blue square if BTN3 is not pressed
        {
            //Update buttons_drawn bit 0 to be 0
            buttons_drawn = buttons_drawn & ~BUTTONS_BTN3_MASK;
            display_fillRect(BLUE_TOP_LEFT_COORD_X,BLUE_TOP_LEFT_COORD_Y,RECT_WIDTH,RECT_HEIGHT,DISPLAY_BLACK);
        }

    return buttons_drawn;
}

// Runs a test of the switches. As you slide the switches, LEDs directly above
// the switches will illuminate. The test will run until all switches are slid
// upwards. When all 4 slide switches are slid upward, this function will
// return.
void gpioTest_switches(){
    switches_init();
    int32_t returnValue = leds_init();

    uint8_t lastSwitchPass = FALSE;

    //While all switches are not slid up, keep checking the state of the
    //switches.
    while (switches_read() != ALL_ON && !lastSwitchPass){

        leds_write(switches_read());
    }
    leds_write(switches_read());
}