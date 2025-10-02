#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "KBD.h"
#include "GLCD.h"
#include <string.h>

#define PLAYER 1
#define AI 2
#define EMPTY 0
#define HORIZONTAL_SHIFT 4
#define VERTICAL_SHIFT 1


//global variables
static int board[3][3]={
    {0,0,0},
    {0,0,0},
    {0,0,0}
};

typedef struct Coordinate{
    int x;
    int y;
}Coordinate;


static struct Coordinate cord[3][3]={0}; //transform board coordinates to display coordinates



static int step_y = 3;
static int step_x = 4;
static int y_current = 0;
static int x_current  = 0;
static bool moveStatus = false;
static bool selectStatus = false;
static int prev_x =0;
static int prev_y =0;
static int player_score=0;
static int ai_score =0;
static int total_draw =0;


void display_tictactoe_board(){
	char ai[2];
	char pl[2];
	char drw[3];
	sprintf(ai,"%d",ai_score);
	sprintf(pl,"%d",player_score);
	sprintf(drw,"%d",total_draw);
	GLCD_Clear(Black);
	GLCD_SetBackColor(Black);
  GLCD_SetTextColor(White);
	GLCD_DisplayString (VERTICAL_SHIFT-1, HORIZONTAL_SHIFT-1, 1, " ___ ___ ___");
	GLCD_DisplayString (VERTICAL_SHIFT+0, HORIZONTAL_SHIFT-1, 1, "|   |   |   |");
	GLCD_DisplayString (VERTICAL_SHIFT+1, HORIZONTAL_SHIFT-1, 1, "|   |   |   |");
	GLCD_DisplayString (VERTICAL_SHIFT+2, HORIZONTAL_SHIFT-1, 1, "|___|___|___|");
	GLCD_DisplayString (VERTICAL_SHIFT+3, HORIZONTAL_SHIFT-1, 1, "|   |   |   |");
	GLCD_DisplayString (VERTICAL_SHIFT+4, HORIZONTAL_SHIFT-1, 1, "|   |   |   |");
  GLCD_DisplayString (VERTICAL_SHIFT+5, HORIZONTAL_SHIFT-1, 1, "|___|___|___|");
	GLCD_DisplayString (VERTICAL_SHIFT+6, HORIZONTAL_SHIFT-1, 1, "|   |   |   |");
	GLCD_DisplayString (VERTICAL_SHIFT+7, HORIZONTAL_SHIFT-1, 1, "|   |   |   |");
	GLCD_DisplayString (VERTICAL_SHIFT+8, HORIZONTAL_SHIFT-1, 1, "|___|___|___|");
	GLCD_DisplayString (4,0,1,"PL");
	GLCD_DisplayString (5,0,1,pl);
	GLCD_DisplayString (6,0,1,"DRW");
	GLCD_DisplayString (7,0,1,drw);
	GLCD_DisplayString (4,17,1,"AI");
	GLCD_DisplayString (5,17,1,ai);
 }


static void delay_ms(int delay){
	for(int i=0;i<delay*500;i++){
		__nop();
	}
}

int display_retry_screen(int result){
		
		GLCD_Clear(Black);
		GLCD_SetBackColor(White);
		GLCD_SetTextColor(Black);
		GLCD_DisplayString(3,0,1,"    Retry                    ");
		GLCD_SetBackColor(Black);
		GLCD_SetTextColor(White);
		GLCD_DisplayString(4,0,1,"    Exit                     ");
		GLCD_DisplayString(7,0,1,"Joystick => UP/DOWN");
		GLCD_DisplayString(8,0,1,"Select => SELECT"); 
		//display who won the game or if it is a draw
		GLCD_SetBackColor(White);
		GLCD_SetTextColor(Black);
		switch(result){
			case(-10):
				GLCD_DisplayString(1,0,1,"    Player Wins!!                   "); 
				player_score++;
				break;
			case(10):
				GLCD_DisplayString(1,0,1,"    AI Wins!! :(                  ");
				ai_score++;
				break;
			case(0):
				GLCD_DisplayString(1,0,1,"    Draw                 ");	
				total_draw++;
				break;
		}
		GLCD_SetBackColor(Black);
		GLCD_SetTextColor(White);	
		int chosen_btn =1; 
		while(true){
			switch(get_button()){
				case KBD_UP:
					GLCD_SetBackColor(White);
					GLCD_SetTextColor(Black);
					GLCD_DisplayString(3,0,1,"    Retry                    ");
				  GLCD_SetBackColor(Black);
					GLCD_SetTextColor(White);
					GLCD_DisplayString(4,0,1,"    Exit                     ");
					chosen_btn =1;
					break;
				case KBD_DOWN:
					GLCD_SetBackColor(Black);
					GLCD_SetTextColor(White);
					GLCD_DisplayString(3,0,1,"    Retry                    ");
					GLCD_SetBackColor(White);
					GLCD_SetTextColor(Black);
					GLCD_DisplayString(4,0,1,"    Exit                     ");
				  GLCD_SetBackColor(Black);
					GLCD_SetTextColor(White);
					chosen_btn =0;
					break;
				case KBD_SELECT:
					if(chosen_btn){
						 return 1;
					}
					else{
						return 0;	
					}
					break;
			}
			delay_ms(5000);
		}
		return 1;
}
//working
void initialize_coord(){
    int y  = VERTICAL_SHIFT+1;
    int x  = HORIZONTAL_SHIFT+1;
    for(int i=0;i<3;i++){
        for(int j=0;j<3;j++){
            cord[i][j] = (Coordinate){x,y};
            x = x+step_x ;
        }
        y = y+step_y;
        x = HORIZONTAL_SHIFT+1;
    }
}

void boardCoordToDisplayCoord(int row,int col,int arr[]){
     if(row < 3 && row >= 0 && col<3 && col>=0){
        arr[0]=cord[row][col].x;
        arr[1]=cord[row][col].y;
     }
}

//display cordinate to board coordinates --> working
void displayCoordToBoardCord(int display_x, int display_y,int arr[]){
    for(int i=0;i<3;i++){
        for(int j=0;j<3;j++){
            if(cord[i][j].x == display_x && cord[i][j].y == display_y){
                arr[0]=i;
                arr[1]=j;
                break;
            }
        }
    }
}

bool isInputValid(int row, int col){
    if(board[row][col] == EMPTY) return true;
    return false;
}

void manage_joystick(int id){
		
    //define the min and max values so that the cursor
    //cannot escape the defined borders
    int max_x = (HORIZONTAL_SHIFT+1)+step_x*2; //13
    int max_y = (VERTICAL_SHIFT+1)+step_y*2; //7
    int min_x = HORIZONTAL_SHIFT+1;
    int min_y = VERTICAL_SHIFT+1;
		prev_x =x_current;
		prev_y =y_current;
    switch(id){
        case KBD_UP:
            if((y_current-step_y)>= min_y && (y_current-step_y) <= max_y ){ //within the range y_min<=y<=y_max
                y_current -= step_y;
                moveStatus = true;
                //also update the cursor
                //remove prev cursor and return it to its original state

            }
            else{
              
                moveStatus = false;
                //nothing else to do
            }
            break;

        case KBD_DOWN:
            if((y_current+step_y)>= min_y && (y_current+step_y) <= max_y ){
                y_current += step_y;
                moveStatus = true;
            }
            else{

                moveStatus = false;
            }
            break;
        case KBD_LEFT:
            if((x_current-step_x)>= min_x && (x_current-step_x) <= max_x ){
                x_current -= step_x;
                moveStatus = true;
            }
            else{
               
                moveStatus = false;
            }
            break;
        case KBD_RIGHT:
            if((x_current+step_x)>= min_x && (x_current+step_x) <= max_x){
                x_current += step_x;
                moveStatus = true;
            }
            else{
                
                moveStatus = false;
            }
            break;
        case KBD_SELECT:
            //convert display coordinate to board coordinates
            ;
            int data_array[2];
            displayCoordToBoardCord(x_current, y_current,data_array);
            if(isInputValid(data_array[0],data_array[1])){
                board[data_array[0]][data_array[1]] = PLAYER;
                selectStatus = true;
            }
            else{
               
                selectStatus = false;
            }
            break;
    }
		

}

int evaluate(){
    //check rows
    for(int i=0;i<3;i++){
        if(board[i][0] == board[i][1] && board[i][1]==board[i][2]){
            if(board[i][0] == PLAYER) return -10;
            else if(board[i][0] == AI) return 10;
        }
    }
    //check columns
     for(int i=0;i<3;i++){
        if(board[0][i] == board[1][i] && board[1][i]==board[2][i]){
            if(board[0][i] == PLAYER) return -10;
            else if(board[0][i] == AI)return  10;
        }
    }
    //check diagonal
    if(board[0][0]==board[1][1] && board[1][1]==board[2][2]){
        if(board[0][0] == PLAYER) return -10;
        else if(board[0][0] == AI)return  10;
    }
    //check antidiagonal
    if(board[0][2]==board[1][1] && board[1][1]==board[2][0]){
        if(board[0][2] == PLAYER) return -10;
        else if(board[0][2] == AI) return 10;
    }
    return 0; //game continues
}

bool is_moves_left() {
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j] == EMPTY) return true;
    }
    }
    return false;
}

int minimax(int depth, int is_maximizing) {
    int score = evaluate();
    // If the game is over, return the score
    if (score == 10 || score == -10) return score;
    if (!is_moves_left()) return 0; // Draw

    if (is_maximizing) {
        int best = -1000;
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (board[i][j] == EMPTY) {
                    board[i][j] = AI; // Make a move
                    best = (best > minimax(depth + 1, 0)) ? best : minimax( depth + 1, 0);
                    board[i][j] = EMPTY; // Undo move
                }
            }
        }
        return best;
    } else {
        int best = 1000;
        for (int i = 0; i < 3; i++) {
            for (int j = 0; j < 3; j++) {
                if (board[i][j] == EMPTY) {
                    board[i][j] = PLAYER; // Make a move
                    best = (best < minimax(depth + 1, 1)) ? best : minimax( depth + 1, 1);
                    board[i][j] = EMPTY; // Undo move
                }
            }
        }
        return best;
    }
}

void find_best_move(int *best_row, int *best_col) {
    int best_value = -1000;
    *best_row = -1;
    *best_col = -1;
    for (int i = 0; i < 3; i++) {
        for (int j = 0; j < 3; j++) {
            if (board[i][j] == EMPTY) {
                board[i][j] = AI; // Try a move
                int move_value = minimax( 0, 0);
                board[i][j] = EMPTY; // Undo move

                if (move_value > best_value) {
                    *best_row = i;
                    *best_col = j;
                    best_value = move_value;
                }
            }
        }
    }
}



void enterInput(int row, int col, bool is_player){
    if(is_player){
        board[row][col]=PLAYER;
    }
    else{
        board[row][col]=AI;
    }
}

void reset_game(){
	 memset(board, 0, sizeof(board[0][0]) * 3 * 3);
	 y_current = 0;
	 x_current  = 0;
	 moveStatus = false;
	 selectStatus = false;
	 prev_x =0;
	 prev_y =0;
}
int start_game(){
		struct Coordinate ai_first_move[3][3]={{{1,1},{0,0},{1,1}},
																						{{0,0},{0,0},{0,2}},
																						{{1,1},{0,1},{1,1}}};
		bool isAIFirstMove =true;
		reset_game();
    initialize_coord(); //wrt to H shift and V shift
    //start at the midpoint
    y_current  = VERTICAL_SHIFT+1+step_y;
    x_current  = HORIZONTAL_SHIFT+1+step_x;
		display_tictactoe_board();
		KBD_Init();
	//display the cursor on the current POSITION
		GLCD_SetBackColor(White);
		GLCD_SetTextColor(Black);
		GLCD_DisplayString(y_current+step_y-2,x_current-1,1,"___");	
		//reset the background and text color
		GLCD_SetBackColor(Black);
		GLCD_SetTextColor(White);
    while(1){
         uint32_t input;
         while(1){  
						input = get_button();
            manage_joystick(input);
            if(moveStatus){
								//remove the cursor from the prev position
								GLCD_SetBackColor(Black);
								GLCD_SetTextColor(White);
								GLCD_DisplayString(prev_y+step_y-2,prev_x-1,1,"___");	
								//update the cursor from the current position
								GLCD_SetBackColor(White);
								GLCD_SetTextColor(Black);
								GLCD_DisplayString(y_current+step_y-2,x_current-1,1,"___");	
								GLCD_SetBackColor(Black);
		            GLCD_SetTextColor(White);
                moveStatus = false;
								delay_ms(5000);
            }
            if(selectStatus){
								GLCD_DisplayChar(y_current,x_current,1,'X');							
                selectStatus = false;
                break;
            }
         }      
				 
				//ai move
        int row = 0;
        int col =0;
				int data_array[2];
				if(isAIFirstMove){
				   displayCoordToBoardCord(x_current, y_current,data_array);
					 row=ai_first_move[data_array[0]][data_array[1]].x;
					 col=ai_first_move[data_array[0]][data_array[1]].y;
					 isAIFirstMove=false;
				 }
				else{
					find_best_move(&row,&col); 
				}
        //finding the optimal move
        enterInput(row,col,false);

        
        boardCoordToDisplayCoord(row,col,data_array);
        //we need to update display based on the ai move
				GLCD_DisplayChar(data_array[1],data_array[0],1,'O');	
        int winner =evaluate();
        if(!is_moves_left() || winner==10 || winner ==-10){
						if(display_retry_screen(winner)){
							return 1;
						}
            else{
							return 0;
						}
        }
        }
    return 0;
}