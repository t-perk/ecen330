/*
This software is provided for student assignment use in the Department of
Electrical and Computer Engineering, Brigham Young University, Utah, USA.

Users agree to not re-host, or redistribute the software, in source or binary
form, to other persons or other institutions. Users may modify and use the
source code for personal or educational use.

For questions, contact Brad Hutchings or Jeff Goeders, https://ece.byu.edu/
*/

#include "drivers/buttons.h"
#include "drivers/switches.h"

#include "gpioTest.h"

// Run switches and buttons runTest functions
int main() {
  gpioTest_switches();
  gpioTest_buttons();
  return 0;
}
