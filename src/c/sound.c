#include "sound.h"

int init_sound() {
    // Init sound
    int status = Mix_Init(0);
    if (status < 0) {
        printf("Failed at Mix_Init()");
        exit(status);
    }
    Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 1, 1024);
    return 0;
}

int load_sound(GameState *g) {
    g->snd_bounce = Mix_LoadWAV("bounce.wav");
    g->snd_pad = Mix_LoadWAV("pad.wav");
    g->snd_score = Mix_LoadWAV("score.wav");
    return 0;
}
