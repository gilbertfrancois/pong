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
#include <SDL_timer.h>
#include <limits.h>

#define MIN(X, Y) (((X) < (Y)) ? (X) : (Y))

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
            if (keystates[SDL_SCANCODE_F]) {
                unsigned int flags = 0;
                g->fullscreen = !g->fullscreen;
                if (g->fullscreen)
                    flags = SDL_WINDOW_FULLSCREEN;
                SDL_SetWindowFullscreen(window, flags);
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
    // Trigger serving after timeout
    if (g->left_player_serving && SDL_GetTicks64() > g->serving_timer) {
        launch_ball(0, g);
    }
    if (g->right_player_serving && SDL_GetTicks64() > g->serving_timer) {
        launch_ball(1, g);
    }
}

void draw_segment(SDL_Renderer *renderer, GameState *g, int x0, int y0, int x,
                  int y, int w, int h) {
    SDL_Rect segment;
    segment.x = x0 + x * g->pixel_w;
    segment.y = y0 + y * g->pixel_h;
    segment.w = w * g->pixel_w;
    segment.h = h * g->pixel_h;
    SDL_RenderFillRect(renderer, &segment);
}

void draw_segment_A(SDL_Renderer *renderer, GameState *g, int x0, int y0) {
    draw_segment(renderer, g, x0, y0, 0, 0, 4, 1);
}

void draw_segment_B(SDL_Renderer *renderer, GameState *g, int x0, int y0) {
    draw_segment(renderer, g, x0, y0, 3, 0, 1, 3);
}

void draw_segment_C(SDL_Renderer *renderer, GameState *g, int x0, int y0) {
    draw_segment(renderer, g, x0, y0, 3, 3, 1, 5);
}

void draw_segment_D(SDL_Renderer *renderer, GameState *g, int x0, int y0) {
    draw_segment(renderer, g, x0, y0, 0, 7, 4, 1);
}

void draw_segment_E(SDL_Renderer *renderer, GameState *g, int x0, int y0) {
    draw_segment(renderer, g, x0, y0, 0, 3, 1, 5);
}

void draw_segment_F(SDL_Renderer *renderer, GameState *g, int x0, int y0) {
    draw_segment(renderer, g, x0, y0, 0, 0, 1, 3);
}

void draw_segment_G(SDL_Renderer *renderer, GameState *g, int x0, int y0) {
    draw_segment(renderer, g, x0, y0, 0, 3, 4, 1);
}

void draw_0(SDL_Renderer *renderer, GameState *g, int x0, int y0) {
    draw_segment_A(renderer, g, x0, y0);
    draw_segment_B(renderer, g, x0, y0);
    draw_segment_C(renderer, g, x0, y0);
    draw_segment_D(renderer, g, x0, y0);
    draw_segment_E(renderer, g, x0, y0);
    draw_segment_F(renderer, g, x0, y0);
}

void draw_1(SDL_Renderer *renderer, GameState *g, int x0, int y0) {
    draw_segment_B(renderer, g, x0, y0);
    draw_segment_C(renderer, g, x0, y0);
}

void draw_2(SDL_Renderer *renderer, GameState *g, int x0, int y0) {
    draw_segment_A(renderer, g, x0, y0);
    draw_segment_B(renderer, g, x0, y0);
    draw_segment_D(renderer, g, x0, y0);
    draw_segment_E(renderer, g, x0, y0);
    draw_segment_G(renderer, g, x0, y0);
}

void draw_3(SDL_Renderer *renderer, GameState *g, int x0, int y0) {
    draw_segment_A(renderer, g, x0, y0);
    draw_segment_B(renderer, g, x0, y0);
    draw_segment_C(renderer, g, x0, y0);
    draw_segment_D(renderer, g, x0, y0);
    draw_segment_G(renderer, g, x0, y0);
}

void draw_4(SDL_Renderer *renderer, GameState *g, int x0, int y0) {
    draw_segment_B(renderer, g, x0, y0);
    draw_segment_C(renderer, g, x0, y0);
    draw_segment_F(renderer, g, x0, y0);
    draw_segment_G(renderer, g, x0, y0);
}

void draw_5(SDL_Renderer *renderer, GameState *g, int x0, int y0) {
    draw_segment_A(renderer, g, x0, y0);
    draw_segment_C(renderer, g, x0, y0);
    draw_segment_D(renderer, g, x0, y0);
    draw_segment_F(renderer, g, x0, y0);
    draw_segment_G(renderer, g, x0, y0);
}

void draw_6(SDL_Renderer *renderer, GameState *g, int x0, int y0) {
    draw_segment_C(renderer, g, x0, y0);
    draw_segment_D(renderer, g, x0, y0);
    draw_segment_E(renderer, g, x0, y0);
    draw_segment_F(renderer, g, x0, y0);
    draw_segment_G(renderer, g, x0, y0);
}

void draw_7(SDL_Renderer *renderer, GameState *g, int x0, int y0) {
    draw_segment_A(renderer, g, x0, y0);
    draw_segment_B(renderer, g, x0, y0);
    draw_segment_C(renderer, g, x0, y0);
}

void draw_8(SDL_Renderer *renderer, GameState *g, int x0, int y0) {
    draw_segment_A(renderer, g, x0, y0);
    draw_segment_B(renderer, g, x0, y0);
    draw_segment_C(renderer, g, x0, y0);
    draw_segment_D(renderer, g, x0, y0);
    draw_segment_E(renderer, g, x0, y0);
    draw_segment_F(renderer, g, x0, y0);
    draw_segment_G(renderer, g, x0, y0);
}

void draw_9(SDL_Renderer *renderer, GameState *g, int x0, int y0) {
    draw_segment_A(renderer, g, x0, y0);
    draw_segment_B(renderer, g, x0, y0);
    draw_segment_C(renderer, g, x0, y0);
    draw_segment_F(renderer, g, x0, y0);
    draw_segment_G(renderer, g, x0, y0);
}

void draw_digit(SDL_Renderer *renderer, GameState *g, int digits, int x0,
                int y0) {
    switch (digits) {
    case 0:
        draw_0(renderer, g, x0, y0);
        break;
    case 1:
        draw_1(renderer, g, x0, y0);
        break;
    case 2:
        draw_2(renderer, g, x0, y0);
        break;
    case 3:
        draw_3(renderer, g, x0, y0);
        break;
    case 4:
        draw_4(renderer, g, x0, y0);
        break;
    case 5:
        draw_5(renderer, g, x0, y0);
        break;
    case 6:
        draw_6(renderer, g, x0, y0);
        break;
    case 7:
        draw_7(renderer, g, x0, y0);
        break;
    case 8:
        draw_8(renderer, g, x0, y0);
        break;
    case 9:
        draw_9(renderer, g, x0, y0);
        break;
    }
}

void draw_digits(SDL_Renderer *renderer, GameState *g, int digits, int x0,
                 int y0) {
    digits = digits % 1000;
    int i0 = (int)(digits % 10);
    int i10 = (int)((digits / 10) % 10);
    int i100 = (int)((digits / 100) % 10);
    draw_digit(renderer, g, i0, x0, y0);
    if (digits > 9)
        draw_digit(renderer, g, i10, x0 - 8, y0);
    if (digits > 99)
        draw_digit(renderer, g, i100, x0 - 16, y0);
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

void update_agent(GameState *g, Paddle *paddle, int max_jitter) {
    // method 1:
    // ball dir: 1=left attacking, -1=right attacking
    int ball_dir = (g->ball.vx > 0) ? 1 : ((g->ball.vx < 0) ? -1 : 0);
    // paddle_pos: 1=left, -1=right
    int paddle_pos = paddle->rect.x < g->retro_disp_w / 2 ? 1 : -1;
    // 1=even frame, 0=odd frame
    int even_frame = (int)(g->frame_count % 2);
    // Let agent move slower when attacking, faster when defending.
    int take_it_easy = (ball_dir * paddle_pos * even_frame == 1) ? 0 : 1;
    // Distance between pad and ball.
    int distance = (paddle->rect.y + paddle->rect.h / 2) -
                   (g->ball.rect.y + g->ball.rect.h / 2);
    // Do some jittering to make sure the agent does not hit the ball always in
    // the middle of the pad.
    // int jitter = (int)(rand() % (int)max_jitter);
    printf("distance %d, paddle_pos: %d, max_jitter: %d\n", distance, paddle_pos, max_jitter);
    // Don't let the ball pass by when heading straight to the pad.
    if (abs(g->ball.vy) == 0) {
        max_jitter = (max_jitter > 2) ? 1 : max_jitter;
    }
    if (distance < -1 * max_jitter) {
        paddle->kb_vy = g->ball_speed * take_it_easy;
        paddle->ms_vy = 0;
    } else if (distance > 1 * max_jitter) {
        paddle->kb_vy = -g->ball_speed * take_it_easy;
        paddle->ms_vy = 0;
    } else {
        paddle->kb_vy = 0;
        paddle->ms_vy = 0;
    }
}

void update_left_agent(GameState *g) {
    int jitter = 0;
    int delta = (g->left_score - g->right_score);
    if (delta > 0) {
        jitter = (delta < 7) ? delta : 7; 
    }
    update_agent(g, &(g->left_paddle), jitter);
}

void update_right_agent(GameState *g) {
    int jitter = 0;
    int delta = (g->right_score - g->left_score);
    if (delta > 0) {
        jitter = (delta < 7) ? delta : 7; 
    }
    update_agent(g, &(g->right_paddle), jitter);
}

void update_paddle(GameState *g, Paddle *paddle) {
    paddle->rect.y += paddle->kb_vy + paddle->ms_vy;
    // Prevent the paddles from going off-screen
    if (paddle->rect.y > (g->retro_disp_h - paddle->rect.h))
        paddle->rect.y = (g->retro_disp_h - paddle->rect.h);
    if (paddle->rect.y < 0)
        paddle->rect.y = 0;
}

void update_paddles(GameState *g) {
    update_paddle(g, &(g->left_paddle));
    update_paddle(g, &(g->right_paddle));
}

void update_ball_position(GameState *g) {
    // Update ball position
    g->ball.rect.x += g->ball.vx;
    g->ball.rect.y += g->ball.vy;
    // if (abs(g->ball.vx) > 1 && abs(g->ball.vy) > 0) {
    //     g->ball.rect.x += g->ball.vx / 2;
    //     g->ball.rect.y += g->ball.vy * (int)(g->frame_count % 2);
    // }
    // else {
    //     g->ball.rect.x += g->ball.vx;
    //     g->ball.rect.y += g->ball.vy;
    // }
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
        g->serving_timer = SDL_GetTicks64() + g->serving_duration;
        Mix_PlayChannel(-1, g->snd_score, 0);
    }
    if (g->ball.rect.x >= (g->retro_disp_w - g->ball.rect.w)) {
        g->left_score++;
        g->ball.vx = 0;
        g->ball.vy = 0;
        g->right_player_serving = true;
        g->serving_timer = SDL_GetTicks64() + g->serving_duration;
        Mix_PlayChannel(-1, g->snd_score, 0);
    }
    // const Uint64 timeout = SDL_GetTicks64() + 100;
    // while (SDL_GetTicks64() < timeout) {
    //     // ... do work until timeout has elapsed
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

void update__collision(GameState *g, Paddle *paddle) {
    if (SDL_HasIntersection(&g->ball.rect, &paddle->rect)) {
        // Change direction of the ball in opposite direction.
        int sign = (g->ball.vx > 0) ? -1 : ((g->ball.vx < 0) ? 1 : 0);
        // Detect where the ball has hit the paddle, to determine the bounce
        // angle.
        int rel = (paddle->rect.y + paddle->rect.h / 2) -
                  (g->ball.rect.y + g->ball.rect.h / 2);
        // Add a bit of randomness in angle and speed when the ball hits the
        // middle of the paddle.
        int mid_x = 0;
        int mid_y = 0;
        if (rel == 0) {
            mid_x = 0;
            mid_y = rand() % 3 - 1;
            if (mid_y != 0)
                mid_x = 1;
        }
        // Compute the bounce back angle and speed.
        g->ball.vy = -(rel / g->pixel_h + mid_y) * g->ball_speed;
        g->ball.vx = sign * (1 + mid_x) * g->ball_speed;
        g->ball.rect.x = paddle->rect.x + paddle->rect.w;
        // Correct the position of the ball to prevent the ball being stuck
        // in the middle of the paddle.
        if (sign > 0) {
            g->ball.rect.x = g->left_paddle.rect.x + g->left_paddle.rect.w;
        } else {
            g->ball.rect.x = g->right_paddle.rect.x - g->ball.rect.w;
        }
        Mix_PlayChannel(-1, g->snd_pad, 0);
    }
}

void update_collision_detections(GameState *g) {
    update__collision(g, &(g->left_paddle));
    update__collision(g, &(g->right_paddle));
}

void _update_collision_detections(GameState *g) {
    // Collision detection
    int mid_x = 0;
    int mid_y = 0;
    if (SDL_HasIntersection(&g->ball.rect, &g->left_paddle.rect)) {
        int rel = (g->left_paddle.rect.y + g->left_paddle.rect.h / 2) -
                  (g->ball.rect.y + g->ball.rect.h / 2);
        if (rel == 0) {
            mid_x = 0;
            mid_y = rand() % 3 - 1;
            if (mid_y != 0) {
                mid_x = 1;
            }
        }
        g->ball.vy = -(rel / g->pixel_h + mid_y) * g->ball_speed;
        g->ball.vx = (1 + mid_x) * g->ball_speed;
        g->ball.rect.x = g->left_paddle.rect.x + g->left_paddle.rect.w;
        Mix_PlayChannel(-1, g->snd_pad, 0);
    }
    if (SDL_HasIntersection(&g->ball.rect, &g->right_paddle.rect)) {
        int rel = (g->right_paddle.rect.y + g->right_paddle.rect.h / 2) -
                  (g->ball.rect.y + g->ball.rect.h / 2);
        if (rel == 0) {
            mid_x = rand() % 2;
            mid_y = rand() % 3 - 1;
        }
        g->ball.vy = -(rel / g->pixel_h) * g->ball_speed;
        g->ball.vx = -(1 + mid_x) * g->ball_speed;
        g->ball.rect.x = g->right_paddle.rect.x - g->ball.rect.w;
        Mix_PlayChannel(-1, g->snd_pad, 0);
    }
}

void update(GameState *g) {
    update_ball_position(g);
    update_right_agent(g);
    // update_left_agent(g);
    update_paddles(g);
    update_scores(g);
    update_collision_detections(g);
}

void draw(SDL_Renderer *renderer, SDL_Texture *texture, GameState *g) {
    // Set renderer to the offline texture.
    SDL_SetRenderTarget(renderer, texture);
    // Set color and clear the render context.
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xff);
    SDL_RenderClear(renderer);
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
    // Draw scores
    draw_digits(renderer, g, g->left_score, g->left_score_pos.x,
                g->left_score_pos.y);
    draw_digits(renderer, g, g->right_score, g->right_score_pos.x,
                g->right_score_pos.y);
    // Set renderer to the screen and copy/scale the texture on screen.
    SDL_SetRenderTarget(renderer, NULL);
    SDL_RenderCopy(renderer, texture, NULL, NULL);
    SDL_RenderPresent(renderer);
}

void init_game_state(GameState *g) {
    g->vsync = false;
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
    g->color.r = 255;
    g->color.g = 255;
    g->color.b = 255;
    g->left_paddle.rect.w = g->pixel_w;
    g->left_paddle.rect.h = g->pixel_h * 5;
    g->left_paddle.rect.x = 9 * g->pixel_w;
    g->left_paddle.rect.y = g->retro_disp_h / 2 - g->left_paddle.rect.h / 2;
    g->left_paddle.kb_vy = 0;
    g->left_paddle.ms_vy = 0;
    g->right_paddle.rect.w = g->left_paddle.rect.w;
    g->right_paddle.rect.h = g->left_paddle.rect.h;
    g->right_paddle.rect.x =
        g->retro_disp_w - g->left_paddle.rect.x - g->right_paddle.rect.w;
    g->right_paddle.rect.y = g->left_paddle.rect.y;
    g->right_paddle.kb_vy = 0;
    g->right_paddle.ms_vy = 0;
    g->left_score = 0;
    g->left_score_pos.x =
        g->left_paddle.rect.x + 3 * g->pixel_w + 8 + g->pixel_w;
    g->left_score_pos.y = 2 * g->pixel_h;
    g->right_score = 0;
    g->right_score_pos.x = g->right_paddle.rect.x - 5 * g->pixel_w;
    g->right_score_pos.y = 2 * g->pixel_h;

    g->ball.rect.w = g->pixel_w;
    g->ball.rect.h = g->pixel_h;
    g->ball.rect.x = g->retro_disp_w / 2 - g->ball.rect.w / 2;
    g->ball.rect.y = g->retro_disp_h / 2 - g->ball.rect.h / 2;
    g->ball.vx = -g->ball_speed;
    g->ball.vy = rand() % 2 - 1;
    g->serving_timer = SDL_GetTicks64() + g->serving_duration;
    g->serving_duration = 2000;
}

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
    g->serving_timer = LONG_MAX;
}

int main(int argc, char *argv[]) {
    // Init variables
    SDL_Renderer *renderer = NULL;
    SDL_Window *window = NULL;
    SDL_Texture *texture = NULL;
    GameState g;
    init_game_state(&g);

    // time sync variables
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
    SDL_DisplayMode display_mode;
    SDL_GetCurrentDisplayMode(0, &display_mode);
    int render_flags = SDL_RENDERER_ACCELERATED;
    if (display_mode.refresh_rate == g.target_fps) {
        render_flags |= SDL_RENDERER_PRESENTVSYNC;
        g.vsync = true;
    }
    renderer = SDL_CreateRenderer(window, -1, render_flags);
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
    // Init mouse
    warp_mouse(window, &g);
    // Info to console
    printf("Resolution: (%d, %d)\n", display_mode.w, display_mode.h);
    // printf("Pixel format: %u\n", current.format);
    printf("Refresh rate: %d\n", display_mode.refresh_rate);
    SDL_RendererInfo info;
    SDL_GetRendererInfo(renderer, &info);
    printf("VSync enabled: %u\n", g.vsync);
    printf("Renderer is using software fallback: %u\n",
           ((info.flags & SDL_RENDERER_SOFTWARE) == SDL_RENDERER_SOFTWARE));
    printf(
        "Renderer is hardware accelerated: %d\n",
        ((info.flags & SDL_RENDERER_ACCELERATED) == SDL_RENDERER_ACCELERATED));
    printf("Renderer supports rendering to texture: %d\n",
           ((info.flags & SDL_RENDERER_TARGETTEXTURE) ==
            SDL_RENDERER_TARGETTEXTURE));
    // Begin event loop
    // int vsync = ((info.flags & SDL_RENDERER_PRESENTVSYNC) ==
    // SDL_RENDERER_PRESENTVSYNC);

    running = true;
    while (running) {
        tic = SDL_GetTicks64();
        update_display_size(window, &g);
        handle_events(window, &g, &running);
        update(&g);
        draw(renderer, texture, &g);
        toc = SDL_GetTicks64();
        chrono_frame = toc - tic;
        if (!g.vsync) {
            // Software vsync
            unsigned int sleep_time =
                (unsigned int)(millis_per_frame - chrono_frame);
            if (sleep_time > 0 && sleep_time < 1000)
                SDL_Delay(sleep_time);
        }
        g.frame_count++;
    }
    // Exit gracefully.
    SDL_DestroyRenderer(renderer);
    SDL_DestroyTexture(texture);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
