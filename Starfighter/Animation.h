//
//  Animation.h
//  Starfighter
//
//  Created by Matt Montag on 10/30/16.
//  Copyright © 2016 Patrick Montag. All rights reserved.
//

#ifndef Animation_h
#define Animation_h

#include "AssetCache.h"
#include "Texture.h"
#include <string>

using namespace std;

static const int DEFAULT_FRAME_DIVIDER = 4;

class Animation : public GameObject {
public:
    // Animates a horizontal sprite sheet, tiled in increments of widthStep
    Animation(Point pos, int widthStep, string filename, AssetCache* ac, bool os = true, int fd = DEFAULT_FRAME_DIVIDER) : GameObject(pos, ac) {
        texture = assetCache->getTexture(filename);
        clip.x = 0;
        clip.y = 0;
        clip.h = texture->getHeight();
        clip.w = widthStep;
        frameDivider = fd;
        oneShot = os;
    }
    
    void reset() {
        frameCounter = 0;
        killed = false;
    }

    void render(Point pos) {
        this->pos = pos;
        render();
    }

    void render() {
        if (killed) return;

        clip.x = floor(frameCounter / frameDivider) * clip.w;
        if (clip.x >= texture->getWidth()) {
            clip.x = 0;
            frameCounter = 0;
            if (oneShot) {
                killed = true;
                return;
            }
        }
        Point center = Point(pos.x - clip.w / 2, pos.y - clip.h / 2);
        texture->render(center.x, center.y, &clip);
        frameCounter++;
    }

private:
    SDL_Rect clip;
    bool oneShot = true;
    int frameCounter = 0;
    float frameDivider = DEFAULT_FRAME_DIVIDER;
};

#endif /* Animation_h */
