#ifndef GAMECONTROL
#define GAMECONTROL

#include <stdbool.h>

// Initialize the game control logic
// This function will initialize everything
void gameControl_init();

// Tick the game control logic
//
// This function should tick the lasers, handle screen touches, collisions,
// and updating statistics.
void gameControl_tick();

#endif /* GAMECONTROL */