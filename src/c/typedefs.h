#ifndef TYPEDEFS_H_FILE
#define TYPEDEFS_H_FILE

#define __SOUND__

#include <SDL2/SDL.h>
#ifdef __SOUND__
#include <SDL2/SDL_mixer.h>
#endif
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
    // Normal or "playing direction": 1=left to right, -1=right to left
    int normal;
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
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    bool running;
    bool vsync;
    int vsync_divider;
    unsigned long frame_count;
    int target_fps;
    int retro_disp_w; // 100
    int retro_disp_h; //  80
    int native_disp_w;
    int native_disp_h;
    bool fullscreen;
    int pixel_w;
    int pixel_h;
    int ball_speed;
    int paddle_speed;
    int number_of_players;
    bool left_player_serving;
    bool right_player_serving;
    bool start_game_serving;
    unsigned long serving_timer;
    unsigned long serving_duration;
    SDL_Color color;
    Mouse mouse;
    Paddle left_paddle;
    Paddle right_paddle;
    Ball ball;
    int left_score;
    int right_score;
    SDL_Point left_score_pos;
    SDL_Point right_score_pos;
#ifdef __SOUND__
    Mix_Chunk *snd_bounce;
    Mix_Chunk *snd_score;
    Mix_Chunk *snd_pad;
#endif
} GameState;

#endif
