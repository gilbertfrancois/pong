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
#include <SDL2/SDL_mixer.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>


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
    bool key_up;
    bool key_down;
    int timer_key;
    int timer_button;
    int timer_service;
} Agent;

typedef struct {
    int target_fps;
    int retro_disp_w; // 100
    int retro_disp_h; //  80
    int native_disp_w;
    int native_disp_h;
    bool fullscreen;
    int offset;
    int pixel_w;
    int pixel_h;
    int ball_speed;
    int paddle_speed;
    bool left_player_serving;
    bool right_player_serving;
    unsigned long serving_timer;
    SDL_Color color;
    Mouse mouse;
    Paddle left_paddle;
    Paddle right_paddle;
    Ball ball;
    SDL_Rect score_board;
    char *score;
    int left_score;
    int right_score;
    int left_score_x0;
    int left_score_y0;
    int right_score_x0;
    int right_score_y0;
    Mix_Chunk *snd_bounce;
    Mix_Chunk *snd_score;
    Mix_Chunk *snd_pad;
} GameState;

void init_game_state(GameState *g);
void retro_to_native_disp(GameState *g, int *x, int *y);
void native_to_retro_disp(GameState *g, int *x, int *y);
void handle_events(SDL_Window *window, GameState *g, bool *running);
void update_display_size(SDL_Window *window, GameState *g);
void update(GameState *g);
void update_paddle_position(GameState *g);
void update_ball_position(GameState *g);
void update_scores(GameState *g);
void update_collision_detection(GameState *g);
void update_right_agent(GameState *g);
void draw(SDL_Renderer *renderer, SDL_Texture *texture, GameState *g);
void draw_segment_A(SDL_Renderer *renderer, GameState *g, int x0, int y0);
void draw_segment_B(SDL_Renderer *renderer, GameState *g, int x0, int y0);
void draw_segment_C(SDL_Renderer *renderer, GameState *g, int x0, int y0);
void draw_segment_D(SDL_Renderer *renderer, GameState *g, int x0, int y0);
void draw_segment_E(SDL_Renderer *renderer, GameState *g, int x0, int y0);
void draw_segment_F(SDL_Renderer *renderer, GameState *g, int x0, int y0);
void draw_segment_G(SDL_Renderer *renderer, GameState *g, int x0, int y0);
void draw_0(SDL_Renderer *renderer, GameState *g, int x0, int y0);
void draw_1(SDL_Renderer *renderer, GameState *g, int x0, int y0);
void draw_2(SDL_Renderer *renderer, GameState *g, int x0, int y0);
void draw_3(SDL_Renderer *renderer, GameState *g, int x0, int y0);
void draw_4(SDL_Renderer *renderer, GameState *g, int x0, int y0);
void draw_5(SDL_Renderer *renderer, GameState *g, int x0, int y0);
void draw_6(SDL_Renderer *renderer, GameState *g, int x0, int y0);
void draw_7(SDL_Renderer *renderer, GameState *g, int x0, int y0);
void draw_8(SDL_Renderer *renderer, GameState *g, int x0, int y0);
void draw_9(SDL_Renderer *renderer, GameState *g, int x0, int y0);
void draw_digit(SDL_Renderer *renderer, GameState *g, int digits, int x0, int y0);

/* void drawLabel(SDL_Renderer *renderer, GameState *g, char *text, int x, int
 * y); */
void launch_ball(int player, GameState *g);
void warp_mouse(SDL_Window *window, GameState *g);

int main(int argc, char *argv[]);

#endif
