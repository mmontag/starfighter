//
//  Missile.h
//  Starfighter
//
//  Created by Patrick Montag on 11/2/16.
//  Copyright © 2016 Patrick Montag. All rights reserved.
//

#ifndef Missile_h
#define Missile_h

#include "GameObject.h"

class Missile : public GameObject
{
public:
    const static int MISSILE_VEL = 2;
    int frameCounter = 0;
    
    Missile(Point pos, AssetCache* ac, SDL_Rect &gb) : GameObject(pos, ac, gb) {
        texture = assetCache->getTexture("missile.png");
        vel = Point(0, MISSILE_VEL);
        collisionBox = { (int)pos.x, (int)pos.y, texture->getWidth(), texture->getHeight() };
        frameCounter = rand() % 32; // Start with random phase
        damageInflicted = 5;
    }

    void update() {
        pos.x += vel.x;
        pos.x += sin(frameCounter / 32.0 * 2 * M_PI);
        pos.y += vel.y;
        vel.y = vel.y - 0.2; // some acceleration
        
        collisionBox.x = pos.x;
        collisionBox.y = pos.y;

        if (pos.y < 0 - texture->getHeight())
            killed = true;
        
        frameCounter++;
    }
};

#endif /* Missile_h */
