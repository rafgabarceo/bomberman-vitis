#include "../headers/LEVEL0_DEFINES.h"
#include "../headers/WINDOW_CONSTANTS.h"
#include "../headers/MEM_CONSTANTS.h"

void level0_init(uint8_t* framebuffer, USER_DAT* user)
{
	static uint8_t* background = (uint8_t*)(FRAMEBUFFER_BASE_ADDR + LEVEL0_BACKGROUND_ADDR);

	static int cornersX[4] = { 0, // X corners coordinates
							BACKGROUND_WIDTH - 56,
							0,
							BACKGROUND_WIDTH - 56
	};

	static int cornersY[4] = { 0, // Y corners coordinates
								0,
								BACKGROUND_HEIGHT - 56,
								BACKGROUND_HEIGHT - 56
	};

	Block bombs[BOMB_AMMO];

	for (int i = 0; i < BOMB_AMMO; i++)
	{
		bombs[i] = (Block){
			.imageData = (uint8_t*) BOMB_ADDR,
			.state = PRESENT,
			.type = BOMB_BLOCK,
			.x_position = 56+(5*i),
			.y_position = BACKGROUND_HEIGHT + 23
		};
	}

	user->bombs = bombs;

	// Generate the metadata for the corner blocks
	Block corners[4];
	for (int i = 0; i < 4; i++)
	{
		corners[i] = (Block){
			.imageData = (uint8_t*)LEVEL0_AIR_CORNER_ADDR,
			.state = PRESENT,
			.type = WALL_BLOCK,
			.x_position = cornersX[i],
			.y_position = cornersY[i],
			.timerIsInitialized = false
		};
	}

	static int cratesX[44];
	static int cratesY[44];


	// Generate crate data
	Block crates[CRATE_COUNT];
	for (int i = 0; i < CRATE_COUNT; i++)
	{
		crates[i] = (Block){
		.imageData = (uint8_t*)CRATE_ADDR,
		.state = PRESENT,
		.type = BREAKABLE_BLOCK,
		.x_position = CRATE_COORDINATE_DATA[2*i],
		.y_position = CRATE_COORDINATE_DATA[2*i + 1]
		};
	}

	// Generate wall data
	Block wallsTop[25];
	Block wallsRight[11];
	Block wallsBottom[25];
	Block wallsLeft[11];

	for (int i = 0; i < 25; i++)
	{
		wallsTop[i] = (Block){
			.imageData = (uint8_t*)LEVEL0_AIR_CORNER_ADDR,
			.state = PRESENT,
			.type = WALL_BLOCK,
			.x_position = 56 * i,
			.y_position = 0
		};

		wallsBottom[i] = (Block){
			.imageData = (uint8_t*)LEVEL0_AIR_CORNER_ADDR,
			.state = PRESENT,
			.type = WALL_BLOCK,
			.x_position = 56 * i,
			.y_position = BACKGROUND_HEIGHT - 56
		};
	}

	for (int i = 0; i < 11; i++)
	{
		wallsRight[i] = (Block){
			.imageData = (uint8_t*)LEVEL0_AIR_WALL_ADDR,
			.state = PRESENT,
			.type = WALL_BLOCK,
			.x_position = 0,
			.y_position = 56*i
		};

		wallsLeft[i] = (Block){
			.imageData = (uint8_t*)LEVEL0_AIR_WALL_ADDR,
			.state = PRESENT,
			.type = WALL_BLOCK,
			.x_position = BACKGROUND_WIDTH - 56,
			.y_position = 56*i
		};
	}

	// Generate the tile data
	Block tiles[10];
	for (int i = 0; i < 10; i++)
	{
		tiles[i] = (Block){
			.imageData = (uint8_t*)LEVEL0_AIR_TILE_SQUARE_ADDR,
			.state = PRESENT,
			.type = WALL_BLOCK,
			.x_position = 112 + 112*i,
			.y_position = 56*2
		};
	}

	Block tiles1[10];
	for (int i = 0; i < 10; i++)
	{
		tiles1[i] = (Block){
			.imageData = (uint8_t*)LEVEL0_AIR_TILE_SQUARE_ADDR,
			.state = PRESENT,
			.type = WALL_BLOCK,
			.x_position = 112 + 112 * i,
			.y_position = 56 * 2 + 112
		};
	}

	Block tiles2[10];
	for (int i = 0; i < 10; i++)
	{
		tiles2[i] = (Block){
			.imageData = (uint8_t*)LEVEL0_AIR_TILE_SQUARE_ADDR,
			.state = PRESENT,
			.type = WALL_BLOCK,
			.x_position = 112 + 112 * i,
			.y_position = 56 * 3 + 168
		};
	}

	Block tiles3[10];
	for (int i = 0; i < 10; i++)
	{
		tiles3[i] = (Block){
			.imageData = (uint8_t*)LEVEL0_AIR_TILE_SQUARE_ADDR,
			.state = PRESENT,
			.type = WALL_BLOCK,
			.x_position = 112 + 112 * i,
			.y_position = 56 * 4 + 224
		};
	}

	while (1)
	{

		// Load the background into the page.
		for (int i = 1; i < 615; i++)
		{
			for(int j = 1; j < 1279; i++)
			{
				framebuffer[1280*3*i + 3*j] = background[1280*3*i + 3*j];
				framebuffer[1280*3*i + 3*j + 1] = background[1280*3*i + 3*j + 1];
				framebuffer[1280*3*i + 3*j + 2] = background[1280*3*i + 3*j + 2];
			}
		}

		generate_crates(framebuffer, crates);
		generate_walls_and_corners(framebuffer, wallsTop, wallsBottom, wallsRight, wallsLeft, corners);
		generate_tiles(framebuffer, tiles, tiles1, tiles2, tiles3);
		generate_player(framebuffer, user);
		generate_bomb(framebuffer, bombs, user);

		if (user->bombsDrop == BOMB_AMMO-1)
		{
			printf("Out of bombs. Exiting.");
			break; // exit to game
		}
	}
}

void generate_walls_and_corners(uint8_t* framebuffer, Block* wallsTop, Block* wallsBottom, Block* wallsRight, Block* wallsLeft, Block* corners)
{

	// Draw the top and bottom walls
	for (int k = 0; k < 25; k++)
	{
		for (int i = 0; i < BLOCK_HEIGHT; i++)
		{
			for (int j = 0; j < BLOCK_WIDTH; j++)
			{
				uint8_t r = wallsTop[k].imageData[BLOCK_WIDTH * 3 * i + 3 * j + 2];
				uint8_t g = wallsTop[k].imageData[BLOCK_WIDTH * 3 * i + 3 * j + 1];
				uint8_t b = wallsTop[k].imageData[BLOCK_WIDTH * 3 * i + 3 * j];
				uint32_t pixel = (r << 16) | (g << 8) | b;
				if (pixel) framebuffer[BACKGROUND_WIDTH * (i + wallsTop[k].y_position) + (j + wallsTop[k].x_position)] = pixel; // access the location in the framebuffer that we want to overwrite.

				r = wallsBottom[k].imageData[BLOCK_WIDTH * 3 * i + 3 * j + 2];
				g = wallsBottom[k].imageData[BLOCK_WIDTH * 3 * i + 3 * j + 1];
				b = wallsBottom[k].imageData[BLOCK_WIDTH * 3 * i + 3 * j];
				if (pixel) framebuffer[BACKGROUND_WIDTH * (i + wallsBottom[k].y_position) + (j + wallsBottom[k].x_position)] = pixel;

			}
		}
	}

	// Draw the right and left walls
	for (int k = 0; k < 11; k++)
	{
		for (int i = 0; i < BLOCK_HEIGHT; i++)
		{
			for (int j = 0; j < BLOCK_WIDTH; j++)
			{
				uint8_t r = wallsLeft[k].imageData[BLOCK_WIDTH * 3 * i + 3 * j + 2];
				uint8_t g = wallsLeft[k].imageData[BLOCK_WIDTH * 3 * i + 3 * j + 1];
				uint8_t b = wallsLeft[k].imageData[BLOCK_WIDTH * 3 * i + 3 * j];
				uint32_t pixel = (r << 16) | (g << 8) | b;
				if (pixel) framebuffer[BACKGROUND_WIDTH * (i + wallsLeft[k].y_position) + (j + wallsLeft[k].x_position)] = pixel; // access the location in the framebuffer that we want to overwrite.

				r = wallsRight[k].imageData[BLOCK_WIDTH * 3 * i + 3 * j + 2];
				g = wallsRight[k].imageData[BLOCK_WIDTH * 3 * i + 3 * j + 1];
				b = wallsRight[k].imageData[BLOCK_WIDTH * 3 * i + 3 * j];
				if (pixel) framebuffer[BACKGROUND_WIDTH * (i + wallsRight[k].y_position) + (j + wallsRight[k].x_position)] = pixel;

			}
		}
	}

	// Generate the four corners
	for (int k = 0; k < 4; k++)
	{
		for (int i = 0; i < BLOCK_HEIGHT; i++)
		{
			for (int j = 0; j < BLOCK_WIDTH; j++)
			{
				uint8_t r = corners[k].imageData[BLOCK_WIDTH * 3 * i + 3 * j + 2];
				uint8_t g = corners[k].imageData[BLOCK_WIDTH * 3 * i + 3 * j + 1];
				uint8_t b = corners[k].imageData[BLOCK_WIDTH * 3 * i + 3 * j];
				uint32_t pixel = (r << 16) | (g << 8) | b;
				if (pixel) framebuffer[BACKGROUND_WIDTH * (i + corners[k].y_position) + (j + corners[k].x_position)] = pixel; // access the location in the framebuffer that we want to overwrite.
			}
		}
	}
}

void generate_tiles(uint8_t* framebuffer, Block* tiles, Block* tiles1, Block* tiles2, Block* tiles3)
{
	// Generate the tiles
	for (int k = 0; k < 10; k++)
	{
		for (int i = 0; i < BLOCK_HEIGHT; i++)
		{
			for (int j = 0; j < BLOCK_WIDTH; j++)
			{
				uint8_t r = tiles[k].imageData[BLOCK_WIDTH * 3 * i + 3 * j + 3];
				uint8_t g = tiles[k].imageData[BLOCK_WIDTH * 3 * i + 3 * j + 2];
				uint8_t b = tiles[k].imageData[BLOCK_WIDTH * 3 * i + 3 * j + 1];
				uint8_t t = tiles[k].imageData[BLOCK_WIDTH * 3 * i + 3 * j];
				uint32_t pixel = (r << 24) | (g << 16) | (b << 8) | t;
				if (pixel) framebuffer[BACKGROUND_WIDTH * (i + tiles[k].y_position) + (j + tiles[k].x_position)] = pixel; // access the location in the framebuffer that we want to overwrite.

				r = tiles1[k].imageData[BLOCK_WIDTH * 3 * i + 3 * j + 3];
				g = tiles1[k].imageData[BLOCK_WIDTH * 3 * i + 3 * j + 2];
				b = tiles1[k].imageData[BLOCK_WIDTH * 3 * i + 3 * j + 1];
				t = tiles1[k].imageData[BLOCK_WIDTH * 3 * i + 3 * j];
				pixel = (r << 24) | (g << 16) | (b << 8) | t;
				if (pixel) framebuffer[BACKGROUND_WIDTH * (i + tiles1[k].y_position) + (j + tiles1[k].x_position)] = pixel; // access the location in the framebuffer that we want to overwrite.
				r = tiles2[k].imageData[BLOCK_WIDTH * 3 * i + 3 * j + 3];
				g = tiles2[k].imageData[BLOCK_WIDTH * 3 * i + 3 * j + 2];
				b = tiles2[k].imageData[BLOCK_WIDTH * 3 * i + 3 * j + 1];
				t = tiles2[k].imageData[BLOCK_WIDTH * 3 * i + 3 * j];
				pixel = (r << 24) | (g << 16) | (b << 8) | t;
				if (pixel) framebuffer[BACKGROUND_WIDTH * (i + tiles2[k].y_position) + (j + tiles2[k].x_position)] = pixel; // access the location in the framebuffer that we want to overwrite.

				r = tiles3[k].imageData[BLOCK_WIDTH * 3 * i + 3 * j + 3];
				g = tiles3[k].imageData[BLOCK_WIDTH * 3 * i + 3 * j + 2];
				b = tiles3[k].imageData[BLOCK_WIDTH * 3 * i + 3 * j + 1];
				t = tiles3[k].imageData[BLOCK_WIDTH * 3 * i + 3 * j];
				pixel = (r << 24) | (g << 16) | (b << 8) | t;
				if (pixel) framebuffer[BACKGROUND_WIDTH * (i + tiles3[k].y_position) + (j + tiles3[k].x_position)] = pixel; // access the location in the framebuffer that we want to overwrite.
			}
		}
	}
}

void generate_player(uint8_t* framebuffer, USER_DAT* user)
{
	for (int i = 0; i < BLOCK_HEIGHT; i++)
	{
		for (int j = 0; j < BLOCK_WIDTH; j++)
		{
			uint8_t r = user->imageData[BLOCK_WIDTH * 3 * i + 3 * j + 2];
			uint8_t g = user->imageData[BLOCK_WIDTH * 3 * i + 3 * j + 1];
			uint8_t b = user->imageData[BLOCK_WIDTH * 3 * i + 3 * j];
			uint32_t pixel = (r << 16) | (g << 8) | b;
			if (pixel) framebuffer[BACKGROUND_WIDTH * (i + user->user_pos_y) + (j + user->user_pos_x)] = pixel; // access the location in the framebuffer that we want to overwrite.
		}
	}
}

void generate_crates(uint8_t* framebuffer, Block* crates)
{
	for (int k = 0; k < CRATE_COUNT; k++)
	{
		if (crates[k].state != BROKEN)
		{
			for (int i = 0; i < BLOCK_HEIGHT; i++)
			{
				for (int j = 0; j < BLOCK_WIDTH; j++)
				{
					uint8_t r = crates[k].imageData[BLOCK_WIDTH * 3 * i + 3 * j + 2];
					uint8_t g = crates[k].imageData[BLOCK_WIDTH * 3 * i + 3 * j + 1];
					uint8_t b = crates[k].imageData[BLOCK_WIDTH * 3 * i + 3 * j];
					uint32_t pixel = (r << 16) | (g << 8) | b;
					if (pixel) framebuffer[BACKGROUND_WIDTH * (i + crates[k].y_position) + (j + crates[k].x_position)] = pixel; // access the location in the framebuffer that we want to overwrite.
				}
			}
		}

	}
}
void generate_bomb(uint8_t* framebuffer, Block* bomb, USER_DAT* user)
{
	for (int k = 0; k < BOMB_AMMO; k++)
	{
		for (int i = 0; i < BLOCK_HEIGHT; i++)
		{
			for (int j = 0; j < BLOCK_WIDTH; j++)
			{
				uint8_t r = bomb[user->bombsDrop].imageData[BLOCK_WIDTH * 3 * i + 3 * j + 2];
				uint8_t g = bomb[user->bombsDrop].imageData[BLOCK_WIDTH * 3 * i + 3 * j + 1];
				uint8_t b = bomb[user->bombsDrop].imageData[BLOCK_WIDTH * 3 * i + 3 * j];
				uint32_t pixel = (r << 16) | (g << 8) | b;
				if (pixel && user->bombs[k].state == PRESENT) framebuffer[BACKGROUND_WIDTH * (i + user->bombs[k].y_position) + (j + user->bombs[k].x_position)] = pixel; // access the location in the framebuffer that we want to overwrite.
			}
		}
	}
}
