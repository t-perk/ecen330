#include "interrupt_test.h"
#include "interrupts.h"
#include "intervalTimer.h"
#include "leds.h"
#include "stdio.h"


#define TEN_HZ_INTERRUPT .1 //Should fire every .1 second
#define ONE_HZ_INTERRUPT 1  //Should fire every 1 second
#define TENTH_HZ_INTERRUPT 10//Should fire every 10 seconds

#define LED_0_BIT 0x01
#define LED_1_BIT 0x02
#define LED_2_BIT 0x04

//toggle LED_BIT_0 every INTERVAL_TIMER_0_INTERRUPT_IRQ
static void timer0_isr() {
    // Acknowledge timer interrupt
    intervalTimer_ackInterrupt(INTERVAL_TIMER_0_INTERRUPT_IRQ);

    //Flips the bit with the xor bit operator
    leds_write(leds_read()^LED_0_BIT);
}

//toggle LED_BIT_1 every INTERVAL_TIMER_1_INTERRUPT_IRQ
static void timer1_isr() {
    // Acknowledge timer interrupt
    intervalTimer_ackInterrupt(INTERVAL_TIMER_1_INTERRUPT_IRQ);

    //Flips the bit with the xor bit operator
    leds_write(leds_read()^LED_1_BIT);
}

//toggle LED_BIT_2 every INTERVAL_TIMER_2_INTERRUPT_IRQ
static void timer2_isr() {
    // Acknowledge timer interrupt
    intervalTimer_ackInterrupt(INTERVAL_TIMER_2_INTERRUPT_IRQ);

    //Flips the bit with the xor bit operator
    leds_write(leds_read()^LED_2_BIT);
}

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
void interrupt_test_run(){

    interrupts_init();
    leds_init();

    //Note that there was previously an issue here where  the 
    //interrupts_irq_enable command was overwriting previous enables 
    //(turning off other interupts) which was causing them to stop working.

    //Initialize Timer 0 - 10Hz (.1 seconds)
    interrupts_register(INTERVAL_TIMER_0_INTERRUPT_IRQ, timer0_isr);
    interrupts_irq_enable(INTERVAL_TIMER_0_INTERRUPT_IRQ);

    intervalTimer_initCountDown(INTERVAL_TIMER_0, TEN_HZ_INTERRUPT);
    intervalTimer_enableInterrupt(INTERVAL_TIMER_0);
    intervalTimer_start(INTERVAL_TIMER_0);
    

    //Initialize Timer 1 - 1Hz (1 second)
    interrupts_register(INTERVAL_TIMER_1_INTERRUPT_IRQ, timer1_isr);
    interrupts_irq_enable(INTERVAL_TIMER_1_INTERRUPT_IRQ);

    intervalTimer_initCountDown(INTERVAL_TIMER_1, ONE_HZ_INTERRUPT);
    intervalTimer_enableInterrupt(INTERVAL_TIMER_1);
    intervalTimer_start(INTERVAL_TIMER_1);

    //Initialize Timer 2 - .1Hz (10 second delay)
    interrupts_register(INTERVAL_TIMER_2_INTERRUPT_IRQ, timer2_isr);
    interrupts_irq_enable(INTERVAL_TIMER_2_INTERRUPT_IRQ);

    intervalTimer_initCountDown(INTERVAL_TIMER_2, TENTH_HZ_INTERRUPT);
    intervalTimer_enableInterrupt(INTERVAL_TIMER_2);
    intervalTimer_start(INTERVAL_TIMER_2);

    while (1)
    ;
}