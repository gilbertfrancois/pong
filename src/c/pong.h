//    Retro Tennis
//    Copyright (C) 2023  Gilbert Francois Duivesteijn
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

#include "typedefs.h"
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include <SDL2/SDL.h>
#ifdef __SOUND__
#include <SDL2/SDL_mixer.h>
#endif
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

void handle_events(GameState *g);
void init_game_state(GameState *g);
void init_players_state(GameState *g);
void retro_to_native_disp(GameState *g, int *x, int *y);
void native_to_retro_disp(GameState *g, int *x, int *y);
void update_display_size(GameState *g);
void update(GameState *g);
void update_left_agent(GameState *g);
void update_right_agent(GameState *g);
void update_paddle(GameState *g, Paddle *paddle);
void update_paddles(GameState *g);
void update_ball_position(GameState *g);
void update_scores(GameState *g);
void update__collision(GameState *g, Paddle *paddle);
void update_collision_detections(GameState *g);
void update_right_agent(GameState *g);
void draw_digits(GameState *g, int digits, int x0, int y0);
void draw(GameState *g);
void main_loop(void *arg);
void launch_ball(GameState *g, int direction);
void warp_mouse(GameState *g);

int main(int argc, char *argv[]);

#endif
