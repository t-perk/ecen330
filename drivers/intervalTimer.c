#include "intervalTimer.h"
#include "xparameters.h"
#include "xil_io.h"
#include "stdio.h"

#define TIMER_ZERO 0
#define TIMER_ONE 1
#define TIMER_TWO 2
#define CASC_BIT_OFFSET 0x800
#define UDT0_BIT_OFFSET 0x2
#define ENABLE_TIMER_BIT 0x80
#define LOAD_BIT 0x20

#define TLR0_OFFSET 0x04
#define TLR1_OFFSET 0x14

#define TCR0_OFFSET 0x08
#define TCR1_OFFSET 0x18

#define ARHT_OFFSET 0x10

#define ENIT_OFFSET 0x40

#define TOINT_OFFSET 0x100

#define SHIFT_32 32

//helper function for reading from device registers
//uint32_t returned value contains data in register.
static uint32_t readRegister(uint8_t timerNumber, uint32_t offset);

//helper function for writing to device registers
static void writeRegister(uint8_t timerNumber, uint32_t offset, uint32_t value);

// You must configure the interval timer before you use it:
// 1. Set the Timer Control/Status Registers such that:
//  - The timer is in 64-bit cascade mode
//  - The timer counts up
// 2. Initialize both LOAD registers with zeros
// 3. Call the _reload function to move the LOAD values into the Counters
void intervalTimer_initCountUp(uint32_t timerNumber){
    //Step 1
    writeRegister(timerNumber,0,0);//write a 0 to the TCSR0 register.
    //Not messing with TCSR1 because... it doesn't apply in cascade mode?
    //“TCSR1 register is used only for loading the TLR1 register in cascade mode.”

    //Step 1 - Create a filter that sets CASC bit = 1(Cascade enabled) & UDT0 bit = 0(Count up).
    //Data to write
    uint32_t timerData = readRegister(timerNumber,0);

    //set the CASC bit to 1 and the UDT0 bit to 0 in the TCSR0 
    //register (cascade mode and up counting).
    timerData = timerData | CASC_BIT_OFFSET;
    timerData = timerData & ~(UDT0_BIT_OFFSET);
    timerData = timerData & ~(ARHT_OFFSET);//Make sure the ARHT is off since we

    writeRegister(timerNumber,0,timerData);

    //Step 2 Initialize both LOAD registers with zeros

    //Set TLR0 to 0
    writeRegister(timerNumber, TLR0_OFFSET, 0);
    //Set TLR1 to 0
    writeRegister(timerNumber, TLR1_OFFSET, 0);

    //Step 3. 
    //Call the _reload function to move the LOAD values into the Counters
    intervalTimer_reload(timerNumber);
}

// You must configure the interval timer before you use it:
// 1. Set the Timer Control/Status Registers such that:
//  - The timer is in 64-bit cascade mode
//  - The timer counts down
//  - The timer automatically reloads when reaching zero
// 2. Initialize LOAD registers with appropriate values, given the `period`.
// 3. Call the _reload function to move the LOAD values into the Counters
void intervalTimer_initCountDown(uint32_t timerNumber, double period){
    //Initialize the timer registers and enable cascade mode.
    // initializeCounter(timerNumber);

    //Step 1
    writeRegister(timerNumber,0,0);//write a 0 to the TCSR0 register.
    //Not messing with TCSR1 because... it doesn't apply in cascade mode?
    //“TCSR1 register is used only for loading the TLR1 register in cascade mode.”

    //Step 1 - Create a filter that sets CASC bit = 1(Cascade enabled) & UDT0 bit = 0(Count up).
    //Data to write
    uint32_t timerData = readRegister(timerNumber,0);

    //set the CASC bit to 1 and the UDT0 bit to 1 in the TCSR0 and the ARHT bit to 1
    //register (cascade mode and down counting).
    timerData = timerData | CASC_BIT_OFFSET | UDT0_BIT_OFFSET | ARHT_OFFSET;

    writeRegister(timerNumber,0,timerData);

    //Step 2 Initialize both LOAD registers with appropriate values given the `period`.

    uint64_t ticks = XPAR_AXI_TIMER_0_CLOCK_FREQ_HZ * period;//100000000Hzs *  ??

    uint32_t upper = ticks >> SHIFT_32;
    uint32_t lower = ticks;

    //Set TLR1 to upper
    writeRegister(timerNumber, TLR1_OFFSET, upper);
    //Set TLR0 to lower
    writeRegister(timerNumber, TLR0_OFFSET, lower);

    //Step 3. 
    //Call the _reload function to move the LOAD values into the Counters
    intervalTimer_reload(timerNumber);        
}

// This function starts the interval timer running.
// If the interval timer is already running, this function does nothing.
// timerNumber indicates which timer should start running.
// Make sure to only change the Enable Timer bit of the register and not modify
// the other bits.
void intervalTimer_start(uint32_t timerNumber){
    uint32_t timerData = readRegister(timerNumber,0);
    timerData = timerData | (ENABLE_TIMER_BIT);//Enable the timer

    writeRegister(timerNumber, 0, timerData);
}

// This function stops a running interval timer.
// If the interval time is currently stopped, this function does nothing.
// timerNumber indicates which timer should stop running.s
// Make sure to only change the Enable Timer bit of the register and not modify
// the other bits.
void intervalTimer_stop(uint32_t timerNumber){
    uint32_t timerData = readRegister(timerNumber,0);
    timerData = timerData & ~(ENABLE_TIMER_BIT);//Disable the timer

    writeRegister(timerNumber, 0, timerData);
}

// This function is called nt8_whenever you want to reload the Counter values
// from the load registers.  For a count-up timer, this will reset the
// timer to zero.  For a count-down timer, this will reset the timer to
// its initial count-down value.  The load registers should have already
// been set in the appropriate `init` function, so there is no need to set
// them here.  You just need to enable the load (and remember to disable it
// immediately after otherwise you will be loading indefinitely).
void intervalTimer_reload(uint32_t timerNumber){

    uint32_t timerData = readRegister(timerNumber,0);

    timerData = timerData | LOAD_BIT;//Turn on load bit

    //Enable the load bit
    writeRegister(timerNumber,0,timerData);

    timerData = timerData & ~(LOAD_BIT);//Turn off load bit

    //Disable the load bit
    writeRegister(timerNumber,0,timerData);
}

// Use this function to ascertain how long a given timer has been running.
// Note that it should not be an error to call this function on a running timer
// though it usually makes more sense to call this after intervalTimer_stop()
// has been called. The timerNumber argument determines which timer is read.
// In cascade mode you will need to read the upper and lower 32-bit registers,
// concatenate them into a 64-bit counter value, and then perform the conversion
// to a double seconds value.
double intervalTimer_getTotalDurationInSeconds(uint32_t timerNumber){

    //Get upper bits from timer counter register
    uint64_t upper = readRegister(timerNumber, TCR1_OFFSET);
    //Get lower bits from timer counter register
    uint32_t lower = readRegister(timerNumber, TCR0_OFFSET);

    //Concat the upper and lower together
    double concatenatedCount = upper << SHIFT_32 | lower;
    double seconds = (concatenatedCount/XPAR_AXI_TIMER_0_CLOCK_FREQ_HZ);
    
    //Does this return a double?
    return seconds;
}

// Enable the interrupt output of the given timer.
void intervalTimer_enableInterrupt(uint8_t timerNumber){
    //Change ENIT bit to 1
    uint32_t TCSRData = readRegister(timerNumber,0);

    //set the ENIT bit to 1 in the TCSR0 register (enable interrupt).
    TCSRData = TCSRData | ENIT_OFFSET;

    writeRegister(timerNumber,0,TCSRData);
}

// Disable the interrupt output of the given timer.
void intervalTimer_disableInterrupt(uint8_t timerNumber){

    //Change ENIT bit to 0
    uint32_t TCSRData = readRegister(timerNumber,0);

    //ENIT bit to 0 in TCSR0 register (disable interrupt).
    TCSRData = TCSRData & ~(ENIT_OFFSET);

    writeRegister(timerNumber,0,TCSRData);
}

// Acknowledge the rollover to clear the interrupt output.
void intervalTimer_ackInterrupt(uint8_t timerNumber){
    //Change TOINT bit to 1
    uint32_t TCSRData = readRegister(timerNumber,0);

    //TOINT bit to 1 in TCSR0 register (acknowledge interrupt).
    TCSRData = TCSRData | TOINT_OFFSET;

    writeRegister(timerNumber,0,TCSRData);
}

//helper function for reading from device registers
//uint32_t returned value contains data in register.
static uint32_t readRegister(uint8_t timerNumber, uint32_t offset){
    if (timerNumber == TIMER_ZERO){
        return Xil_In32(XPAR_AXI_TIMER_0_BASEADDR + offset);
    } else if (timerNumber == TIMER_ONE){
        return Xil_In32(XPAR_AXI_TIMER_1_BASEADDR + offset);
    } else if (timerNumber == TIMER_TWO){
        return Xil_In32(XPAR_AXI_TIMER_1_BASEADDR + offset);
    }
}

//helper function for writing to device registers
static void writeRegister(uint8_t timerNumber, uint32_t offset, uint32_t value){
    if (timerNumber == TIMER_ZERO){
        Xil_Out32(XPAR_AXI_TIMER_0_BASEADDR + offset, value);
    } else if (timerNumber == TIMER_ONE){
        Xil_Out32(XPAR_AXI_TIMER_1_BASEADDR + offset, value);
    } else if (timerNumber == TIMER_TWO){
        Xil_Out32(XPAR_AXI_TIMER_2_BASEADDR + offset, value);
    }
}