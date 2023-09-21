/*
This software is provided for student assignment use in the Department of
Electrical and Computer Engineering, Brigham Young University, Utah, USA.

Users agree to not re-host, or redistribute the software, in source or binary
form, to other persons or other institutions. Users may modify and use the
source code for personal or educational use.

For questions, contact Brad Hutchings or Jeff Goeders, https://ece.byu.edu/
*/

#ifndef CLOCKDISPLAY
#define CLOCKDISPLAY

#include "display.h"

// Initialize clock display
void clockDisplay_init();

// Performs the increment or decrement, given a touched point.
void clockDisplay_performIncDec(display_point_t point);

// Advances the time forward by 1 second and update the display.
void clockDisplay_advanceTimeOneSecond();

#endif /* CLOCKDISPLAY */
