#include "xgpio.h"
#include "xscugic.h"
#include "xparameters.h"
#include "xil_exception.h"

#include <stdio.h>

#include "headers/BLOCK.h"
#include "headers/LEVEL0_DEFINES.h"
#include "headers/MEM_CONSTANTS.h"
#include "headers/USER.h"

void initializeInterrupts();
void input_changed(void *callback_ref);

// Define some aliases for the GPIO channels
#define	GPIO_BTN_CHANNEL	1
#define GPIO_SW_CHANNEL		2
#define GPIO_LED_CHANNEL	1

#define MBUTTON 0
#define DBUTTON 1
#define LBUTTON 2
#define RBUTTON 3
#define UBUTTON 4

// Create a macro to define the interrupt priority
#define GIC_PRIO(prio)	(prio << 3)

USER_DAT* user; // Global variable for user data. Modified by main.

int main()
{

	// TODO: Generate overall user data
	// Initialize the player
	USER_DAT user = (USER_DAT){
		.imageData = (uint8_t*) USER_ADDR, // Because imageData is an address, simply make it equalt to the USER_ADDR
		.user_pos_x_prev = 112,
		.user_pos_y_prev = 56,
		.user_pos_x = 112,
		.user_pos_y = 56,
		.user_score = 0,
		.user_timer = 0,
		.bombsDrop = 0,
		.currentLevel = 0 // initialize the current level.
	};

	uint8_t* framebuffer = (uint8_t*)FRAMEBUFFER_BASE_ADDR;

	while(1)
	{
		switch(user.currentLevel)
		{
			case 0:
				level0_init(framebuffer, &user);
				break;
		}
	}
	return 0;
}

void initializeInterrupts()
{
	// TODO: Generate interrupt data

	int status = 0;
	XGpio btn_sw;
	XScuGic intc;
	XScuGic_Config *cfg;

	XGpio_Initialize(&btn_sw, XPAR_AXI_GPIO_0_DEVICE_ID);

	XGpio_SetDataDirection(&btn_sw, GPIO_BTN_CHANNEL, 0xFF);

	cfg = XScuGic_LookupConfig(XPAR_SCUGIC_0_DEVICE_ID);

	status = XScuGic_CfgInitialize(&intc, cfg, cfg->CpuBaseAddress);

	status = XScuGic_SelfTest(&intc);

	Xil_ExceptionInit();
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT, (Xil_ExceptionHandler)XScuGic_InterruptHandler, &intc);
	Xil_ExceptionEnable();

	status = XScuGic_Connect(&intc, XPAR_FABRIC_GPIO_0_VEC_ID, (Xil_ExceptionHandler)input_changed, (void *)&btn_sw);

	XScuGic_SetPriorityTriggerType(&intc, XPAR_FABRIC_GPIO_0_VEC_ID, GIC_PRIO(10), 0x03);

	XScuGic_Enable(&intc, XPAR_FABRIC_GPIO_0_VEC_ID);

	// Enable the interrupt on the GPIO device
	XGpio_InterruptEnable(&btn_sw, XGPIO_IR_MASK);
	XGpio_InterruptGlobalEnable(&btn_sw);
}

/*
 *
 * This function will handle the movement of the user along with the placement of the bombs and their current status.
 * Because this is an interrupt, the routine of execution will be very small and will not include any heavy calculations.
 *
 * */
void input_changed(void* callback_ref)
{
	static uint8_t key;

	key = XGpio_DiscreteRead(callback_ref, GPIO_BTN_CHANNEL);

	user->user_pos_x_prev = user->user_pos_x;
	user->user_pos_y_prev = user->user_pos_y;

		if (key == 0x08)
		{
			user->user_pos_x += USER_MOVEMENT_L0;
		}
		else if (key == 0x04)
		{
			user->user_pos_x -= USER_MOVEMENT_L0;
		}
		else if (key == 0x02)
		{
			user->user_pos_y += USER_MOVEMENT_L0;
		}
		else if (key == 0x10)
		{
			user->user_pos_y -= USER_MOVEMENT_L0;
		}
		else if (key == 0x01) // place da bomb
		{
			// Initiliaze bomb countdown
			if (user->cooldown != 1)
			{
				// user->cooldown = 1;
				// user->bombs[user->bombsDrop].blocktimer = bombTime; // Attach the bomb timer
				user->bombs[user->bombsDrop].timerIsInitialized = true;
				user->bombs[user->bombsDrop].x_position = user->user_pos_x;
				user->bombs[user->bombsDrop].y_position = user->user_pos_y;
				user->bombsDrop++;
			}

		}

		// Check x, y collision on tiles
		if ((((user->user_pos_y) / 56) % 2 == 0) && (((user->user_pos_x) / 56) % 2 == 0)) // Collision detected.
		{
			user->user_pos_y = user->user_pos_y_prev;
			user->user_pos_x = user->user_pos_x_prev;
		}
		else if ((user->user_pos_x == 0) || (user->user_pos_x == 1232))
		{
			user->user_pos_x = user->user_pos_x_prev;
		}
		else if ((user->user_pos_y == 0) || (user->user_pos_y == 560))
		{
			user->user_pos_y = user->user_pos_y_prev;
		}

		// Check if any coordinates hit the crate data.
		for (int i = 0; i < CRATE_COUNT; i++)
		{
			if (user->user_pos_y == CRATE_COORDINATE_DATA[2 * i + 1] && user->user_pos_x == CRATE_COORDINATE_DATA[2 * i])
			{
				user->user_pos_y = user->user_pos_y_prev;
				user->user_pos_x = user->user_pos_x_prev;
			}
		}
}
