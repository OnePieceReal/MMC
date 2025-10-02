#include <stdio.h>
#include "Blinky.h"
#include "LPC17xx.h"                    
#include "KBD.h"
#include "LED.h"
#include "GLCD.h"
#include <stdbool.h>
#include "Photo_Gallery.h"
#include "TicTacToe.h"

#define MENU 0;
#define PHOTO_GALLERY 1
#define MP3_PLAYER 2
#define GAME 3

#define DELAY 500

static bool isDisplayStateModified = false;
static bool isStateModified = false; 
static int chosen_state = 0; 
static int menu_display_state = 1;
static int max_state = 3;

void delay(int delay_ms){
	for( int i=0;i<delay_ms;i++){
		__nop();
	}
}

void display_main_menu(){
		GLCD_SetBackColor(Black);
		GLCD_SetTextColor(White);

		GLCD_SetBackColor(White);
		GLCD_SetTextColor(Black);
		GLCD_DisplayString(1,0,1,"     MAIN MENU          ");
		GLCD_SetBackColor(Black);
		GLCD_SetTextColor(White);

		GLCD_DisplayString(3,0,1,"    Photo Gallery            ");
		GLCD_DisplayString(4,0,1,"    MP3 Player               ");
		GLCD_DisplayString(5,0,1,"    Tic-Tac-Toe              ");
		GLCD_DisplayString(7,0,1,"Joystick => UP/DOWN");
		GLCD_DisplayString(8,0,1,"Select => RIGHT"); 
}


void update_menu(){
	switch(menu_display_state){
		case PHOTO_GALLERY: 
				GLCD_SetBackColor(White);
				GLCD_SetTextColor(Black);
				GLCD_DisplayString(3,0,1, "    Photo Gallery              ");
				GLCD_SetBackColor(Black);
				GLCD_SetTextColor(White);
				GLCD_DisplayString(4,0,1, "    MP3 Player                 ");
				GLCD_DisplayString(5,0,1, "    Tic-Tac-Toe                ");
				GLCD_DisplayString(7, 0, 1, "Joystick => UP/DOWN");
				GLCD_DisplayString(8, 0, 1, "Select => RIGHT");
			break;
		case MP3_PLAYER:
				GLCD_SetBackColor(Black);
				GLCD_SetTextColor(White);
				GLCD_DisplayString(3,0,1, "    Photo Gallery              ");
				GLCD_SetBackColor(White);
				GLCD_SetTextColor(Black);
				GLCD_DisplayString(4,0,1, "    MP3 Player                 ");
				GLCD_SetBackColor(Black);
				GLCD_SetTextColor(White);
				GLCD_DisplayString(5,0,1, "    Tic-Tac-Toe                ");
				GLCD_DisplayString(7, 0, 1, "Joystick => UP/DOWN");
				GLCD_DisplayString(8, 0, 1, "Select => RIGHT"); 
			break;
		case GAME:
				GLCD_SetBackColor(Black);
				GLCD_SetTextColor(White);
				GLCD_DisplayString(3,0,1, "    Photo Gallery              ");
				GLCD_DisplayString(4,0,1, "    MP3 Player                 ");
				GLCD_SetBackColor(White);
				GLCD_SetTextColor(Black);			
				GLCD_DisplayString(5,0,1, "    Tic-Tac-Toe                ");
				GLCD_SetBackColor(Black);
				GLCD_SetTextColor(White);
				GLCD_DisplayString(7, 0, 1, "Joystick => UP/DOWN");
				GLCD_DisplayString(8, 0, 1, "Select => RIGHT");  
			break;
	}
}

void handle_joystick_input(uint32_t joystick_val){
	switch(joystick_val){
				case KBD_UP://GPIO port 1 -> PIN 23
					if(menu_display_state-1 <= 0){
						menu_display_state=1;
						isDisplayStateModified =false;
					}
					else{
						menu_display_state--;
						isDisplayStateModified =true;
					}
					break;
				case KBD_RIGHT://GPIO port 1 -> PIN 24
					chosen_state = menu_display_state; 
					isStateModified =true;
					break;
				case KBD_DOWN://GPIO port 1 -> PIN 25
					if(menu_display_state+1 > max_state){
						menu_display_state=max_state;
						isDisplayStateModified =false;
					}
					else{
						menu_display_state++;
						isDisplayStateModified =true;
					}
					break;
					
			}

		
}




void reset_main_menu(){
		GLCD_Clear(Black);
		isDisplayStateModified = false;
		isStateModified = false; 
		chosen_state = 0; 
		menu_display_state = 1;
		display_main_menu();
		update_menu();
}

void update_state(){
	//FSM
	//Initiate the respective view 
	switch(chosen_state){
		case PHOTO_GALLERY: 
			//start the photo gallery
			start_photo_gallery();
			break;
		case MP3_PLAYER:
			MP3();
			//start the mp3 player
			break;
		case GAME:
			;
			//start the game 
			int loop =1;
			while(loop){
				loop=start_game();
			}
			break;				
	}
	reset_main_menu();
}

/*------------------------------------------------------------------------------
  Main function
 *----------------------------------------------------------------------------*/
int main (void) {
	//initialize the needed peripherals 
	
	
	KBD_Init();
	GLCD_Init();
	GLCD_Clear(Black);
	uint32_t joystick_val;
	display_main_menu();
	update_menu();
	
	while(true){
		joystick_val = get_button();
		if(joystick_val != 0){
				handle_joystick_input(joystick_val);
		}
		//highlight the option hovered by the joystick
		if(isDisplayStateModified){
			update_menu();
			isDisplayStateModified=false;
		}
		//switch to the chosen view
		if(isStateModified){
			update_state();
			isStateModified=false;
		}
	}
}