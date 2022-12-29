#include "SDL2/SDL_keyboard.h"
#include "SDL2/SDL_render.h"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>

#define WIDTH 640
#define HEIGHT 480
#define FONT_SIZE 64

#define OWIDTH 160
#define OHEIGHT 192

const int PXLW = WIDTH / OWIDTH;
const int PXLH = HEIGHT / OHEIGHT;
const int BALL_SPEED = PXLW;
const int PADDLE_SPEED = PXLH * 2;

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
float vel_x;
float vel_y;
std::string score;
int left_score;
int right_score;
bool turn;

void serve();
void input();
void update();
void draw();
void drawLabel(std::string text, int x, int y);

int main();
