#include "buttons.h"
#include "xparameters.h"
#include "xil_io.h"
#include "stdio.h"

#define FOUR_OFFSET 4
#define LOWER_FOUR_ALL_ON 0xF

//helper function for reading from device registers
//uint32_t returned value contains data in register. 
static uint32_t readRegister(uint32_t offset);

//helper function for writing to device registers
static void writeRegister(uint32_t offset, uint32_t value);

// Initializes the button driver software and hardware.
void buttons_init(){

    //Write to base address + offset 0x4 to set the direction of the switches
    //Sets up GPIO_TRI to be 0xF, which means that the first 4 bits are inputs
    writeRegister(FOUR_OFFSET, LOWER_FOUR_ALL_ON);

    //write the value to the GPIO_TRI register to setup the GPIO hardware to 
    //serve as inputs and then read the value back and check to see that it 
    //is the correct value.
    uint32_t gpio_tri = readRegister(4);
    if (gpio_tri != LOWER_FOUR_ALL_ON) {
        //The %x means print as hex
        printf("Error: GPIO_TRI register should be 0xF, but is 0x%x\n", gpio_tri);
    }
}

// Returns the current value of all 4 buttons as the lower 4 bits of the
// returned value. bit3 = BTN3, bit2 = BTN2, bit1 = BTN1, bit0 = BTN0.
uint8_t buttons_read(){

    uint32_t buttons = readRegister(0);
    //Apply the mask 0xF to the buttons register to get the lower 4 bits
    uint8_t buttons_lower_4_bits = buttons & LOWER_FOUR_ALL_ON;

    //print the buttons_lower_4_bits
    return buttons_lower_4_bits;
}

//helper function for reading from device registers
//uint32_t returned value contains data in register. 
static uint32_t readRegister(uint32_t offset) {
   return Xil_In32(XPAR_PUSH_BUTTONS_BASEADDR + offset);
}

//helper function for writing to device registers
static void writeRegister(uint32_t offset, uint32_t value) {
   Xil_Out32(XPAR_PUSH_BUTTONS_BASEADDR + offset, value);
}