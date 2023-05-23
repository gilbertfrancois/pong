#ifndef SOUND_H_FILE
#define SOUND_H_FILE

#include "typedefs.h"
#ifdef __EMSCRIPTEN__
#include <emscripten.h>
#endif

#include <SDL2/SDL.h>
#ifdef __SOUND__
#include <SDL2/SDL_mixer.h>
#endif
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

int init_sound();
int load_sound(GameState *g);

int play_bounce();
int play_score();
int play_pad();

#endif
