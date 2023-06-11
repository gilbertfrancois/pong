#ifndef TYPEDEFS_H_FILE
#define TYPEDEFS_H_FILE


#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

enum GameMode {
    MODE_INTRO,
    MODE_PLAYING
};

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
    // Display parameters
    SDL_Window *window;
    SDL_Renderer *renderer;
    SDL_Texture *texture;
    SDL_Color color;
    bool vsync;
    int vsync_divider;
    unsigned long frame_count;
    int target_fps;
    int retro_disp_w; // 100
    int retro_disp_h; //  80
    int native_disp_w;
    int native_disp_h;
    int pixel_w;
    int pixel_h;
    bool fullscreen;
    // Game parameters
    enum GameMode game_mode;
    bool running;
    Ball ball;
    int ball_speed;
    Paddle left_paddle;
    Paddle right_paddle;
    int paddle_speed;
    // Player parameters
    int number_of_players;
    bool left_player_serving;
    bool right_player_serving;
    bool start_game_serving;
    unsigned long serving_timer;
    unsigned long serving_duration;
    // Score
    int left_score;
    int right_score;
    SDL_Point left_score_pos;
    SDL_Point right_score_pos;
    // Sound
    bool sound_activated;
    Mix_Chunk *snd_bounce;
    Mix_Chunk *snd_score;
    Mix_Chunk *snd_pad;
    // Human Input Devices
    Mouse mouse;
} Context;

#endif
