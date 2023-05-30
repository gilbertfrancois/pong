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
#include "digit.h"
#include "sound.h"
#include <SDL_keycode.h>
#include <SDL_scancode.h>
#include <limits.h>
#include <stdint.h>

void handle_events(GameState *g) {
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
                launch_ball(g, 1);
            }
            if (keystates[SDL_SCANCODE_M] && g->right_player_serving) {
                launch_ball(g, -1);
            }
            if (keystates[SDL_SCANCODE_F]) {
                unsigned int flags = 0;
                g->fullscreen = !g->fullscreen;
                if (g->fullscreen)
                    flags = SDL_WINDOW_FULLSCREEN;
                SDL_SetWindowFullscreen(g->window, flags);
            }
            if (keystates[SDLK_MINUS] || keystates[SDLK_KP_MINUS] ||
                keystates[SDL_SCANCODE_MINUS]) {
                if (g->vsync_divider < 5)
                    g->vsync_divider++;
                SDL_Log("vsync_divider: %d", g->vsync_divider);
            }
            if (keystates[SDLK_PLUS] || keystates[SDLK_KP_PLUS] ||
                keystates[SDL_SCANCODE_EQUALS]) {
                if (g->vsync_divider > 1)
                    g->vsync_divider--;
                SDL_Log("vsync_divider: %d", g->vsync_divider);
            }
            if (keystates[SDL_SCANCODE_R]) {
                init_players_state(g);
            }
            if (keystates[SDL_SCANCODE_0]) {
                init_players_state(g);
                g->number_of_players = 0;
            }
            if (keystates[SDL_SCANCODE_1]) {
                init_players_state(g);
                g->number_of_players = 1;
            }
            if (keystates[SDL_SCANCODE_2]) {
                init_players_state(g);
                g->number_of_players = 2;
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
        if (event.type == SDL_QUIT || keystates[SDL_SCANCODE_ESCAPE] ||
            keystates[SDL_SCANCODE_Q]) {
            g->running = false;
        }
        // -------------------------------------------------------------------
        if (event.type == SDL_MOUSEBUTTONDOWN) {
            if (button_state == SDL_BUTTON_LEFT && g->left_player_serving) {
                launch_ball(g, 1);
            }
        }
    }
    // Allow mouse and keyboard input at the same time. Keep the mouse position
    // in sync.
    if (g->mouse.needs_warp) {
        warp_mouse(g);
    }
    // Scale mouse movement to the render texture.
    int x = 0;
    int y = 0;
    SDL_GetMouseState(&x, &y);
    native_to_retro_disp(g, &x, &y);
    g->left_paddle.ms_vy = y - g->mouse.y;
    g->mouse.y = y;
    // Trigger serving after timeout
    if (g->start_game_serving && SDL_GetTicks64() > g->serving_timer) {
        int direction = (arc4random() % 2 == 0) ? 1 : -1;
        launch_ball(g, direction);
    }
    if (g->left_player_serving && SDL_GetTicks64() > g->serving_timer) {
        launch_ball(g, 1);
    }
    if (g->right_player_serving && SDL_GetTicks64() > g->serving_timer) {
        launch_ball(g, -1);
    }
}

void draw_digits(GameState *g, int digits, int x0, int y0) {
    digits = digits % 1000;
    int i0 = (int)(digits % 10);
    int i10 = (int)((digits / 10) % 10);
    int i100 = (int)((digits / 100) % 10);
    draw_digit(g, i0, x0, y0);
    if (digits > 9)
        draw_digit(g, i10, x0 - 8, y0);
    if (digits > 99)
        draw_digit(g, i100, x0 - 16, y0);
}

void retro_to_native_disp(GameState *g, int *x, int *y) {
    *x = (*x * g->native_disp_w) / g->retro_disp_w;
    *y = (*y * g->native_disp_h) / g->retro_disp_h;
}

void native_to_retro_disp(GameState *g, int *x, int *y) {
    *x = (*x * g->retro_disp_w) / g->native_disp_w;
    *y = (*y * g->retro_disp_h) / g->native_disp_h;
}

void warp_mouse(GameState *g) {
    g->mouse.y = g->left_paddle.rect.y + g->left_paddle.rect.h / 2;
    int x = 0;
    int y = g->mouse.y;
    retro_to_native_disp(g, &x, &y);
    SDL_WarpMouseInWindow(g->window, g->native_disp_w / 2, y);
    g->left_paddle.ms_vy = 0;
    g->mouse.needs_warp = false;
}

// Update display size in case the user has changed it.
void update_display_size(GameState *g) {
    SDL_GetWindowSize(g->window, &g->native_disp_w, &g->native_disp_h);
    SDL_ShowCursor(SDL_DISABLE);
}

void update_agent(GameState *g, Paddle *paddle, int max_jitter) {
    // method 1:
    // ball dir: 1=left attacking, -1=right attacking
    int ball_dir = (g->ball.vx > 0) ? 1 : ((g->ball.vx < 0) ? -1 : 0);
    // 1=even frame, 0=odd frame
    int even_frame = (int)(g->frame_count % (2*g->vsync_divider)) == 0 ? 1 : 0;
    // Let agent move slower when attacking, faster when defending.
    int move = (ball_dir * paddle->normal * even_frame == 1) ? 0 : 1;
    // Distance between pad and ball.
    int distance = (paddle->rect.y + paddle->rect.h / 2) -
                   (g->ball.rect.y + g->ball.rect.h / 2);
    // Don't let the ball pass by when heading straight to the pad.
    if (abs(g->ball.vy) == 0) {
        max_jitter = (max_jitter > 2) ? 1 : max_jitter;
    }
    if (distance < -1 * max_jitter) {
        paddle->kb_vy = g->ball_speed * move;
        paddle->ms_vy = 0;
    } else if (distance > 1 * max_jitter) {
        paddle->kb_vy = -g->ball_speed * move;
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
    // Bounce agaist horizontal walls.
    if (g->ball.rect.y <= 0) {
        g->ball.vy = -g->ball.vy;
        g->ball.rect.y = 0;
        play_bounce(g);
    }
    if (g->ball.rect.y >= (g->retro_disp_h - g->ball.rect.h)) {
        g->ball.vy = -g->ball.vy;
        g->ball.rect.y = g->retro_disp_h - g->ball.rect.h;
        play_bounce(g);
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
        play_score(g);
    }
    if (g->ball.rect.x >= (g->retro_disp_w - g->ball.rect.w)) {
        g->left_score++;
        g->ball.vx = 0;
        g->ball.vy = 0;
        g->right_player_serving = true;
        g->serving_timer = SDL_GetTicks64() + g->serving_duration;
        play_score(g);
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
            mid_y = arc4random() % 3 - 1;
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
        play_pad(g);
    }
}

void update_collision_detections(GameState *g) {
    update__collision(g, &(g->left_paddle));
    update__collision(g, &(g->right_paddle));
}

void update(GameState *g) {
    update_ball_position(g);
    if (g->number_of_players == 0 || g->number_of_players == 1) {
        update_right_agent(g);
    }
    if (g->number_of_players == 0) {
        update_left_agent(g);
    }
    update_paddles(g);
    update_scores(g);
    update_collision_detections(g);
}

void draw(GameState *g) {
    // Set renderer to the offline texture.
    SDL_SetRenderTarget(g->renderer, g->texture);
    // Set color and clear the render context.
    SDL_SetRenderDrawColor(g->renderer, 0x00, 0x00, 0x00, 0xff);
    SDL_RenderClear(g->renderer);
    // Set the current render color.
    SDL_SetRenderDrawColor(g->renderer, g->color.r, g->color.g, g->color.b,
                           255);
    SDL_RenderFillRect(g->renderer, &g->left_paddle.rect);
    SDL_RenderFillRect(g->renderer, &g->right_paddle.rect);
    SDL_RenderFillRect(g->renderer, &g->ball.rect);
    // Render net
    SDL_Rect block;
    // block.w = g->pixel_w / 4;
    block.w = g->pixel_w;
    block.h = g->pixel_h;
    block.x = g->retro_disp_w / 2 - block.w / 2;
    for (int i = 0; i < g->native_disp_h; i += 3 * g->pixel_h) {
        block.y = i;
        SDL_RenderFillRect(g->renderer, &block);
    }
    // Draw scores
    draw_digits(g, g->left_score, g->left_score_pos.x, g->left_score_pos.y);
    draw_digits(g, g->right_score, g->right_score_pos.x, g->right_score_pos.y);
    // Set g->renderer to the screen and copy/scale the texture on screen.
    SDL_SetRenderTarget(g->renderer, NULL);
    SDL_RenderCopy(g->renderer, g->texture, NULL, NULL);
    SDL_RenderPresent(g->renderer);
}

void init_game_state(GameState *g) {
    g->running = false;
    g->vsync = false;
    g->vsync_divider = 1;
    g->target_fps = 60;
    g->retro_disp_w = 100;
    g->retro_disp_h = 80;
#ifdef __EMSCRIPTEN__
    g->fullscreen = false;
    g->native_disp_w = 4 * 100;
    g->native_disp_h = 4 * 80;
#else
    g->fullscreen = false;
    g->native_disp_w = 5 * 100;
    g->native_disp_h = 5 * 80;
#endif
    SDL_Log("native resolution (%d, %d)\n", g->native_disp_w, g->native_disp_h);
    g->pixel_w = 1;
    g->pixel_h = 1;
    g->ball_speed = g->pixel_w; // * 60 / g->target_fps;
    g->paddle_speed = g->ball_speed * 2;
    g->color.r = 255;
    g->color.g = 255;
    g->color.b = 255;
}

void init_players_state(GameState *g) {
    g->left_paddle.rect.w = g->pixel_w;
    g->left_paddle.rect.h = g->pixel_h * 5;
    g->left_paddle.rect.x = 9 * g->pixel_w;
    g->left_paddle.rect.y = g->retro_disp_h / 2 - g->left_paddle.rect.h / 2;
    g->left_paddle.kb_vy = 0;
    g->left_paddle.ms_vy = 0;
    g->left_paddle.normal = 1;
    g->right_paddle.rect.w = g->left_paddle.rect.w;
    g->right_paddle.rect.h = g->left_paddle.rect.h;
    g->right_paddle.rect.x =
        g->retro_disp_w - g->left_paddle.rect.x - g->right_paddle.rect.w;
    g->right_paddle.rect.y = g->left_paddle.rect.y;
    g->right_paddle.kb_vy = 0;
    g->right_paddle.ms_vy = 0;
    g->right_paddle.normal = -1;
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
    g->ball.vx = 0;
    g->ball.vy = 0;
    g->number_of_players = 0;
    g->serving_duration = 2000;
    g->serving_timer = SDL_GetTicks64() + g->serving_duration;
    g->left_player_serving = false;
    g->right_player_serving = false;
    g->start_game_serving = true;
}

void launch_ball(GameState *g, int direction) {
    g->ball.vy = ((arc4random() % 2) - 1) * g->ball_speed;
    g->ball.vx = direction * g->ball_speed;
    g->left_player_serving = false;
    g->right_player_serving = false;
    g->start_game_serving = false;
    g->serving_timer = ULONG_MAX;
}

void main_loop(void *arg) {
    GameState *g = (GameState *)arg;
    if (g->frame_count % g->vsync_divider == 0) {
        update_display_size(g);
        handle_events(g);
        update(g);
    }
    draw(g);
    g->frame_count++;
}

int main(int argc, char *argv[]) {
    // Init variables
    SDL_Renderer *renderer = NULL;
    SDL_Window *window = NULL;
    SDL_Texture *texture = NULL;
    GameState g;
    init_game_state(&g);
    init_players_state(&g);

    int status = SDL_Init(SDL_INIT_VIDEO);
    if (status < 0) {
        SDL_Log("Failed at SDL_Init()");
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
#ifndef __EMSCRIPTEN__
    if (display_mode.refresh_rate == g.target_fps) {
        render_flags |= SDL_RENDERER_PRESENTVSYNC;
        g.vsync = true;
    }
#endif
    renderer = SDL_CreateRenderer(window, -1, render_flags);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                                SDL_TEXTUREACCESS_TARGET, g.retro_disp_w,
                                g.retro_disp_h);
    g.window = window;
    g.renderer = renderer;
    g.texture = texture;
    update_display_size(&g);
    init_sound();
    load_sound(&g);
    // Init mouse
    warp_mouse(&g);
    // Info to console
    SDL_Log("Resolution: (%d, %d)\n", display_mode.w, display_mode.h);
    SDL_Log("Refresh rate: %d\n", display_mode.refresh_rate);
    SDL_RendererInfo info;
    SDL_GetRendererInfo(renderer, &info);
    SDL_Log("VSync enabled: %u\n", g.vsync);
    SDL_Log("Renderer is using software fallback: %u\n",
            ((info.flags & SDL_RENDERER_SOFTWARE) == SDL_RENDERER_SOFTWARE));
    SDL_Log(
        "Renderer is hardware accelerated: %d\n",
        ((info.flags & SDL_RENDERER_ACCELERATED) == SDL_RENDERER_ACCELERATED));
    SDL_Log("Renderer supports rendering to texture: %d\n",
            ((info.flags & SDL_RENDERER_TARGETTEXTURE) ==
             SDL_RENDERER_TARGETTEXTURE));
    g.running = true;
#ifdef __EMSCRIPTEN__
    // Uses the browser’s requestAnimationFrame mechanism to call the function.
    int fps = 0;
    // If true, this function will throw an exception in order to stop execution
    // of the caller.
    int simulate_infinite_loop = (int)g.running;
    // Set main loop with arguments.
    emscripten_set_main_loop_arg(main_loop, &g, fps, simulate_infinite_loop);
    // Updates are performed using the requestAnimationFrame function (with
    // vsync enabled)
    emscripten_set_main_loop_timing(EM_TIMING_RAF, 1);
#else
    while (g.running) {
        unsigned long tic = 0;
        unsigned long toc = 0;
        unsigned long chrono_frame = 0;
        unsigned long millis_per_frame = 1000 / g.target_fps;
        tic = SDL_GetTicks64();
        main_loop(&g);
        toc = SDL_GetTicks64();
        chrono_frame = toc - tic;
        if (chrono_frame > 30)
            SDL_Log("chrono: %lu\n", chrono_frame);
        if (!g.vsync) {
            // Software vsync
            unsigned int sleep_time =
                (unsigned int)(millis_per_frame - chrono_frame);
            if (sleep_time > 0 && sleep_time < 1000)
                SDL_Delay(sleep_time);
        }
    }
#endif
    // Exit gracefully.
    SDL_DestroyRenderer(renderer);
    SDL_DestroyTexture(texture);
    SDL_DestroyWindow(window);
    g.renderer = NULL;
    g.window = NULL;
    g.texture = NULL;
    renderer = NULL;
    window = NULL;
    texture = NULL;
    SDL_Quit();

    return 0;
}
