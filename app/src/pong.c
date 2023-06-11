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
#include "typedefs.h"
#include <limits.h>

void init_context(Context *ctx) {
    ctx->window = NULL;
    ctx->renderer = NULL;
    ctx->texture = NULL;
    ctx->color.r = 255;
    ctx->color.g = 255;
    ctx->color.b = 255;
    ctx->vsync = false;
    ctx->vsync_divider = 1;
    ctx->frame_count = 0;
    ctx->target_fps = 60;
    ctx->retro_disp_w = 100;
    ctx->retro_disp_h = 80;
#ifdef __EMSCRIPTEN__
    ctx->fullscreen = false;
    ctx->native_disp_w = 4 * 100;
    ctx->native_disp_h = 4 * 80;
#else
    ctx->fullscreen = false;
    ctx->native_disp_w = 5 * 100;
    ctx->native_disp_h = 5 * 80;
#endif
    ctx->pixel_w = 1;
    ctx->pixel_h = 1;
    // Game parameters
    ctx->game_mode = MODE_INTRO;
    ctx->running = false;
    init_ball(ctx);
    init_paddles(ctx);
    // Player parameters
    ctx->number_of_players = 0;
    ctx->left_player_serving = false;
    ctx->right_player_serving = false;
    ctx->start_game_serving = false;
    ctx->serving_timer = 0;
    ctx->serving_duration = 0;
    // Score parameters
    init_score(ctx);
    // Sound parameters
    ctx->sound_activated = false;
    // HID parameters
    ctx->mouse.x = 0;
    ctx->mouse.y = 0;
}

void init_ball(Context *ctx) {
    ctx->ball_speed = ctx->pixel_w; // * 60 / ctx->target_fps;
    ctx->ball.rect.w = ctx->pixel_w;
    ctx->ball.rect.h = ctx->pixel_h;
    ctx->ball.rect.x = ctx->retro_disp_w / 4 + rand() % ctx->retro_disp_w / 2;
    ctx->ball.rect.y = ctx->retro_disp_h / 4 + rand() % ctx->retro_disp_h / 2;
    ctx->ball.vx = 1;
    ctx->ball.vy = 1;
}

void init_paddles(Context *ctx) {
    ctx->paddle_speed = ctx->ball_speed * 2;
    ctx->left_paddle.rect.w = ctx->pixel_w;
    ctx->left_paddle.rect.h = ctx->pixel_h * 5;
    ctx->left_paddle.rect.x = 9 * ctx->pixel_w;
    ctx->left_paddle.rect.y =
        ctx->retro_disp_h / 2 - ctx->left_paddle.rect.h / 2;
    ctx->left_paddle.kb_vy = 0;
    ctx->left_paddle.ms_vy = 0;
    ctx->left_paddle.normal = 1;
    ctx->right_paddle.rect.w = ctx->left_paddle.rect.w;
    ctx->right_paddle.rect.h = ctx->left_paddle.rect.h;
    ctx->right_paddle.rect.x =
        ctx->retro_disp_w - ctx->left_paddle.rect.x - ctx->right_paddle.rect.w;
    ctx->right_paddle.rect.y = ctx->left_paddle.rect.y;
    ctx->right_paddle.kb_vy = 0;
    ctx->right_paddle.ms_vy = 0;
    ctx->right_paddle.normal = -1;
}

void init_score(Context *ctx) {
    ctx->left_score = 0;
    ctx->right_score = 0;
    ctx->left_score_pos.x =
        ctx->left_paddle.rect.x + 3 * ctx->pixel_w + 8 + ctx->pixel_w;
    ctx->left_score_pos.y = 2 * ctx->pixel_h;
    ctx->right_score_pos.x = ctx->right_paddle.rect.x - 5 * ctx->pixel_w;
    ctx->right_score_pos.y = 2 * ctx->pixel_h;
}

void init_stage_intro(Context *ctx) {
    init_ball(ctx);
    // TODO: factor out static values for ball, paddle, etc.
    ctx->serving_duration = 100;
    ctx->serving_timer = SDL_GetTicks64() + ctx->serving_duration;
    ctx->left_player_serving = false;
    ctx->right_player_serving = false;
    ctx->start_game_serving = false;
}

void init_stage_playing(Context *ctx) {
    init_paddles(ctx);
    init_ball(ctx);
    init_score(ctx);
    ctx->ball.rect.x = ctx->native_disp_w / 2;
    ctx->ball.rect.y = ctx->native_disp_h / 2;
    ctx->number_of_players = 0;
    ctx->serving_duration = 2000;
    ctx->serving_timer = SDL_GetTicks64() + ctx->serving_duration;
    ctx->left_player_serving = false;
    ctx->right_player_serving = false;
    ctx->start_game_serving = true;
}

void update(Context *ctx) {
    switch (ctx->game_mode) {
    case MODE_INTRO:
        update_mode_intro(ctx);
        break;
    case MODE_PLAYING:
        update_mode_playing(ctx);
        break;
    }
}

void update_agent(Context *ctx, Paddle *paddle, int max_jitter) {
    // method 1:
    // ball dir: 1=left attacking, -1=right attacking
    int ball_dir = (ctx->ball.vx > 0) ? 1 : ((ctx->ball.vx < 0) ? -1 : 0);
    // 1=even frame, 0=odd frame
    int even_frame =
        (int)(ctx->frame_count % (2 * ctx->vsync_divider)) == 0 ? 1 : 0;
    // Let agent move slower when attacking, faster when defending.
    int move = (ball_dir * paddle->normal * even_frame == 1) ? 0 : 1;
    // Distance between pad and ball.
    int distance = (paddle->rect.y + paddle->rect.h / 2) -
                   (ctx->ball.rect.y + ctx->ball.rect.h / 2);
    // Don't let the ball pass by when heading straight to the pad.
    if (abs(ctx->ball.vy) == 0) {
        max_jitter = (max_jitter > 2) ? 1 : max_jitter;
    }
    if (distance < -1 * max_jitter) {
        paddle->kb_vy = ctx->ball_speed * move;
        paddle->ms_vy = 0;
    } else if (distance > 1 * max_jitter) {
        paddle->kb_vy = -ctx->ball_speed * move;
        paddle->ms_vy = 0;
    } else {
        paddle->kb_vy = 0;
        paddle->ms_vy = 0;
    }
}

void update_ball_position(Context *ctx) {
    // Update ball position
    ctx->ball.rect.x += ctx->ball.vx;
    ctx->ball.rect.y += ctx->ball.vy;
    // Bounce agaist horizontal walls.
    if (ctx->ball.rect.y <= 0) {
        ctx->ball.vy = -ctx->ball.vy;
        ctx->ball.rect.y = 0;
        play_bounce(ctx);
    }
    if (ctx->ball.rect.y >= (ctx->retro_disp_h - ctx->ball.rect.h)) {
        ctx->ball.vy = -ctx->ball.vy;
        ctx->ball.rect.y = ctx->retro_disp_h - ctx->ball.rect.h;
        play_bounce(ctx);
    }
}

void update_bouncing_ball(Context *ctx) {
    // Update ball position
    ctx->ball.rect.x += ctx->ball.vx;
    ctx->ball.rect.y += ctx->ball.vy;
    // Bounce agaist horizontal walls.
    if (ctx->ball.rect.y <= 0) {
        ctx->ball.vy = -ctx->ball.vy;
        ctx->ball.rect.y = 0;
    }
    if (ctx->ball.rect.y >= (ctx->retro_disp_h - ctx->ball.rect.h)) {
        ctx->ball.vy = -ctx->ball.vy;
        ctx->ball.rect.y = ctx->retro_disp_h - ctx->ball.rect.h;
    }
    if (ctx->ball.rect.x <= 0) {
        ctx->ball.vx = -ctx->ball.vx;
        ctx->ball.rect.x = 0;
    }
    if (ctx->ball.rect.x >= (ctx->retro_disp_w - ctx->ball.rect.w)) {
        ctx->ball.vx = -ctx->ball.vx;
        ctx->ball.rect.x = ctx->retro_disp_w - ctx->ball.rect.w;
    }
}

void update_collision(Context *ctx, Paddle *paddle) {
    if (SDL_HasIntersection(&ctx->ball.rect, &paddle->rect)) {
        // Change direction of the ball in opposite direction.
        int sign = (ctx->ball.vx > 0) ? -1 : ((ctx->ball.vx < 0) ? 1 : 0);
        // Detect where the ball has hit the paddle, to determine the bounce
        // angle.
        int rel = (paddle->rect.y + paddle->rect.h / 2) -
                  (ctx->ball.rect.y + ctx->ball.rect.h / 2);
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
        ctx->ball.vy = -(rel / ctx->pixel_h + mid_y) * ctx->ball_speed;
        ctx->ball.vx = sign * (1 + mid_x) * ctx->ball_speed;
        ctx->ball.rect.x = paddle->rect.x + paddle->rect.w;
        // Correct the position of the ball to prevent the ball being stuck
        // in the middle of the paddle.
        if (sign > 0) {
            ctx->ball.rect.x =
                ctx->left_paddle.rect.x + ctx->left_paddle.rect.w;
        } else {
            ctx->ball.rect.x = ctx->right_paddle.rect.x - ctx->ball.rect.w;
        }
        play_pad(ctx);
    }
}

void update_collision_detections(Context *ctx) {
    update_collision(ctx, &(ctx->left_paddle));
    update_collision(ctx, &(ctx->right_paddle));
}

// Update display size in case the user has changed it.
void update_display_size(Context *ctx) {
    SDL_GetWindowSize(ctx->window, &ctx->native_disp_w, &ctx->native_disp_h);
    SDL_ShowCursor(SDL_DISABLE);
}

void update_left_agent(Context *ctx) {
    int jitter = 0;
    int delta = (ctx->left_score - ctx->right_score);
    if (delta > 0) {
        jitter = (delta < 7) ? delta : 7;
    }
    update_agent(ctx, &(ctx->left_paddle), jitter);
}

void update_right_agent(Context *ctx) {
    int jitter = 0;
    int delta = (ctx->right_score - ctx->left_score);
    if (delta > 0) {
        jitter = (delta < 7) ? delta : 7;
    }
    update_agent(ctx, &(ctx->right_paddle), jitter);
}

void update_mode_intro(Context *ctx) { update_bouncing_ball(ctx); }

void update_mode_playing(Context *ctx) {
    update_ball_position(ctx);
    if (ctx->number_of_players == 0 || ctx->number_of_players == 1) {
        update_right_agent(ctx);
    }
    if (ctx->number_of_players == 0) {
        update_left_agent(ctx);
    }
    update_paddles(ctx);
    update_scores(ctx);
    update_collision_detections(ctx);
}

void update_paddle(Context *ctx, Paddle *paddle) {
    paddle->rect.y += paddle->kb_vy + paddle->ms_vy;
    // Prevent the paddles from going off-screen
    if (paddle->rect.y > (ctx->retro_disp_h - paddle->rect.h))
        paddle->rect.y = (ctx->retro_disp_h - paddle->rect.h);
    if (paddle->rect.y < 0)
        paddle->rect.y = 0;
}

void update_paddles(Context *ctx) {
    update_paddle(ctx, &(ctx->left_paddle));
    update_paddle(ctx, &(ctx->right_paddle));
}

void update_scores(Context *ctx) {
    // Point scored by one of the players.
    if (ctx->ball.rect.x <= 0) {
        ctx->right_score++;
        ctx->ball.vx = 0;
        ctx->ball.vy = 0;
        ctx->left_player_serving = true;
        ctx->serving_timer = SDL_GetTicks64() + ctx->serving_duration;
        play_score(ctx);
    }
    if (ctx->ball.rect.x >= (ctx->retro_disp_w - ctx->ball.rect.w)) {
        ctx->left_score++;
        ctx->ball.vx = 0;
        ctx->ball.vy = 0;
        ctx->right_player_serving = true;
        ctx->serving_timer = SDL_GetTicks64() + ctx->serving_duration;
        play_score(ctx);
    }
    if (ctx->left_player_serving) {
        ctx->ball.rect.x = ctx->left_paddle.rect.x + ctx->left_paddle.rect.w;
        ctx->ball.rect.y = ctx->left_paddle.rect.y +
                           ctx->right_paddle.rect.h / 2 + ctx->ball.rect.h / 2;
    }
    if (ctx->right_player_serving) {
        ctx->ball.rect.x = ctx->right_paddle.rect.x - ctx->ball.rect.w;
        ctx->ball.rect.y = ctx->right_paddle.rect.y +
                           ctx->right_paddle.rect.h / 2 + ctx->ball.rect.h / 2;
    }
}

void draw(Context *ctx) {
    // Set renderer to the offline texture.
    SDL_SetRenderTarget(ctx->renderer, ctx->texture);
    // Set color and clear the render context.
    SDL_SetRenderDrawColor(ctx->renderer, 0x00, 0x00, 0x00, 0xff);
    SDL_RenderClear(ctx->renderer);
    // Set the current render color.
    SDL_SetRenderDrawColor(ctx->renderer, ctx->color.r, ctx->color.g,
                           ctx->color.b, 255);
    if (ctx->game_mode == MODE_INTRO) {
        draw_stage_intro(ctx);
    }
    if (ctx->game_mode == MODE_PLAYING) {
        draw_stage_playing(ctx);
    }
    // Set ctx->renderer to the screen and copy/scale the texture on screen.
    SDL_SetRenderTarget(ctx->renderer, NULL);
    SDL_RenderCopy(ctx->renderer, ctx->texture, NULL, NULL);
    SDL_RenderPresent(ctx->renderer);
}

void draw_digits(Context *ctx, int digits, int x0, int y0) {
    digits = digits % 1000;
    int i0 = (int)(digits % 10);
    int i10 = (int)((digits / 10) % 10);
    int i100 = (int)((digits / 100) % 10);
    draw_digit(ctx, i0, x0, y0);
    if (digits > 9)
        draw_digit(ctx, i10, x0 - 8, y0);
    if (digits > 99)
        draw_digit(ctx, i100, x0 - 16, y0);
}

void draw_stage_intro(Context *ctx) {
    SDL_RenderFillRect(ctx->renderer, &ctx->ball.rect);
}

void draw_stage_playing(Context *ctx) {
    SDL_RenderFillRect(ctx->renderer, &ctx->left_paddle.rect);
    SDL_RenderFillRect(ctx->renderer, &ctx->right_paddle.rect);
    SDL_RenderFillRect(ctx->renderer, &ctx->ball.rect);
    // Render net
    SDL_Rect block;
    // block.w = ctx->pixel_w / 4;
    block.w = ctx->pixel_w;
    block.h = ctx->pixel_h;
    block.x = ctx->retro_disp_w / 2 - block.w / 2;
    for (int i = 0; i < ctx->native_disp_h; i += 3 * ctx->pixel_h) {
        block.y = i;
        SDL_RenderFillRect(ctx->renderer, &block);
    }
    // Draw scores
    draw_digits(ctx, ctx->left_score, ctx->left_score_pos.x,
                ctx->left_score_pos.y);
    draw_digits(ctx, ctx->right_score, ctx->right_score_pos.x,
                ctx->right_score_pos.y);
}

void main_loop(void *arg) {
    Context *ctx = (Context *)arg;
    if (ctx->frame_count % ctx->vsync_divider == 0) {
        update_display_size(ctx);
        handle_events(ctx);
        update(ctx);
    }
    draw(ctx);
    ctx->frame_count++;
}

void handle_events(Context *ctx) {
    SDL_Event event;
    // Handle keyboard events
    while (SDL_PollEvent(&event)) {
        const unsigned char *keystates = SDL_GetKeyboardState(NULL);
        int x = 0;
        int y = 0;
        const unsigned int button_state = SDL_GetMouseState(&x, &y);
        if (event.type == SDL_KEYDOWN) {
            if (keystates[SDL_SCANCODE_UP] || keystates[SDL_SCANCODE_W]) {
                ctx->left_paddle.kb_vy = -ctx->paddle_speed;
                ctx->mouse.needs_warp = true;
            }
            if (keystates[SDL_SCANCODE_DOWN] || keystates[SDL_SCANCODE_S]) {
                ctx->left_paddle.kb_vy = ctx->paddle_speed;
                ctx->mouse.needs_warp = true;
            }
            if (keystates[SDL_SCANCODE_U]) {
                ctx->right_paddle.kb_vy = -ctx->paddle_speed;
            }
            if (keystates[SDL_SCANCODE_J]) {
                ctx->right_paddle.kb_vy = ctx->paddle_speed;
            }
            if (keystates[SDL_SCANCODE_X] && ctx->left_player_serving) {
                launch_ball(ctx, 1);
            }
            if (keystates[SDL_SCANCODE_M] && ctx->right_player_serving) {
                launch_ball(ctx, -1);
            }
            if (keystates[SDLK_MINUS] || keystates[SDL_SCANCODE_MINUS]) {
                if (ctx->vsync_divider < 5)
                    ctx->vsync_divider++;
                SDL_Log("vsync_divider: %d", ctx->vsync_divider);
            }
            if (keystates[SDLK_PLUS] || keystates[SDL_SCANCODE_EQUALS]) {
                if (ctx->vsync_divider > 1)
                    ctx->vsync_divider--;
                SDL_Log("vsync_divider: %d", ctx->vsync_divider);
            }
            if (keystates[SDL_SCANCODE_R]) {
                init_stage_playing(ctx);
            }
            if (keystates[SDL_SCANCODE_0]) {
                switch_game_mode(ctx, MODE_PLAYING);
                ctx->number_of_players = 0;
            }
            if (keystates[SDL_SCANCODE_1]) {
                ctx->game_mode = MODE_PLAYING;
                init_stage_playing(ctx);
                ctx->number_of_players = 1;
            }
            if (keystates[SDL_SCANCODE_2]) {
                ctx->game_mode = MODE_PLAYING;
                init_stage_playing(ctx);
                ctx->number_of_players = 2;
            }
        }
        if (event.type == SDL_KEYUP) {
            if (!keystates[SDL_SCANCODE_UP] && ctx->left_paddle.kb_vy < 0) {
                ctx->left_paddle.kb_vy = 0;
                ctx->mouse.needs_warp = true;
            }
            if (!keystates[SDL_SCANCODE_DOWN] && ctx->left_paddle.kb_vy > 0) {
                ctx->left_paddle.kb_vy = 0;
                ctx->mouse.needs_warp = true;
            }
            if (!keystates[SDL_SCANCODE_U] && ctx->right_paddle.kb_vy < 0) {
                ctx->right_paddle.kb_vy = 0;
            }
            if (!keystates[SDL_SCANCODE_J] && ctx->right_paddle.kb_vy > 0) {
                ctx->right_paddle.kb_vy = 0;
            }
        }
#ifndef __EMSCRIPTEN__
        if (keystates[SDL_SCANCODE_F]) {
            unsigned int flags = 0;
            ctx->fullscreen = !ctx->fullscreen;
            if (ctx->fullscreen)
                flags = SDL_WINDOW_FULLSCREEN;
            SDL_SetWindowFullscreen(ctx->window, flags);
        }
#endif
        if (event.type == SDL_QUIT || keystates[SDL_SCANCODE_ESCAPE] ||
            keystates[SDL_SCANCODE_Q]) {
            ctx->running = false;
        }
        // -------------------------------------------------------------------
        if (event.type == SDL_MOUSEBUTTONDOWN) {
            if (ctx->game_mode == MODE_INTRO) {
                switch_game_mode(ctx, MODE_PLAYING);
                init_stage_playing(ctx);
                ctx->number_of_players = 1;
            }
            if (button_state == SDL_BUTTON_LEFT && ctx->left_player_serving) {
                launch_ball(ctx, 1);
            }
        }
    }
    // Allow mouse and keyboard input at the same time. Keep the mouse position
    // in sync.
    if (ctx->mouse.needs_warp) {
        warp_mouse(ctx);
    }
    // Scale mouse movement to the render texture.
    int x = 0;
    int y = 0;
    SDL_GetMouseState(&x, &y);
    native_to_retro_disp(ctx, &x, &y);
    ctx->left_paddle.ms_vy = y - ctx->mouse.y;
    ctx->mouse.y = y;
    // Trigger serving after timeout
    if (ctx->start_game_serving && SDL_GetTicks64() > ctx->serving_timer) {
        int direction = (rand() % 2 == 0) ? 1 : -1;
        launch_ball(ctx, direction);
    }
    if (ctx->left_player_serving && SDL_GetTicks64() > ctx->serving_timer) {
        launch_ball(ctx, 1);
    }
    if (ctx->right_player_serving && SDL_GetTicks64() > ctx->serving_timer) {
        launch_ball(ctx, -1);
    }
}

void launch_ball(Context *ctx, int direction) {
    ctx->ball.vy = ((rand() % 2) - 1) * ctx->ball_speed;
    ctx->ball.vx = direction * ctx->ball_speed;
    ctx->left_player_serving = false;
    ctx->right_player_serving = false;
    ctx->start_game_serving = false;
    ctx->serving_timer = ULONG_MAX;
}

void retro_to_native_disp(Context *ctx, int *x, int *y) {
    *x = (*x * ctx->native_disp_w) / ctx->retro_disp_w;
    *y = (*y * ctx->native_disp_h) / ctx->retro_disp_h;
}

void native_to_retro_disp(Context *ctx, int *x, int *y) {
    *x = (*x * ctx->retro_disp_w) / ctx->native_disp_w;
    *y = (*y * ctx->retro_disp_h) / ctx->native_disp_h;
}

void switch_game_mode(Context *ctx, enum GameMode target_game_mode) {
    ctx->game_mode = target_game_mode;
    if (ctx->game_mode == MODE_PLAYING) {
        init_stage_playing(ctx);
        if (ctx->sound_activated == false) {
            ctx->sound_activated = true;
            init_sound();
            load_sound(ctx);
        }
    } else if (ctx->game_mode == MODE_INTRO) {
        init_stage_intro(ctx);
    }
}

void warp_mouse(Context *ctx) {
    ctx->mouse.y = ctx->left_paddle.rect.y + ctx->left_paddle.rect.h / 2;
    int x = 0;
    int y = ctx->mouse.y;
    retro_to_native_disp(ctx, &x, &y);
    SDL_WarpMouseInWindow(ctx->window, ctx->native_disp_w / 2, y);
    ctx->left_paddle.ms_vy = 0;
    ctx->mouse.needs_warp = false;
}

int main(int argc, char *argv[]) {
    // Init variables
    SDL_Renderer *renderer = NULL;
    SDL_Window *window = NULL;
    SDL_Texture *texture = NULL;
    Context ctx;
    init_context(&ctx);
    init_stage_intro(&ctx);

    int status = SDL_Init(SDL_INIT_VIDEO);
    if (status < 0) {
        SDL_Log("Failed at SDL_Init()");
        exit(status);
    }
    // Init window and renderer
    window = SDL_CreateWindow("Pong", SDL_WINDOWPOS_CENTERED,
                              SDL_WINDOWPOS_CENTERED, ctx.native_disp_w,
                              ctx.native_disp_h, SDL_WINDOW_SHOWN);
    if (ctx.fullscreen) {
        SDL_SetWindowFullscreen(window, SDL_WINDOW_FULLSCREEN);
    }
    SDL_DisplayMode display_mode;
    SDL_GetCurrentDisplayMode(0, &display_mode);
    int render_flags = SDL_RENDERER_ACCELERATED;
#ifndef __EMSCRIPTEN__
    if (display_mode.refresh_rate == ctx.target_fps) {
        render_flags |= SDL_RENDERER_PRESENTVSYNC;
        ctx.vsync = true;
    }
#endif
    renderer = SDL_CreateRenderer(window, -1, render_flags);
    texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_ARGB8888,
                                SDL_TEXTUREACCESS_TARGET, ctx.retro_disp_w,
                                ctx.retro_disp_h);
    ctx.window = window;
    ctx.renderer = renderer;
    ctx.texture = texture;
    update_display_size(&ctx);
    // Init mouse
    warp_mouse(&ctx);
    // Info to console
    SDL_Log("Resolution: (%d, %d)\n", display_mode.w, display_mode.h);
    SDL_Log("Native resolution (%d, %d)\n", ctx.native_disp_w,
            ctx.native_disp_h);
    SDL_Log("Refresh rate: %d\n", display_mode.refresh_rate);
    SDL_RendererInfo info;
    SDL_GetRendererInfo(renderer, &info);
    SDL_Log("VSync enabled: %u\n", ctx.vsync);
    SDL_Log("Renderer is using software fallback: %u\n",
            ((info.flags & SDL_RENDERER_SOFTWARE) == SDL_RENDERER_SOFTWARE));
    SDL_Log(
        "Renderer is hardware accelerated: %d\n",
        ((info.flags & SDL_RENDERER_ACCELERATED) == SDL_RENDERER_ACCELERATED));
    SDL_Log("Renderer supports rendering to texture: %d\n",
            ((info.flags & SDL_RENDERER_TARGETTEXTURE) ==
             SDL_RENDERER_TARGETTEXTURE));
    ctx.running = true;
#ifdef __EMSCRIPTEN__
    // Uses the browser’s requestAnimationFrame mechanism to call the function.
    int fps = 0;
    // If true, this function will throw an exception in order to stop execution
    // of the caller.
    int simulate_infinite_loop = (int)ctx.running;
    // Set main loop with arguments.
    emscripten_set_main_loop_arg(main_loop, &ctx, fps, simulate_infinite_loop);
    // Updates are performed using the requestAnimationFrame function (with
    // vsync enabled)
    emscripten_set_main_loop_timing(EM_TIMING_RAF, 1);
#else
    while (ctx.running) {
        unsigned long tic = 0;
        unsigned long toc = 0;
        unsigned long chrono_frame = 0;
        unsigned long millis_per_frame = 1000 / ctx.target_fps;
        tic = SDL_GetTicks64();
        main_loop(&ctx);
        toc = SDL_GetTicks64();
        chrono_frame = toc - tic;
        if (chrono_frame > 30)
            SDL_Log("chrono: %lu\n", chrono_frame);
        if (!ctx.vsync) {
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
    ctx.renderer = NULL;
    ctx.window = NULL;
    ctx.texture = NULL;
    renderer = NULL;
    window = NULL;
    texture = NULL;
    SDL_Quit();

    return 0;
}
