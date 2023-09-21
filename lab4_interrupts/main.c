#include <stdbool.h>
#include <stdio.h>

#include "armInterrupts.h"
#include "interrupt_test.h"
#include "interrupts.h"

// Run the interrupt test to test the driver for the AXI Interrupt Controller
int main() {
  printf("Running the interrupt test\n");
  interrupt_test_run();
}