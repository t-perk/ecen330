#ifndef INTERRUPT_TEST
#define INTERRUPT_TEST

/*
This function is a small test application of your interrupt controller.  The
goal is to use the three AXI Interval Timers to generate interrupts at different
rates (10Hz, 1Hz, 0.1Hz), and create interrupt handler functions that change the
LEDs at this rate.  For example, the 1Hz interrupt will flip an LED value each
second, resulting in LED that turns on for 1 second, off for 1 second,
repeatedly.

For each interval timer:
    1. Initialize it as a count down timer with appropriate period.
    2. Enable the timer's interrupt output.
    3. Enable the associated interrupt input on the interrupt controller.
    4. Register an appropriate interrupt handler function (isr_timer0,
isr_timer1, isr_timer2).
    5. Start the timer.

Make sure you call `interrupts_init()` first!
*/
void interrupt_test_run();

#endif /* INTERRUPT_TEST */
