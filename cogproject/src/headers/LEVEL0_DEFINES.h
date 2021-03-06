#ifndef LEVEL0_DEFINES_H
#define LEVEL0_DEFINES_H
#include "BLOCK.h"
#include "USER.h"

#define COLLISION_OFFSET 58
#define CRATE_COUNT 13
#define BOMB_AMMO 23

// Access: 2*row + 1
static int CRATE_COORDINATE_DATA[] = {
	224,56,
	280,56,
	336,56,
	280,112,
	280,168,
	56,280,
	56,336,
	56,393,
	56,448,
	728,336,
	672,392,
	784,392,
	728,448
};
/*

Window data and framebuffer data should be passed.
This function will hyjack the current window and start placing down map data into the buffer.

Firts argument should be casted to a struct mfb_window before usage.

*/
void level0_init(uint8_t* framebuffer, USER_DAT* user);

/*

Wrapper function to generate or regenerate the walls and corners of the level.
This takes in the current window, the buffer, and the arrays that contain the block data for each segment.

*/
void generate_walls_and_corners(uint8_t*, Block*, Block*, Block*, Block*, Block*);


/*

Wrapper fucntions to generate or regenerate the tiles of the level.
This takes in the current window, the buffer, and the arrays that containt he block data for each segment.

*/
void generate_tiles(uint8_t*, Block*, Block*, Block*, Block*);

/*
Keyboard callback for player move functions.
Information on this callback function is taken from the Github documentation.
First parameter takes in the window pointer.
The second one would be the button pressed.
The third one would be the modifier key pressed.
The fourth one checks if it is currently pressed.
*/
void generate_player(uint8_t*, USER_DAT*);
void generate_collision_data(uint8_t*, Block*, Block*, Block*, Block*, Block*, int*, int*);
void generate_crates(uint8_t*, Block*);
void generate_bomb(uint8_t*, Block*, USER_DAT*);
#endif // LEVEL_0_H
