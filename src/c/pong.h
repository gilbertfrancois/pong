//    Retro Tennis
//    Copyright (C) 2022  Gilbert Francois Duivesteijn
//
//    This program is free software: you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation, either version 3 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program.  If not, see <https://www.gnu.org/licenses/>.

#ifndef PONG_H_FILE
#define PONG_H_FILE

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

const int FONT_SIZE = 64;

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

typedef struct {
    int retro_disp_w; // 100
    int retro_disp_h; //  80
    bool fullscreen;
    int display_w;
    int display_h;
    int offset;
    int pixel_w;   // display_w / retro_disp_w;
    int pixel_h;   // display_h / retro_disp_h;
    int font_size; // 64
    int ball_speed;
    int paddle_speed;
    TTF_Font *font;
    SDL_Color color;
    Mouse mouse;
    Paddle left_paddle;
    Paddle right_paddle;
    Ball ball;
    SDL_Rect score_board;
    char *score;
    int left_score;
    int right_score;
} GameState;

void init_game_state(GameState *g);
int init_mouse(SDL_Window *window, GameState *g);

void handle_events(SDL_Window *window, GameState *g, bool *running);
void update_display_size(SDL_Window *window, GameState *g);
void update(SDL_Window *window, GameState *g);
void draw(SDL_Renderer *renderer, GameState *g);
/* void drawLabel(SDL_Renderer *renderer, GameState *g, char *text, int x, int y); */

int main(int argc, char *argv[]);

#endif
