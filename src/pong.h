#ifndef PONG_H_FILE
#define PONG_H_FILE

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>

#define WIDTH 1000
#define HEIGHT 800 
#define FONT_SIZE 64

#define OWIDTH 100
#define OHEIGHT 80

const int PXLW = WIDTH / OWIDTH;
const int PXLH = HEIGHT / OHEIGHT;
const int BALL_SPEED = PXLW;
const int PADDLE_SPEED = PXLW * 2;

SDL_Renderer *renderer;
SDL_Window *window;
TTF_Font *font;
SDL_Color color;
bool running;
int frameCount;
int timerFPS;
int lastTime;
int lastFrame;
int fps;

SDL_Rect left_paddle;
SDL_Rect right_paddle;
int left_paddle_vel;
int right_paddle_vel;
SDL_Rect ball;
SDL_Rect score_board;
int vel_x;
int vel_y;
std::string score;
int left_score;
int right_score;
bool turn;

void serve();
void input();
void update();
void draw();
void drawLabel(std::string text, int x, int y);

int main(int argc, char *argv[]);

#endif
