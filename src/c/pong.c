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

#include "pong.h"
#include "SDL2/SDL_render.h"
#include "SDL2/SDL_video.h"
#include <SDL_mixer.h>
#include <SDL_scancode.h>

void handle_events(SDL_Window *window, GameState *g, bool *running) {
    SDL_Event event;
    // Handle keyboard events
    while (SDL_PollEvent(&event)) {
        const unsigned char *keystates = SDL_GetKeyboardState(NULL);
        int x = 0;
        int y = 0;
        const unsigned int button_state = SDL_GetMouseState(&x, &y);
        if (event.type == SDL_KEYDOWN) {
            if (keystates[SDL_SCANCODE_UP] || keystates[SDL_SCANCODE_W]) {
                g->left_paddle.kb_vy = -g->paddle_speed;
                g->mouse.needs_warp = true;
            }
            if (keystates[SDL_SCANCODE_DOWN] || keystates[SDL_SCANCODE_S]) {
                g->left_paddle.kb_vy = g->paddle_speed;
                g->mouse.needs_warp = true;
            }
            if (keystates[SDL_SCANCODE_U]) {
                g->right_paddle.kb_vy = -g->paddle_speed;
            }
            if (keystates[SDL_SCANCODE_J]) {
                g->right_paddle.kb_vy = g->paddle_speed;
            }
            if (keystates[SDL_SCANCODE_X] && g->left_player_serving) {
                launch_ball(0, g);
            }
            if (keystates[SDL_SCANCODE_M] && g->right_player_serving) {
                launch_ball(1, g);
            }
        }
        if (event.type == SDL_KEYUP) {
            if (!keystates[SDL_SCANCODE_UP] && g->left_paddle.kb_vy < 0) {
                g->left_paddle.kb_vy = 0;
                g->mouse.needs_warp = true;
            }
            if (!keystates[SDL_SCANCODE_DOWN] && g->left_paddle.kb_vy > 0) {
                g->left_paddle.kb_vy = 0;
                g->mouse.needs_warp = true;
            }

            if (!keystates[SDL_SCANCODE_U] && g->right_paddle.kb_vy < 0) {
                g->right_paddle.kb_vy = 0;
            }
            if (!keystates[SDL_SCANCODE_J] && g->right_paddle.kb_vy > 0) {
                g->right_paddle.kb_vy = 0;
            }
        }
        if (event.type == SDL_QUIT)
            *running = false;
        if (keystates[SDL_SCANCODE_ESCAPE])
            *running = false;
        if (keystates[SDL_SCANCODE_Q])
            *running = false;
        // -------------------------------------------------------------------
        if (event.type == SDL_MOUSEBUTTONDOWN) {
            if (button_state == SDL_BUTTON_LEFT && g->left_player_serving) {
                launch_ball(0, g);
            }
        }
    }
    // Allow mouse and keyboard input at the same time. Keep the mouse position
    // in sync.
    if (g->mouse.needs_warp) {
        warp_mouse(window, g);
    }
    // Scale mouse movement to the render texture.
    int x = 0;
    int y = 0;
    const unsigned int button_state = SDL_GetMouseState(&x, &y);
    native_to_retro_disp(g, &x, &y);
    g->left_paddle.ms_vy = y - g->mouse.y;
    g->mouse.y = y;
    // -------------------------------------------------------------------
}

void retro_to_native_disp(GameState *g, int *x, int *y) {
    *x = (*x * g->native_disp_w) / g->retro_disp_w;
    *y = (*y * g->native_disp_h) / g->retro_disp_h;
}

void native_to_retro_disp(GameState *g, int *x, int *y) {
    *x = (*x * g->retro_disp_w) / g->native_disp_w;
    *y = (*y * g->retro_disp_h) / g->native_disp_h;
}

void warp_mouse(SDL_Window *window, GameState *g) {
    g->mouse.y = g->left_paddle.rect.y + g->left_paddle.rect.h / 2;
    int x = 0;
    int y = g->mouse.y;
    retro_to_native_disp(g, &x, &y);
    SDL_WarpMouseInWindow(window, g->native_disp_w / 2, y);
    g->left_paddle.ms_vy = 0;
    g->mouse.needs_warp = false;
}

// Update display size in case the user has changed it.
void update_display_size(SDL_Window *window, GameState *g) {
    SDL_GetWindowSize(window, &g->native_disp_w, &g->native_disp_h);
    SDL_ShowCursor(SDL_DISABLE);
}

void update_agent(GameState *g, Paddle *paddle) {
    // Robot player
    int jitter = rand() % 3;
    int distance = (paddle->rect.y + paddle->rect.h / 2) -
                   (g->ball.rect.y + g->ball.rect.h / 2);
    int accel = 3 + jitter;
    paddle->rect.y -= distance / accel;
}

void update_left_agent(GameState *g) { update_agent(g, &(g->left_paddle)); }

void update_right_agent(GameState *g) { update_agent(g, &(g->right_paddle)); }

void update_paddle_position(GameState *g) {
    // Update paddle position
    g->left_paddle.rect.y += g->left_paddle.kb_vy + g->left_paddle.ms_vy;
    g->right_paddle.rect.y += g->right_paddle.kb_vy + g->right_paddle.ms_vy;
    // Prevent the paddles from going off-screen
    if (g->left_paddle.rect.y > (g->retro_disp_h - g->left_paddle.rect.h)) {
        g->left_paddle.rect.y = (g->retro_disp_h - g->left_paddle.rect.h);
    }
    if (g->left_paddle.rect.y < 0) {
        g->left_paddle.rect.y = 0;
    }
    if (g->right_paddle.rect.y > (g->retro_disp_h - g->right_paddle.rect.h)) {
        g->right_paddle.rect.y = (g->retro_disp_h - g->right_paddle.rect.h);
    }
    if (g->right_paddle.rect.y < 0) {
        g->right_paddle.rect.y = 0;
    }
}

void update_ball_position(GameState *g) {
    // Update ball position
    g->ball.rect.x += g->ball.vx;
    g->ball.rect.y += g->ball.vy;
    // Bounce agaist horizontal walls.
    if (g->ball.rect.y <= 0) {
        g->ball.vy = -g->ball.vy;
        g->ball.rect.y = 0;
        Mix_PlayChannel(-1, g->snd_bounce, 0);
    }
    if (g->ball.rect.y >= (g->retro_disp_h - g->ball.rect.h)) {
        g->ball.vy = -g->ball.vy;
        g->ball.rect.y = g->retro_disp_h - g->ball.rect.h;
        Mix_PlayChannel(-1, g->snd_bounce, 0);
    }
}

void update_scores(GameState *g) {
    // Point scored by one of the players.
    if (g->ball.rect.x <= 0) {
        g->right_score++;
        g->ball.vx = 0;
        g->ball.vy = 0;
        g->left_player_serving = true;
        Mix_PlayChannel(-1, g->snd_score, 0);
    }
    if (g->ball.rect.x >= (g->retro_disp_w - g->ball.rect.w)) {
        g->left_score++;
        g->ball.vx = 0;
        g->ball.vy = 0;
        g->right_player_serving = true;
        Mix_PlayChannel(-1, g->snd_score, 0);
    }
    if (g->left_player_serving) {
        g->ball.rect.x = g->left_paddle.rect.x + g->left_paddle.rect.w;
        g->ball.rect.y = g->left_paddle.rect.y + g->right_paddle.rect.h / 2 +
                         g->ball.rect.h / 2;
    }
    if (g->right_player_serving) {
        g->ball.rect.x = g->right_paddle.rect.x - g->ball.rect.w;
        g->ball.rect.y = g->right_paddle.rect.y + g->right_paddle.rect.h / 2 +
                         g->ball.rect.h / 2;
    }
}

void update_collision_detection(GameState *g) {
    // Collision detection
    if (SDL_HasIntersection(&g->ball.rect, &g->left_paddle.rect)) {
        int rel = (g->left_paddle.rect.y + g->left_paddle.rect.h / 2) -
                  (g->ball.rect.y + g->ball.rect.h / 2);
        g->ball.vy = -rel / g->pixel_h * g->ball_speed;
        g->ball.vx = -g->ball.vx;
        g->ball.rect.x = g->left_paddle.rect.x + g->left_paddle.rect.w;
        Mix_PlayChannel(-1, g->snd_pad, 0);
    }
    if (SDL_HasIntersection(&g->ball.rect, &g->right_paddle.rect)) {
        int rel = (g->right_paddle.rect.y + g->right_paddle.rect.h / 2) -
                  (g->ball.rect.y + g->ball.rect.h / 2);
        g->ball.vy = -rel / g->pixel_h * g->ball_speed;
        g->ball.vx = -g->ball.vx;
        g->ball.rect.x = g->right_paddle.rect.x - g->ball.rect.w;
        Mix_PlayChannel(-1, g->snd_pad, 0);
    }
}

void update(GameState *g) {
    update_ball_position(g);
    update_right_agent(g);
    update_left_agent(g);
    update_paddle_position(g);
    update_scores(g);
    update_collision_detection(g);
}

void draw(SDL_Renderer *renderer, SDL_Texture *texture, GameState *g) {
    // Set renderer to the offline texture.
    SDL_SetRenderTarget(renderer, texture);
    // Set color and clear the render context.
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xff);
    SDL_RenderClear(renderer);
    /* g->right_paddle.rect.y += 1; */

    // Set the current render color.
    SDL_SetRenderDrawColor(renderer, g->color.r, g->color.g, g->color.b, 255);
    SDL_RenderFillRect(renderer, &g->left_paddle.rect);
    SDL_RenderFillRect(renderer, &g->right_paddle.rect);
    SDL_RenderFillRect(renderer, &g->ball.rect);
    // Render net
    SDL_Rect block;
    // block.w = g->pixel_w / 4;
    block.w = g->pixel_w;
    block.h = g->pixel_h;
    block.x = g->retro_disp_w / 2 - block.w / 2;
    for (int i = 0; i < g->native_disp_h; i += 3 * g->pixel_h) {
        block.y = i;
        SDL_RenderFillRect(renderer, &block);
    }
    // Set renderer to the screen and copy/scale the texture on screen.
    SDL_SetRenderTarget(renderer, NULL);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}

/* void drawLabel(SDL_Renderer *renderer, TTF_Font *font, SDL_Color color, */
/*                char *text, int x, int y) { */
/*     SDL_Surface *surface; */
/*     SDL_Texture *texture; */
/*     surface = TTF_RenderText_Solid(font, text, color); */
/*     texture = SDL_CreateTextureFromSurface(renderer, surface); */
/*     score_board.w = surface->w; */
/*     score_board.h = surface->h; */
/*     score_board.x = x - surface->w / 2; */
/*     score_board.y = y; */
/*     SDL_FreeSurface(surface); */
/*     SDL_RenderCopy(renderer, texture, NULL, &score_board); */
/*     SDL_DestroyTexture(texture); */
/* } */

void init_game_state(GameState *g) {
    g->target_fps = 60;
    g->retro_disp_w = 100;
    g->retro_disp_h = 80;
    g->fullscreen = false;
    g->native_disp_w = 500;
    g->native_disp_h = 400;
    g->pixel_w = 1;
    g->pixel_h = 1;
    g->ball_speed = g->pixel_w * 60 / g->target_fps;
    g->paddle_speed = g->ball_speed * 2;
    g->font_size = 32;
    g->color.r = 255;
    g->color.g = 255;
    g->color.b = 255;
    g->left_paddle.rect.w = g->pixel_w;
    g->left_paddle.rect.h = g->pixel_h * 5;
    g->left_paddle.rect.x = 9 * g->pixel_w;
    g->left_paddle.rect.y = g->retro_disp_h / 2 - g->left_paddle.rect.h / 2;
    g->left_paddle.kb_vy = 0;
    g->right_paddle.rect.w = g->left_paddle.rect.w;
    g->right_paddle.rect.h = g->left_paddle.rect.h;
    g->right_paddle.rect.x =
        g->retro_disp_w - g->left_paddle.rect.x - g->right_paddle.rect.w;
    g->right_paddle.rect.y = g->left_paddle.rect.y;
    g->right_paddle.kb_vy = 0;
    g->ball.rect.w = g->pixel_w;
    g->ball.rect.h = g->pixel_h;
    g->ball.rect.x = g->retro_disp_w / 2 - g->ball.rect.w / 2;
    g->ball.rect.y = g->retro_disp_h / 2 - g->ball.rect.h / 2;
    g->ball.vx = -g->ball_speed;
    g->ball.vy = rand() % 2 - 1;
}

// int init_mouse(SDL_Window *window, GameState *g) {
// warp_mouse(window, g);
// SDL_WarpMouseInWindow(window, g->display_w / 2, g->display_h / 2);
// SDL_GetMouseState(&g->mouse.x, &g->mouse.y);
// g->mouse.y = (g->mouse.y * g->retro_disp_h) / g->display_h;
// return 0;
// }

void launch_ball(int player, GameState *g) {
    if (player == 0) {
        g->ball.vy = ((rand() % 2) - 1) * g->ball_speed;
        g->ball.vx = g->ball_speed;
    } else if (player == 1) {
        g->ball.vy = ((rand() % 2) - 1) * g->ball_speed;
        g->ball.vx = -g->ball_speed;
    } else {
        printf("Illegal state\n");
    }
    g->left_player_serving = false;
    g->right_player_serving = false;
}

int main(int argc, char *argv[]) {
    // Init variables
    SDL_Renderer *renderer = NULL;
    SDL_Window *window = NULL;
    SDL_Texture *texture = NULL;
    GameState g;
    init_game_state(&g);

    // time sync variables
    unsigned long fps_timer_start = 0;
    unsigned long tic = 0;
    unsigned long toc = 0;
    unsigned long chrono_frame = 0;
    unsigned long millis_per_frame = 1000 / g.target_fps;
    bool running = false;

    int status = SDL_Init(SDL_INIT_VIDEO);
    if (status < 0) {
        printf("Failed at SDL_Init()");
        exit(status);
    }
    // Init window and renderer
    window =
        SDL_CreateWindow("Pong", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                         g.native_disp_w, g.native_disp_h, SDL_WINDOW_SHOWN);
    if (g.fullscreen) {
        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
    }
    renderer = SDL_CreateRenderer(
        window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                                SDL_TEXTUREACCESS_TARGET, g.retro_disp_w,
                                g.retro_disp_h);
    update_display_size(window, &g);
    // Init sound
    status = Mix_Init(0);
    if (status < 0) {
        printf("Failed at Mix_Init()");
        exit(status);
    }
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 1, 1024);
    g.snd_bounce = Mix_LoadWAV("bounce.wav");
    g.snd_pad = Mix_LoadWAV("pad.wav");
    g.snd_score = Mix_LoadWAV("score.wav");
    // Init font
    TTF_Init();
    g.font = TTF_OpenFont("atari.ttf", g.font_size);
    // Init mouse
    warp_mouse(window, &g);
    // Info to console
    SDL_DisplayMode current;
    SDL_GetCurrentDisplayMode(0, &current);
    printf("Resolution: (%d, %d)\n", current.w, current.h);
    // printf("Pixel format: %u\n", current.format);
    printf("Refresh rate: %d\n", current.refresh_rate);
    SDL_RendererInfo info;
    SDL_GetRendererInfo(renderer, &info);
    printf("VSync enabled: %u\n", ((info.flags & SDL_RENDERER_PRESENTVSYNC) ==
                                   SDL_RENDERER_PRESENTVSYNC));
    printf("Renderer is using software fallback: %u\n",
           ((info.flags & SDL_RENDERER_SOFTWARE) == SDL_RENDERER_SOFTWARE));
    printf(
        "Renderer is hardware accelerated: %d\n",
        ((info.flags & SDL_RENDERER_ACCELERATED) == SDL_RENDERER_ACCELERATED));
    printf("Renderer supports rendering to texture: %d\n",
           ((info.flags & SDL_RENDERER_TARGETTEXTURE) ==
            SDL_RENDERER_TARGETTEXTURE));
    // Begin event loop
    running = true;
    while (running) {
        tic = SDL_GetTicks64();
        update_display_size(window, &g);
        handle_events(window, &g, &running);
        update(&g);
        draw(renderer, texture, &g);
        toc = SDL_GetTicks64();
        chrono_frame = toc - tic;
        unsigned int sleep_time =
            (unsigned int)(millis_per_frame - chrono_frame);
        // if (sleep_time > 0 && sleep_time < 1000)
        //     SDL_Delay(sleep_time);
    }
    // Exit gracefully.
    TTF_CloseFont(g.font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyTexture(texture);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
