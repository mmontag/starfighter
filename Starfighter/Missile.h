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
    const static int MISSILE_VEL = -2;

    Missile(Point pos, AssetCache* ac, SDL_Rect &gb) : GameObject(pos, ac, gb) {
        texture = assetCache->getTexture("missile.png");
        vel = Point(0, MISSILE_VEL);
        collisionBox = { (int)pos.x, (int)pos.y, texture->getWidth(), texture->getHeight() };
    }

    void update() {
        pos.x += vel.x;
        pos.y += vel.y;
        collisionBox.x = pos.x;
        collisionBox.y = pos.y;

        if (pos.y < 0 - texture->getHeight())
            killed = true;
    }
};

#endif /* Missile_h */
