#include "sound.h"

int init_sound() {
    // Init sound
    int status = Mix_Init(0);
    if (status < 0) {
        printf("Failed at Mix_Init()");
        exit(status);
    }
    return Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 1, 1024);
}

int load_sound(GameState *g) {
    g->snd_bounce = Mix_LoadWAV("bounce.wav");
    g->snd_pad = Mix_LoadWAV("pad.wav");
    g->snd_score = Mix_LoadWAV("score.wav");
    return 0;
}

int play_sound(Mix_Chunk *sound) {
    if (Mix_PlayChannel(-1, sound, 0) != -1)
        return 0;
    return 1;
}

int play_bounce(GameState *g) {
    return play_sound(g->snd_bounce);
}

int play_score(GameState *g) {
    return play_sound(g->snd_score);
}


int play_pad(GameState *g) {
    return play_sound(g->snd_pad);
}
