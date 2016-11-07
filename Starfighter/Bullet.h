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
        damageInflicted = 1;
    }

    Bullet(Point pos, Point vel, AssetCache* ac, SDL_Rect &gb, char const* filename) : GameObject(pos, ac, gb) {
        this->vel = vel;
        texture = assetCache->getTexture(filename);
        collisionBox = { (int)pos.x, (int)pos.y, texture->getWidth(), texture->getHeight() };
        damageInflicted = 1;
    }

    void update() {
        pos.x += vel.x;
        pos.y += vel.y;
        collisionBox.x = pos.x;
        collisionBox.y = pos.y;

        if (pos.y < 0 - texture->getHeight() ||
            pos.y > gameBounds.h ||
            pos.x < 0 - texture->getWidth() ||
            pos.x > gameBounds.w)
            killed = true;
    }
};

#endif /* Bullet_h */
