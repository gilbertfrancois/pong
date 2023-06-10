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

int load_sound(Context *ctx) {
    Mix_LoadWAV_RW(SDL_RWFromFile("assets/bounce.ogg", "rb"), 1);
    ctx->snd_bounce = Mix_LoadWAV_RW(SDL_RWFromFile("bounce.wav", "rb"), 1);
    ctx->snd_pad = Mix_LoadWAV_RW(SDL_RWFromFile("pad.wav", "rb"), 1);
    ctx->snd_score = Mix_LoadWAV_RW(SDL_RWFromFile("score.wav", "rb"), 1);
    return 0;
}

int play_sound(Context *ctx, Mix_Chunk *sound) {
    if (!ctx->sound_activated)
        return 1;
    if (Mix_PlayChannel(-1, sound, 0) != -1)
        return 0;
    return 1;
}

int play_bounce(Context *ctx) {
    return play_sound(ctx, ctx->snd_bounce);
}

int play_score(Context *ctx) {
    return play_sound(ctx, ctx->snd_score);
}


int play_pad(Context *ctx) {
    return play_sound(ctx, ctx->snd_pad);
}

void free_sound(Context *ctx) {
    Mix_FreeChunk(ctx->snd_bounce);
    Mix_FreeChunk(ctx->snd_pad);
    Mix_FreeChunk(ctx->snd_score);
    ctx->snd_bounce = NULL;
    ctx->snd_pad = NULL;
    ctx->snd_score = NULL;
    Mix_CloseAudio();
    Mix_Quit();
}   
