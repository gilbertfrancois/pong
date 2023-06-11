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

#include "sound.h"
#include "typedefs.h"
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include <SDL2/SDL.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

void init_context(Context *ctx);
void init_ball(Context *ctx);
void init_paddles(Context *ctx);
void init_score(Context *ctx);
void init_stage_intro(Context *ctx);
void init_stage_playing(Context *ctx);

void update(Context *ctx);
void update_agent(Context *ctx, Paddle *paddle, int max_jitter);
void update_ball_position(Context *ctx);
void update_bouncing_ball(Context *ctx);
void update_collision(Context *ctx, Paddle *paddle);
void update_collision_detections(Context *ctx);
void update_display_size(Context *ctx);
void update_left_agent(Context *ctx);
void update_right_agent(Context *ctx);
void update_mode_intro(Context *ctx);
void update_mode_playing(Context *ctx);
void update_paddle(Context *ctx, Paddle *paddle);
void update_paddles(Context *ctx);
void update_scores(Context *ctx);

void draw(Context *ctx);
void draw_digits(Context *ctx, int digits, int x0, int y0);
void draw_stage_intro(Context *ctx);
void draw_stage_playing(Context *ctx);

void main_loop(void *arg);

void handle_events(Context *ctx);
void launch_ball(Context *ctx, int direction);
void native_to_retro_disp(Context *ctx, int *x, int *y);
void retro_to_native_disp(Context *ctx, int *x, int *y);
void switch_game_mode(Context *ctx, enum GameMode target_game_mode);
void warp_mouse(Context *ctx);

int main(int argc, char *argv[]);

#endif
