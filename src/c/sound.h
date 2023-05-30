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
int load_sound(GameState *g);

int play_bounce(GameState *g);
int play_score(GameState *g);
int play_pad(GameState *g);

#endif
