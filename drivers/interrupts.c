#include <xparameters.h>
#include "interrupts.h"
#include "intervalTimer.h"
#include "xil_io.h"
#include "stdio.h"
#include "armInterrupts.h"

#define INPUT_PENDING_REGISTER_OFFSET 0x04
#define INTERRUPT_ENABLE_REGISTER_OFFSET 0x08
#define INTERRUPT_ACKNOWLEDGE_REGISTER_OFFSET 0x0C
#define SET_INTERRUPT_ENABLED_OFFSET 0x10
#define CLEAR_INTERRUPT_ENABLED_OFFSET 0x14
#define MER_OFFSET 0x1C
#define MER_BITSON 0x03
#define TIMER_INDICATOR 3

//helper function created to check whether there's a pending input for
//a given interrupt. If there is, we call it's coresponding callback
//function if it is setup. (!NULL) 
static void interrupts_isr();

//helper function for reading from device registers
//uint32_t returned value contains data in register.
static uint32_t readRegister(uint32_t offset);

//helper function for writing to device registers
static void writeRegister(uint32_t offset, uint32_t value);

//Pointer to a callback function used by a specific interrupt
static void (*isrFcnPtrs[3])() = {NULL};

// Initialize interrupt hardware
// This function should:
// 1. Configure AXI INTC registers to:
//  - Enable interrupt output (see Master Enable Register)
//  - Disable all interrupt input lines.
// 2. Enable the Interrupt system on the ARM processor, and register an ISR
// handler function. This is done by calling:
//  - armInterrupts_init()
//  - armInterrupts_setupIntc(isr_fcn_ptr)
//  - armInterrupts_enable()
void interrupts_init(){

    //Enable interrupt output
    writeRegister(MER_OFFSET, MER_BITSON);

    //Disable all interrupt input lines
    writeRegister(INTERRUPT_ENABLE_REGISTER_OFFSET, 0);

    // 2. Enable the Interrupt system on the ARM processor, and register an ISR
    // handler function. This is done by calling:
    armInterrupts_init();
    armInterrupts_setupIntc(interrupts_isr);
    armInterrupts_enable();
}

// Register a callback function (fcn is a function pointer to this callback
// function) for a given interrupt input number (irq).  When this interrupt
// input is active, fcn will be called.
void interrupts_register(uint8_t irq, void (*fcn)()){
    //Register a function that is specific to a given interrupt.
    isrFcnPtrs[irq] = fcn;
}

// Enable single input interrupt line, given by irq number.
void interrupts_irq_enable(uint8_t irq){
    writeRegister(SET_INTERRUPT_ENABLED_OFFSET, 1<<irq);
}

// Disable single input interrupt line, given by irq number.
void interrupts_irq_disable(uint8_t irq){
    writeRegister(CLEAR_INTERRUPT_ENABLED_OFFSET, 1<<irq);
}

//helper function for reading from device registers
//uint32_t returned value contains data in register.
static uint32_t readRegister(uint32_t offset){
    return Xil_In32(XPAR_AXI_INTC_0_BASEADDR + offset);
}

//helper function for writing to device registers
static void writeRegister(uint32_t offset, uint32_t value){
    Xil_Out32(XPAR_AXI_INTC_0_BASEADDR + offset, value);
}

//helper function created to check whether there's a pending input for
//a given interrupt. If there is, we call it's coresponding callback
//function if it is setup. (!NULL) 
static void interrupts_isr() {

    uint8_t i;

    //Three timers. Three interrupts
    //Loop through each interrupt input
    for (i = 0; i < TIMER_INDICATOR; i++){
        uint32_t inputPendingRegister = readRegister(INPUT_PENDING_REGISTER_OFFSET);

        //Check if it has an interrupt pending
        //Get uint32_t and mask it with an appropriately sized value to check
        //if the specified pending bit is 1.
        if (readRegister(INPUT_PENDING_REGISTER_OFFSET) & 1 << i){
            // Check if there is a callback
            //By callback we mean that a pointer exists and points to a valid
            //function that was tied to the specified interrupt.
            if (isrFcnPtrs[i]){
                //Call the callback function
                isrFcnPtrs[i]();
            }
            writeRegister(INTERRUPT_ACKNOWLEDGE_REGISTER_OFFSET, 1 << i);
        }
    }
}