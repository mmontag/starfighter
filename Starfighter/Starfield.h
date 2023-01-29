//
//  Starfield.h
//  Starfighter
//
//  Created by Matt Montag on 10/30/16.
//  Copyright © 2016 Patrick Montag. All rights reserved.
//

#ifndef Starfield_h
#define Starfield_h

#include "Point.h"

static const float MAX_STAR_SPEED = 0.5;
static const float MIN_STAR_SPEED = 0.1;

inline float RANDF() {
    return (float) rand() / RAND_MAX;
}

inline float RANDF(float m) {
    return RANDF() * m;
}

inline float RANDF(float min, float max) {
    return RANDF(max - min) + min;
}

struct Star {
    Point pos;
    Point vel;
    SDL_Color col;
    Star(float x, float y, float vx, float vy) {
        pos = Point(x, y);
        vel = Point(vx, vy);
        col.a = RANDF(255);
        col.r = 255 - RANDF(50);
        col.g = 255 - RANDF(100);
        col.b = 255 - RANDF(100);
    };
};

class Starfield {
public:
    Starfield() {}
    Starfield(RENDERER* r, SDL_Rect gb) {
#ifdef USE_GPU
        texture = GPU_CreateImage(gb.w, gb.h, GPU_FORMAT_RGB);
#else
        texture = SDL_CreateTexture(r, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, gb.w, gb.h);
#endif
        gameBounds = gb;
        renderer = r;
        for (int i = 0; i < count; i++) {
            stars.push_back(new Star(rand() % gameBounds.w,
                                     rand() % gameBounds.h,
                                     0,
                                     RANDF(MIN_STAR_SPEED, MAX_STAR_SPEED)));
        }
    }

    void render() {
//      return; // disable for Emscripten testing
//        SDL_SetRenderTarget(renderer, texture);
//        SDL_RenderClear(renderer);
//        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 100);
#ifdef USE_GPU
        GPU_SetShapeBlendMode(GPU_BLEND_NORMAL);
#else
//        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
#endif
        // -- This does a motion-trail effect:
        // static const SDL_Rect rect = { 0, 0, width, height };
        // SDL_RenderFillRect(renderer, &rect);
        // -- This clears the texture completely:
//        SDL_RenderClear(renderer);

        for (auto star : stars) {
            float speed = star->vel.y;
            SDL_Color* col = &star->col;
            star->pos.y += speed;
            if (star->pos.y > gameBounds.h) star->pos.y -= gameBounds.h;
#ifdef USE_GPU
            GPU_Line(renderer, star->pos.x, star->pos.y, star->pos.x, star->pos.y - fmax(1.0, speed * 20), *col);
#else
            SDL_SetRenderDrawColor(renderer, col->r, col->g, col->b, col->a);
            SDL_RenderDrawLine(renderer, star->pos.x, star->pos.y, star->pos.x, star->pos.y - speed);
#endif
        }
//        SDL_SetRenderTarget(renderer, NULL);
//        SDL_RenderCopy(renderer, texture, NULL, NULL);
    }

private:
    RENDERER* renderer;
    TEXTURE* texture;
    SDL_Rect gameBounds;
    int count = 80;
    vector<Star*> stars;
};

#endif /* Starfield_h */
