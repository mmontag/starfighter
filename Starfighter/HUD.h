//
//  HUD.h
//  Starfighter
//
//  Created by Matt Montag on 10/31/16.
//  Copyright © 2016 Patrick Montag. All rights reserved.
//

#ifndef HUD_h
#define HUD_h

#include "AssetCache.h"
#include "Point.h"

// where is this used?
const Point POWER_METER_POS(5, 10);

class HUD {
public:
    HUD() {}
    HUD(Starfighter* sf, AssetCache* ac, SDL_Renderer* r, SDL_Rect gb) {
        starfighter = sf;
        assetCache = ac;
        renderer = r;
        gameBounds = gb;
        fgCol = {255, 255, 255, 255};
        texture = SDL_CreateTexture(r, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, gb.w, gb.h);
        SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
    }

    void render() {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

        // Primary weapon power meter
        static const int meterHeight = 6;
        static const int meterWidth = 50;
        SDL_Rect powerMeter = { 5, 15, meterWidth, meterHeight };
        float power = starfighter->laserEnergy / starfighter->maxLaserEnergy;
        SDL_Color powerCol = { 255, Uint8(int(255 - (200 * power))), Uint8(200 - (190 * power)), 255 };
        SDL_Color col;
        if (starfighter->isCoolingDown)
            col = powerCol;
        else
            col = fgCol;
        SDL_SetRenderDrawColor(renderer, col.r, col.g, col.b, col.a);
        SDL_RenderDrawRect(renderer, &powerMeter);

        SDL_SetRenderDrawColor(renderer, powerCol.r, powerCol.g, powerCol.b, powerCol.a);
        SDL_Rect powerMeterInner = { 7, 17, int(power * (meterWidth - 4)), meterHeight - 4 };
        SDL_RenderFillRect(renderer, &powerMeterInner);

        
        // Shield power meter
        SDL_Rect shieldMeter = { 5, 22, meterWidth, meterHeight };
        float shield = (starfighter->health) / 5;
        SDL_Color shieldCol = { 60, 188, 252, 255 };
        SDL_SetRenderDrawColor(renderer, fgCol.r, fgCol.g, fgCol.b, fgCol.a);
        SDL_RenderDrawRect(renderer, &shieldMeter);
        
        SDL_SetRenderDrawColor(renderer, shieldCol.r, shieldCol.g, shieldCol.b, shieldCol.a);
        SDL_Rect shieldMeterInner = { 7, 24, int(shield * (meterWidth - 4)), meterHeight - 4 };
        SDL_RenderFillRect(renderer, &shieldMeterInner);

        // Score (TODO)
    }
private:
    Starfighter* starfighter;
    AssetCache* assetCache;
    SDL_Rect gameBounds;
    SDL_Renderer* renderer;
    SDL_Texture* texture;
    SDL_Color fgCol;
};

#endif /* HUD_h */
