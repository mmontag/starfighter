//
//  Asteroid.h
//  Starfighter
//
//  Created by Patrick Montag on 10/23/16.
//  Copyright © 2016 Patrick Montag. All rights reserved.
//

#ifndef Asteroid_h
#define Asteroid_h

#include "GameObject.h"

// asteroid class
class Asteroid : public GameObject
{
public:
    Asteroid(AssetCache* ac, const SDL_Rect& gb);
    
    void update();
    void render();

    void damage();

    float scale = 1.0;

private:
    int health;

    static const int ASTEROID_WIDTH = 16;
    static const int ASTEROID_HEIGHT = 16;
    static const int ASTEROID_VEL = 2;
    
    SDL_Rect asteroidClip =             { ASTEROID_WIDTH * 0, 0, ASTEROID_WIDTH, ASTEROID_HEIGHT };
    SDL_Rect asteroidDamagedClip =      { ASTEROID_WIDTH * 1, 0, ASTEROID_WIDTH, ASTEROID_HEIGHT };
    SDL_Rect asteroidDamagedTwiceClip = { ASTEROID_WIDTH * 2, 0, ASTEROID_WIDTH, ASTEROID_HEIGHT };
};

Asteroid::Asteroid(AssetCache* ac, const SDL_Rect& gb) : GameObject(ac, gb) {
    texture = assetCache->getTexture("asteroid.png");

    pos.x = rand() % gameBounds.w - 16;
    pos.y = -32;
    vel.x = 0;
    vel.y = (rand() % 40)/10.0 + 1;
    health = (rand() % 3) + 1;
    // Giant Asteroids just for fun
    // if (rand() % 100 == 0) {
    //    scale = 10;
    //    health = 30;
    // }
    collisionBox.x = pos.x;
    collisionBox.y = pos.y;
    collisionBox.w = ASTEROID_WIDTH;
    collisionBox.h = ASTEROID_HEIGHT;
}

void Asteroid::damage() {
    health--;
    if (health <= 0) killed = true;
}

void Asteroid::update() {
    pos.x += vel.x;
    pos.y += vel.y;
    
    collisionBox.x = pos.x;
    collisionBox.y = pos.y;
    collisionBox.w = ASTEROID_WIDTH * scale;
    collisionBox.h = ASTEROID_HEIGHT * scale;

    if (pos.y > gameBounds.h + asteroidClip.h)
        killed = true;
    
    if (health <= 0)
        killed = true;
}

void Asteroid::render() {
    if (health / scale >= 3)
        texture->render(pos.x, pos.y, &asteroidClip, scale);
    else if (health / scale >= 2)
        texture->render(pos.x, pos.y, &asteroidDamagedClip, scale);
    else
        texture->render(pos.x, pos.y, &asteroidDamagedTwiceClip, scale);
}

#endif /* Asteroid_h */
