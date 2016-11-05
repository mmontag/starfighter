//
//  Bullet.h
//  Starfighter
//
//  Created by Patrick Montag on 10/21/16.
//  Copyright © 2016 Patrick Montag. All rights reserved.
//

#ifndef Bullet_h
#define Bullet_h

#include "GameObject.h"

class Bullet : public GameObject
{
public:
    const static int BULLET_VEL = -8;

    Bullet(Point pos, AssetCache* ac, SDL_Rect &gb) : GameObject(pos, ac, gb) {
        texture = assetCache->getTexture("laser.png");
        vel = Point(0, BULLET_VEL);
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

#endif /* Bullet_h */
