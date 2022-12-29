#include "pong.h"
#include "SDL2/SDL_events.h"
#include "SDL2/SDL_render.h"

void input() {
    SDL_Event e;
    const Uint8 *keystates = SDL_GetKeyboardState(NULL);
    /* left_paddle_vel = 0; */
    /* right_paddle_vel = 0; */
    while (SDL_PollEvent(&e)) {
        if (e.type == SDL_KEYUP) {
            std::cout << "Key up" << std::endl;
            if (!keystates[SDL_SCANCODE_UP] && left_paddle_vel < 0) {
                left_paddle_vel = 0;
            }
            if (!keystates[SDL_SCANCODE_DOWN] && left_paddle_vel > 0) {
                left_paddle_vel = 0;
            }
        }
        if (e.type == SDL_KEYDOWN) {
            std::cout << "Key down" << std::endl;
            if (keystates[SDL_SCANCODE_UP]) {
                left_paddle_vel = -PADDLE_SPEED;
            }
            if (keystates[SDL_SCANCODE_DOWN]) {
                left_paddle_vel = PADDLE_SPEED;
            }
        }
        if (e.type == SDL_QUIT)
            running = false;
        if (keystates[SDL_SCANCODE_ESCAPE])
            running = false;
        if (keystates[SDL_SCANCODE_Q])
            running = false;
    }
}

void update() {
    score = std::to_string(left_score) + "  " + std::to_string(right_score);
    left_paddle.y += left_paddle_vel;
}

void serve() {
    left_paddle.y = right_paddle.y = (HEIGHT / 2) - (left_paddle.h / 2);
    if (turn) {
        ball.x = left_paddle.x + (left_paddle.w * 4);
    } else {
        ball.x = right_paddle.x - (right_paddle.w * 4);
    }
    ball.y = HEIGHT / 2 - (ball.h / 2);
    vel_x = BALL_SPEED / 2.0;
    vel_y = 0.0;
    turn = !turn;
}

void draw() {
    input();
    SDL_SetRenderDrawColor(renderer, 0x00, 0x00, 0x00, 0xff);
    SDL_RenderClear(renderer);
    /* right_paddle.y += 1; */

    frameCount++;
    timerFPS = SDL_GetTicks() - lastFrame;
    if (timerFPS < (1000 / 60)) {
        SDL_Delay((1000 / 60) - timerFPS);
    }
    // Set the current render color.
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, 255);
    // Draw the scores
    drawLabel(score, WIDTH / 2, 2*PXLH);
    SDL_RenderFillRect(renderer, &left_paddle);
    SDL_RenderFillRect(renderer, &right_paddle);
    SDL_RenderFillRect(renderer, &ball);
    // Render net
    SDL_Rect block;
    block.w = block.h = PXLW;
    block.x = WIDTH / 2 - block.w / 2;
    for (int i = 0; i < HEIGHT; i += 8 * PXLH) {
        block.y = i;
        SDL_RenderFillRect(renderer, &block);
    }
    SDL_RenderPresent(renderer);
}

void drawLabel(std::string text, int x, int y) {
    SDL_Surface *surface;
    SDL_Texture *texture;
    const char *t = text.c_str();
    surface = TTF_RenderText_Solid(font, t, color);
    texture = SDL_CreateTextureFromSurface(renderer, surface);
    score_board.w = surface->w;
    score_board.h = surface->h;
    score_board.x = x - surface->w / 2;
    score_board.y = y;
    SDL_FreeSurface(surface);
    SDL_RenderCopy(renderer, texture, NULL, &score_board);
    SDL_DestroyTexture(texture);
}

int main() {
    int status = SDL_Init(SDL_INIT_EVERYTHING);
    if (status < 0) {
        std::cerr << "Failed at SDL_Init()" << std::endl;
        return -1;
    }
    status = SDL_CreateWindowAndRenderer(WIDTH, HEIGHT, 0, &window, &renderer);
    if (status < 0) {
        std::cerr << "Failed at SDL_CreateWindowAndRenderer()" << std::endl;
        return -1;
    }
    TTF_Init();
    font = TTF_OpenFont("atari.ttf", FONT_SIZE);
    color.r = 255;
    color.g = 255;
    color.b = 255;
    left_paddle.w = PXLW * 2;
    left_paddle.h = 24 * PXLH;
    left_paddle.x = 3*PXLW;
    left_paddle.y = HEIGHT / 2 - left_paddle.h / 2;
    right_paddle = left_paddle;
    right_paddle.x = WIDTH - 3*PXLW - right_paddle.w;
    left_paddle.y = 0;
    ball.w = 2 * PXLW;
    ball.h = 2 * PXLW;
    ball.x = WIDTH / 2;
    ball.y = HEIGHT / 2;

    running = true;
    while (running) {
        lastFrame = SDL_GetTicks();
        if (lastFrame >= (lastTime + 1000)) {
            lastTime = lastFrame;
            fps = frameCount;
            frameCount = 0;
        }
        /* std::cout << "FPS: " << fps << std::endl; */
        input();
        update();
        draw();
    }

    std::cout << "Success" << std::endl;
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
