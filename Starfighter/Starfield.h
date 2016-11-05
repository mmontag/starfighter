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
    Starfield(SDL_Renderer* r, SDL_Rect gb) {
        texture = SDL_CreateTexture(r, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, gb.w, gb.h);
        gameBounds = gb;
        renderer = r;
        for (int i = 0; i < count; i++) {
            stars.push_back(new Star(rand() % gameBounds.w,
                                     rand() % gameBounds.h,
                                     0,
                                     RANDF(0.5, 1.5)));
        }
    }

    void render() {
//        SDL_SetRenderTarget(renderer, texture);
//        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 100);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

        // -- This does a motion-trail effect:
        // static const SDL_Rect rect = { 0, 0, width, height };
        // SDL_RenderFillRect(renderer, &rect);
        // -- This clears the texture completely:
//        SDL_RenderClear(renderer);

        for (auto star : stars) {
            float speed = star->vel.y;
            SDL_Color* col = &star->col;
            SDL_SetRenderDrawColor(renderer, col->r, col->g, col->b, col->a);
            star->pos.y += speed;
            if (star->pos.y > gameBounds.h) star->pos.y -= gameBounds.h;
            SDL_RenderDrawLine(renderer, star->pos.x, star->pos.y, star->pos.x, star->pos.y - speed);
        }
//        SDL_SetRenderTarget(renderer, NULL);
//        SDL_RenderCopy(renderer, texture, NULL, NULL);
    }

private:
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    SDL_Rect gameBounds;
    const int count = 200;
    vector<Star*> stars;
};

#endif /* Starfield_h */
