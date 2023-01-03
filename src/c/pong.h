#ifndef PONG_H_FILE
#define PONG_H_FILE

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#define WITH_FULLSCREEN true
/* #define WIDTH 500 */
/* #define HEIGHT 400 */
const int FONT_SIZE = 64;

const int retro_disp_w = 100;
const int retro_disp_h = 80;
int display_w = 1920;
int display_h = 1080;
int offset = 0;
int pixel_w; //display_w / retro_disp_w;
int pixel_h; //display_h / retro_disp_h;
int BALL_SPEED;
int PADDLE_SPEED;

typedef struct {
    int x;
    int y;
    bool needs_warp;
} Mouse;

typedef struct {
    // geometry and position
    SDL_Rect rect;
    // y velocity from keyboard input
    int kb_vy;
    // y velocity from mouse input
    int ms_vy;
} Paddle;

typedef struct {
    // geometry and position
    SDL_Rect rect;
    // x velocity
    int vx;
    // y velocity
    int vy;
} Ball;

SDL_Rect score_board;
char *score;
int left_score;
int right_score;

int init_mouse(SDL_Window *window, Mouse *mouse);

void input(SDL_Window *window, Mouse *mouse, Paddle *left_paddle,
           Paddle *right_paddle, bool *running);
void update_display_size(SDL_Window *window);
void update(SDL_Window *window, Mouse *mouse, Ball *ball, Paddle *left_paddle,
            Paddle *right_paddle);
void draw(SDL_Renderer *renderer, TTF_Font *font, SDL_Color color, Ball *ball,
          Paddle *left_paddle, Paddle *right_paddle);
void drawLabel(SDL_Renderer *renderer, TTF_Font *font, SDL_Color color,
               char *text, int x, int y);

int main(int argc, char *argv[]);

#endif
