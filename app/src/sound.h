#ifndef SOUND_H_FILE
#define SOUND_H_FILE

#include "typedefs.h"
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include <SDL2/SDL.h>
#include <SDL2/SDL_mixer.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

int init_sound();
int load_sound(Context *ctx);
int play_sound(Context *ctx, Mix_Chunk *sound);

int play_bounce(Context *ctx);
int play_score(Context *ctx);
int play_pad(Context *ctx);

#endif
