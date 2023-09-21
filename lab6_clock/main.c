/*
This software is provided for student assignment use in the Department of
Electrical and Computer Engineering, Brigham Young University, Utah, USA.

Users agree to not re-host, or redistribute the software, in source or binary
form, to other persons or other institutions. Users may modify and use the
source code for personal or educational use.

For questions, contact Brad Hutchings or Jeff Goeders, https://ece.byu.edu/
*/

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

#include "clockControl.h"
#include "clockDisplay.h"
#include "interrupts.h"
#include "intervalTimer.h"
#include "touchscreen.h"

#define TICK_PERIOD 50E-3
#define ONE_SECOND 1.0

// Interrupt function for state machine ticks
void isr() {
  intervalTimer_ackInterrupt(INTERVAL_TIMER_0);
  clockControl_tick();
  touchscreen_tick();
}

// Interrupt function triggered every second to update the time
void isr_1s() {
  intervalTimer_ackInterrupt(INTERVAL_TIMER_1);

  touchscreen_status_t ts_status = touchscreen_get_status();
  if (ts_status != TOUCHSCREEN_PRESSED)
    clockDisplay_advanceTimeOneSecond();
}

// Main clock test application
int main() {
  printf("Starting clock application\n");

  // Initialize state machines and other modules
  clockDisplay_init();
  clockControl_init(TICK_PERIOD);
  touchscreen_init(TICK_PERIOD);

  // Set up two timer interrupts:
  // Timer 0: Tick timer
  // Timer 1: One second interrupt to advance clock
  interrupts_init();
  interrupts_register(INTERVAL_TIMER_0_INTERRUPT_IRQ, isr);
  interrupts_register(INTERVAL_TIMER_1_INTERRUPT_IRQ, isr_1s);
  interrupts_irq_enable(INTERVAL_TIMER_0_INTERRUPT_IRQ);
  interrupts_irq_enable(INTERVAL_TIMER_1_INTERRUPT_IRQ);

  // Configure the Timers
  intervalTimer_initCountDown(INTERVAL_TIMER_0, TICK_PERIOD);
  intervalTimer_initCountDown(INTERVAL_TIMER_1, ONE_SECOND);

  intervalTimer_enableInterrupt(INTERVAL_TIMER_0);
  intervalTimer_enableInterrupt(INTERVAL_TIMER_1);

  intervalTimer_start(INTERVAL_TIMER_0);
  intervalTimer_start(INTERVAL_TIMER_1);

  while (1)
    ;
  return 0;
}
