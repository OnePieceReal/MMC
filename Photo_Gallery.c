
#include <stdio.h>
#include "LPC17xx.h"                    
#include "KBD.h"
#include "LED.h"
#include "GLCD.h"
#include <stdbool.h>

#define DUNE 1
#define AVATAR 2
#define LUFFY 3
#define IMAGE_HEIGHT 130
#define IMAGE_WIDTH 230
#define GLCD_WIDTH 320
#define GLCD_HEIGHT 240



extern unsigned char avatar[];
extern unsigned char luffy[];
extern unsigned char dune[];
static int image_state = 1;
static bool isStateModified = false;
static bool continueProgram = true;
static int max_state =3;


bool center_image(int *x, int *y){
		if(GLCD_WIDTH < IMAGE_WIDTH || GLCD_HEIGHT<IMAGE_HEIGHT){
			return false;
		}
		 *x = (GLCD_WIDTH - IMAGE_WIDTH) / 2;
     *y = (GLCD_HEIGHT - IMAGE_HEIGHT) / 2;
		 return true;
}


void update_display_state(){
	int x ,y =0;
	center_image(&x,&y);
	switch(image_state){
		case DUNE:
			GLCD_Bitmap(x, y, IMAGE_WIDTH , IMAGE_HEIGHT, dune); // horizontal pos, vertical pos, width, height 
			break;
		case AVATAR:
			GLCD_Bitmap(x, y, IMAGE_WIDTH , IMAGE_HEIGHT, avatar);
			break;
		case LUFFY:
			GLCD_Bitmap(x, y, IMAGE_WIDTH , IMAGE_HEIGHT, luffy);
			break;
	}
}

static void delay_ms(int delay){
	for(int i=0;i<delay*500;i++){
		__nop();
	}
}

void manage_joystick_input(uint32_t joystick_val){
		//handles Joystick input
	switch(joystick_val){
				case KBD_UP://GPIO port 1 -> PIN 23
					if(image_state-1 <= 0){
						image_state =1;
						isStateModified = false;
					}
					else{
						image_state--;
						isStateModified = true;
					}
					break;
				case KBD_DOWN://GPIO port 1 -> PIN 25
					if(image_state+1>max_state){
						 image_state = max_state;
						 isStateModified =false;
					}
					else{
						 image_state++;
						 isStateModified = true;
					}		
					break;
				case KBD_LEFT://GPIO port 1 -> PIN 25	
					continueProgram=false;
					break;
			}
		
}

void display_instructions(){
	GLCD_Init();
	GLCD_Clear(Black);
	GLCD_SetBackColor(Black);
	GLCD_SetTextColor(White);
	
	GLCD_SetBackColor(White);
	GLCD_SetTextColor(Black);
	GLCD_DisplayString(1,0,1,"    PHOTO GALLERY         ");
	GLCD_SetBackColor(Black);
	GLCD_SetTextColor(White);
	GLCD_DisplayString(8,0,1,"Joystick => UP/DOWN");
	GLCD_DisplayString(9,0,1,"BACK 2 MENU => LEFT"); 
}

void reset_photo_gallery(){
	 image_state = 1;
	 isStateModified = false;
	 continueProgram = true;
	 max_state =3;
	 display_instructions(); 
	 update_display_state(); //initialize the first state
	 KBD_Init();
}

int start_photo_gallery(){
	reset_photo_gallery();
	while(continueProgram){
		uint32_t joystick_val = get_button();
		manage_joystick_input(joystick_val);
		if(isStateModified){
			update_display_state();
			isStateModified = false;
			delay_ms(5000);
			
		}
	}
	return 0; //end of program
}

//int main(){
//	
//	return 0;
//}

