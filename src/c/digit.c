#include "digit.h"

void draw_segment(GameState *g, int x0, int y0, int x, int y, int w, int h) {
    SDL_Rect segment;
    segment.x = x0 + x * g->pixel_w;
    segment.y = y0 + y * g->pixel_h;
    segment.w = w * g->pixel_w;
    segment.h = h * g->pixel_h;
    SDL_RenderFillRect(g->renderer, &segment);
}

void draw_segment_A(GameState *g, int x0, int y0) {
    draw_segment(g, x0, y0, 0, 0, 4, 1);
}

void draw_segment_B(GameState *g, int x0, int y0) {
    draw_segment(g, x0, y0, 3, 0, 1, 3);
}

void draw_segment_C(GameState *g, int x0, int y0) {
    draw_segment(g, x0, y0, 3, 3, 1, 5);
}

void draw_segment_D(GameState *g, int x0, int y0) {
    draw_segment(g, x0, y0, 0, 7, 4, 1);
}

void draw_segment_E(GameState *g, int x0, int y0) {
    draw_segment(g, x0, y0, 0, 3, 1, 5);
}

void draw_segment_F(GameState *g, int x0, int y0) {
    draw_segment(g, x0, y0, 0, 0, 1, 3);
}

void draw_segment_G(GameState *g, int x0, int y0) {
    draw_segment(g, x0, y0, 0, 3, 4, 1);
}

void draw_0(GameState *g, int x0, int y0) {
    draw_segment_A(g, x0, y0);
    draw_segment_B(g, x0, y0);
    draw_segment_C(g, x0, y0);
    draw_segment_D(g, x0, y0);
    draw_segment_E(g, x0, y0);
    draw_segment_F(g, x0, y0);
}

void draw_1(GameState *g, int x0, int y0) {
    draw_segment_B(g, x0, y0);
    draw_segment_C(g, x0, y0);
}

void draw_2(GameState *g, int x0, int y0) {
    draw_segment_A(g, x0, y0);
    draw_segment_B(g, x0, y0);
    draw_segment_D(g, x0, y0);
    draw_segment_E(g, x0, y0);
    draw_segment_G(g, x0, y0);
}

void draw_3(GameState *g, int x0, int y0) {
    draw_segment_A(g, x0, y0);
    draw_segment_B(g, x0, y0);
    draw_segment_C(g, x0, y0);
    draw_segment_D(g, x0, y0);
    draw_segment_G(g, x0, y0);
}

void draw_4(GameState *g, int x0, int y0) {
    draw_segment_B(g, x0, y0);
    draw_segment_C(g, x0, y0);
    draw_segment_F(g, x0, y0);
    draw_segment_G(g, x0, y0);
}

void draw_5(GameState *g, int x0, int y0) {
    draw_segment_A(g, x0, y0);
    draw_segment_C(g, x0, y0);
    draw_segment_D(g, x0, y0);
    draw_segment_F(g, x0, y0);
    draw_segment_G(g, x0, y0);
}

void draw_6(GameState *g, int x0, int y0) {
    draw_segment_C(g, x0, y0);
    draw_segment_D(g, x0, y0);
    draw_segment_E(g, x0, y0);
    draw_segment_F(g, x0, y0);
    draw_segment_G(g, x0, y0);
}

void draw_7(GameState *g, int x0, int y0) {
    draw_segment_A(g, x0, y0);
    draw_segment_B(g, x0, y0);
    draw_segment_C(g, x0, y0);
}

void draw_8(GameState *g, int x0, int y0) {
    draw_segment_A(g, x0, y0);
    draw_segment_B(g, x0, y0);
    draw_segment_C(g, x0, y0);
    draw_segment_D(g, x0, y0);
    draw_segment_E(g, x0, y0);
    draw_segment_F(g, x0, y0);
    draw_segment_G(g, x0, y0);
}

void draw_9(GameState *g, int x0, int y0) {
    draw_segment_A(g, x0, y0);
    draw_segment_B(g, x0, y0);
    draw_segment_C(g, x0, y0);
    draw_segment_F(g, x0, y0);
    draw_segment_G(g, x0, y0);
}

void draw_digit(GameState *g, int digits, int x0, int y0) {
    switch (digits) {
    case 0:
        draw_0(g, x0, y0);
        break;
    case 1:
        draw_1(g, x0, y0);
        break;
    case 2:
        draw_2(g, x0, y0);
        break;
    case 3:
        draw_3(g, x0, y0);
        break;
    case 4:
        draw_4(g, x0, y0);
        break;
    case 5:
        draw_5(g, x0, y0);
        break;
    case 6:
        draw_6(g, x0, y0);
        break;
    case 7:
        draw_7(g, x0, y0);
        break;
    case 8:
        draw_8(g, x0, y0);
        break;
    case 9:
        draw_9(g, x0, y0);
        break;
    }
}
