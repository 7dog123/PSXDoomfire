#include <SDL.h>
#include <SDL.h>
#include <SDL_render.h>
#include <SDL_video.h>
#include <SDL_events.h>
#include <stdlib.h>
#include <algorithm>
#include <cmath>

#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240
#define FIRE_HEIGHT 168
#define FIRE_VELOCITY 0.5f
#define SPREAD_FIRE 3
#define PALETTE_COLORS 37
#define FRAME_TO_START_FIRE 0
#define FRAME_TO_KEEP_BURN 500
#define FRAME_TO_START_STOP_FIRE 300
#define FRAME_TO_FINISH_STOP_FIRE 400

struct Color {
    Uint8 r, g, b, a;
};

void setInitialFrame(float &frame);
void populatePalette(Color palette[]);
void populatePoints(int points[]);
void handleFrame(float &frame, int points[], const Color palette[], SDL_Renderer *renderer, SDL_Texture *texture, Uint32 *pixels);
void startFire(int points[]);
void doFire(int points[]);
void spreadFire(int src, int points[]);
void stopFire(int points[]);
void updateTexture(SDL_Texture *texture, Uint32 *pixels, const int points[], const Color palette[]);

int main(int argc, char *argv[]) {
    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        printf("SDL Initialization failed: %s\n", SDL_GetError());
        return EXIT_FAILURE;
    }

    SDL_Window *window = SDL_CreateWindow("Doom Fire PS2", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    if (!window) {
        printf("Window creation failed: %s\n", SDL_GetError());
        SDL_Quit();
        return EXIT_FAILURE;
    }

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE); // Use software rendering for PS2
    if (!renderer) {
        printf("Renderer creation failed: %s\n", SDL_GetError());
        SDL_DestroyWindow(window);
        SDL_Quit();
        return EXIT_FAILURE;
    }

    SDL_Texture *texture = SDL_CreateTexture(renderer, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_STREAMING, SCREEN_WIDTH, FIRE_HEIGHT);
    if (!texture) {
        printf("Texture creation failed: %s\n", SDL_GetError());
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return EXIT_FAILURE;
    }

    float frame = -FIRE_VELOCITY;
    Color palette[PALETTE_COLORS];
    int *points = new int[SCREEN_WIDTH * FIRE_HEIGHT]();
    Uint32 *pixels = new Uint32[SCREEN_WIDTH * FIRE_HEIGHT];

    populatePalette(palette);
    populatePoints(points);

    bool running = true;
    SDL_Event event;

    while (running) {
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_QUIT) {
                running = false;
            }
        }

        handleFrame(frame, points, palette, renderer, texture, pixels);

        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, nullptr, nullptr);
        SDL_RenderPresent(renderer);
    }

    delete[] points;
    delete[] pixels;
    SDL_DestroyTexture(texture);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return EXIT_SUCCESS;
}

void stopFire(int points[]) {
    for (int y = FIRE_HEIGHT - 1; y > 160; y--) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            int idx = y * SCREEN_WIDTH + x;
            if (points[idx] > 0) {
                points[idx] -= rand() % (SPREAD_FIRE + 1);
            }
        }
    }
}

void spreadFire(int src, int points[]) {
    int point = points[src];
    if (point == 0) {
        points[src - SCREEN_WIDTH] = 0;
    } else {
        int randIdx = rand() % (SPREAD_FIRE + 1);
        int dst = src - randIdx + 1;
        points[dst - SCREEN_WIDTH] = point - (randIdx & 1);
    }
}

void doFire(int points[]) {
    for (int y = 1; y < FIRE_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            spreadFire(y * SCREEN_WIDTH + x, points);
        }
    }
}

void startFire(int points[]) {
    for (int x = 0; x < SCREEN_WIDTH; x++) {
        points[(FIRE_HEIGHT - 1) * SCREEN_WIDTH + x] = PALETTE_COLORS - 1;
    }
}

void handleFrame(float &frame, int points[], const Color palette[], SDL_Renderer *renderer, SDL_Texture *texture, Uint32 *pixels) {
    int exactFrame = static_cast<int>(std::floor(frame));

    if (exactFrame == frame) {
        if (exactFrame == FRAME_TO_START_FIRE) {
            startFire(points);
        } else if (exactFrame < FRAME_TO_KEEP_BURN) {
            if (exactFrame >= FRAME_TO_START_STOP_FIRE && exactFrame <= FRAME_TO_FINISH_STOP_FIRE) {
                stopFire(points);
            }
            doFire(points);
        } else {
            setInitialFrame(frame);
        }
    }

    frame += FIRE_VELOCITY;
    updateTexture(texture, pixels, points, palette);
}

void updateTexture(SDL_Texture *texture, Uint32 *pixels, const int points[], const Color palette[]) {
    for (int y = 0; y < FIRE_HEIGHT; y++) {
        for (int x = 0; x < SCREEN_WIDTH; x++) {
            int idx = y * SCREEN_WIDTH + x;
            Color color = palette[points[idx]];
            pixels[idx] = SDL_MapRGBA(SDL_AllocFormat(SDL_PIXELFORMAT_RGBA8888), color.r, color.g, color.b, color.a);
        }
    }
    SDL_UpdateTexture(texture, nullptr, pixels, SCREEN_WIDTH * sizeof(Uint32));
}

void setInitialFrame(float &frame) {
    frame = -FIRE_VELOCITY;
}

void populatePalette(Color palette[]) {
    const Uint8 rgbas[PALETTE_COLORS][4] = {
        {7, 7, 7, 0}, {31, 7, 7, 255}, {47, 15, 7, 255}, {71, 15, 7, 255},
        {87, 23, 7, 255}, {103, 31, 7, 255}, {119, 31, 7, 255}, {143, 39, 7, 255},
        {159, 47, 7, 255}, {175, 63, 7, 255}, {191, 71, 7, 255}, {199, 71, 7, 255},
        {223, 79, 7, 255}, {223, 87, 7, 255}, {223, 87, 7, 255}, {215, 95, 7, 255},
        {215, 95, 7, 255}, {215, 103, 15, 255}, {207, 111, 15, 255}, {207, 119, 15, 255},
        {207, 127, 15, 255}, {207, 135, 23, 255}, {199, 135, 23, 255}, {199, 143, 23, 255},
        {199, 151, 31, 255}, {191, 159, 31, 255}, {191, 159, 31, 255}, {191, 167, 39, 255},
        {191, 167, 39, 255}, {191, 175, 47, 255}, {183, 175, 47, 255}, {183, 183, 47, 255},
        {183, 183, 55, 255}, {207, 207, 111, 255}, {223, 223, 159, 255}, {239, 239, 199, 255},
        {255, 255, 255, 255}
    };

    for (int i = 0; i < PALETTE_COLORS; ++i) {
        palette[i] = {rgbas[i][0], rgbas[i][1], rgbas[i][2], rgbas[i][3]};
    }
}

void populatePoints(int points[]) {
    std::fill(points, points + SCREEN_WIDTH * FIRE_HEIGHT, 0);
}
