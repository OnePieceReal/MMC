#include "LPC17xx.h"
#include "GLCD.h"
#include "KBD.h"

// Screen dimensions
#define SCREEN_HEIGHT 240
#define SCREEN_WIDTH  320

// Paddle and ball dimensions
#define PADDLE_HEIGHT 5
#define PADDLE_WIDTH  1
#define BALL_SIZE     1

// Colors
#define PADDLE_COLOR Yellow
#define BALL_COLOR   White

// Player and AI paddle positions
int player_paddle_pos = 8; // Starting position for player paddle
int ai_paddle_pos = 8;     // Starting position for AI paddle

// Ball position and direction
int ball_x = 16, ball_y = 8; // Starting position of the ball
int ball_dx = 1, ball_dy = 1; // Initial velocity of the ball

// Scores
int player_score = 0;
int ai_score = 0;

// Function to draw a paddle
void draw_paddle(int ln, int col, int height) {
    GLCD_SetBackColor(PADDLE_COLOR); // Set paddle background color
    for (int i = 0; i < height; i++) {
        GLCD_DisplayChar(ln + i, col, 1, ' ');
    }
}

// Function to clear a paddle
void clear_paddle(int ln, int col, int height) {
    GLCD_SetBackColor(Black); // Restore black background
    for (int i = 0; i < height; i++) {
        GLCD_DisplayChar(ln + i, col, 1, ' ');
    }
}

// Function to draw the ball
void draw_ball(int ln, int col) {
    GLCD_SetBackColor(Black); // Black background for the ball
    GLCD_DisplayChar(ln, col, 1, 'O');
}

// Function to clear the ball
void clear_ball(int ln, int col) {
    GLCD_SetBackColor(Black); // Black background for clearing the ball
    GLCD_DisplayChar(ln, col, 1, ' ');
}

// Function to display scores
void display_scores() {
    GLCD_SetBackColor(Black); // Black background for scores
    GLCD_SetTextColor(White);

    // Display player score
    GLCD_DisplayChar(0, 1, 1, 'P');
    GLCD_DisplayChar(0, 2, 1, ':');
    GLCD_DisplayChar(0, 3, 1, '0' + player_score);

    // Display AI score
    GLCD_DisplayChar(0, 16, 1, 'A');
    GLCD_DisplayChar(0, 17, 1, 'I');
    GLCD_DisplayChar(0, 18, 1, ':');
    GLCD_DisplayChar(0, 19, 1, '0' + ai_score);
}

// Update player paddle based on joystick input
void update_player_paddle() {
    uint32_t button = get_button();
    if (button == KBD_UP && player_paddle_pos > 0) {
        clear_paddle(player_paddle_pos, 1, PADDLE_HEIGHT);
        player_paddle_pos--;
        draw_paddle(player_paddle_pos, 1, PADDLE_HEIGHT);
    } else if (button == KBD_DOWN && player_paddle_pos < SCREEN_HEIGHT / 24 - PADDLE_HEIGHT) {
        clear_paddle(player_paddle_pos, 1, PADDLE_HEIGHT);
        player_paddle_pos++;
        draw_paddle(player_paddle_pos, 1, PADDLE_HEIGHT);
    }
}

// Update AI paddle to follow the ball
void update_ai_paddle() {
    if (ai_paddle_pos + PADDLE_HEIGHT / 2 < ball_y) {
        clear_paddle(ai_paddle_pos, SCREEN_WIDTH / 16 - 2, PADDLE_HEIGHT);
        ai_paddle_pos++;
        draw_paddle(ai_paddle_pos, SCREEN_WIDTH / 16 - 2, PADDLE_HEIGHT);
    } else if (ai_paddle_pos + PADDLE_HEIGHT / 2 > ball_y) {
        clear_paddle(ai_paddle_pos, SCREEN_WIDTH / 16 - 2, PADDLE_HEIGHT);
        ai_paddle_pos--;
        draw_paddle(ai_paddle_pos, SCREEN_WIDTH / 16 - 2, PADDLE_HEIGHT);
    }
}

// Update ball position and check for collisions
void update_ball() {
    clear_ball(ball_y, ball_x);

    // Update ball position
    ball_x += ball_dx;
    ball_y += ball_dy;

    // Check for collisions with top and bottom walls
    if (ball_y <= 0 || ball_y >= SCREEN_HEIGHT / 24 - BALL_SIZE) {
        ball_dy = -ball_dy; // Reverse vertical direction
    }

    // Check for collisions with player paddle
    if (ball_x == 2 && ball_y >= player_paddle_pos && ball_y < player_paddle_pos + PADDLE_HEIGHT) {
        ball_dx = -ball_dx; // Reverse horizontal direction

        // Adjust vertical trajectory based on collision point
        int paddle_mid = player_paddle_pos + PADDLE_HEIGHT / 2;
        if (ball_y < paddle_mid) {
            ball_dy = -1; // Ball moves upward
        } else if (ball_y > paddle_mid) {
            ball_dy = 1; // Ball moves downward
        }
    }

    // Check for collisions with AI paddle
    if (ball_x == SCREEN_WIDTH / 16 - 3 && ball_y >= ai_paddle_pos && ball_y < ai_paddle_pos + PADDLE_HEIGHT) {
        ball_dx = -ball_dx; // Reverse horizontal direction

        // Adjust vertical trajectory based on collision point
        int paddle_mid = ai_paddle_pos + PADDLE_HEIGHT / 2;
        if (ball_y < paddle_mid) {
            ball_dy = -1; // Ball moves upward
        } else if (ball_y > paddle_mid) {
            ball_dy = 1; // Ball moves downward
        }
    }

    // Check for scoring conditions
    if (ball_x <= 0) {
        ai_score++; // AI scores
        ball_x = SCREEN_WIDTH / 32;
        ball_y = SCREEN_HEIGHT / 16;
        ball_dx = 1; // Reset direction
        ball_dy = 1;
        display_scores(); // Update scores
    } else if (ball_x >= SCREEN_WIDTH / 16) {
        player_score++; // Player scores
        ball_x = SCREEN_WIDTH / 32;
        ball_y = SCREEN_HEIGHT / 16;
        ball_dx = -1; // Reset direction
        ball_dy = -1;
        display_scores(); // Update scores
    }

    draw_ball(ball_y, ball_x);
}

// Simple delay function
void delay(int count) {
    for (volatile int i = 0; i < count * 10000; i++);
}

int main() {
    GLCD_Init();
    GLCD_Clear(Black);          // Clear screen with black background
    GLCD_SetBackColor(Black);   // Default background is black
    GLCD_SetTextColor(White);   // Set text color to white

    KBD_Init();

    // Draw initial paddles and ball
    draw_paddle(player_paddle_pos, 1, PADDLE_HEIGHT);
    draw_paddle(ai_paddle_pos, SCREEN_WIDTH / 16 - 2, PADDLE_HEIGHT);
    draw_ball(ball_y, ball_x);

    // Display initial scores
    display_scores();

    while (1) {
        update_player_paddle();
        update_ai_paddle();
        update_ball();

        delay(2); // Delay to control the game speed
    }
}
