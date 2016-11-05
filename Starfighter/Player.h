//
//  Starfighter.h
//  Starfighter
//
//  Created by Patrick Montag on 10/21/16.
//  Copyright © 2016 Patrick Montag. All rights reserved.
//

#ifndef Starfighter_h
#define Starfighter_h

#include <vector>
#include "Texture.h"
#include "Bullet.h"
#include "Missile.h"
#include "Timer.h"
#include "Animation.h"

using namespace std;

// starfighter class
class Starfighter
{
public:
    // dimensions of the hero for collision purposes
    static const int STARFIGHTER_WIDTH = 16;
    static const int STARFIGHTER_HEIGHT = 16;
    static const int FIRE_RATE = 10; // bullets per second, fire rate 2 bps 500
    
    // maximum axis velocity of the hero
    static const int STARFIGHTER_VEL = 2;
    
    // initializes the variables
    Starfighter(AssetCache* ac, list<game_obj_ptr> &projectiles, SDL_Rect gameBounds);
    
    // takes key presses and adjusts the hero's velocity
    void handleEvent( SDL_Event& e );
    
    //Moves the Starfighter
    void update();
    
    // shows the hero on the screen
    void render();

    void damage() {
        shield->reset();
        health--;

        if (health <= 0) {
            SDL_Event event;
            event.type = deathEvent;
            SDL_PushEvent(&event);
            health = 5;
        }
    }

    // x and y offsets of the Starfighter
    float posX, posY;
    
    // velocity of the Starfighter
    float velX, velY;
    
    // fire laser weapon function
    void fireWeapon();
    void fireMissile();
    
    // is firing? is spacebar held down?
    bool isFiringLaser;
    bool canFire;
    bool isCoolingDown;
    
    float laserEnergy;
    float maxLaserEnergy;
    float laserCooldownRate;
    
    // health of Starfighter
    float health;
    
    // collision box
    SDL_Rect collisionBox;

    static Uint32 deathEvent;

    int lastBulletTick = 0;
    uint bulletCounter;
    
private:
    AssetCache* assetCache;
    list<game_obj_ptr>* projectiles;
    
    // Starfighter frames, banking left or right, boosting
    SDL_Rect starfighterClip;
    SDL_Rect starfighterLeftClip;
    SDL_Rect starfighterRightClip;
    
    SDL_Rect gameBounds;
    
    Animation* shield;
    
    int frameCounter = 0;

    // the acceleration of the Starfighter falling, or flying.
    float accX = 0;
    float accY = 0;
};

Uint32 Starfighter::deathEvent = SDL_RegisterEvents(1);

Starfighter::Starfighter(AssetCache* ac, list<game_obj_ptr>& projectiles, SDL_Rect gb) : projectiles(&projectiles) {
    
    assetCache = ac;
    gameBounds = gb;
    
    collisionBox = { 0, 0, 16, 16 };
    starfighterClip = { 0, 0, 16, 16 };
    starfighterLeftClip = { 16, 0, 16, 16 };
    starfighterRightClip = { 32, 0, 16, 16 };
    
    // TODO: convert to Point
    posX = gameBounds.w / 2 - starfighterClip.w / 2;
    posY = gameBounds.h / 2 - starfighterClip.h / 2;
    
    // jets = new Animation(posX, posY, 8, "jets.png", assetCache, false, 16);
    shield = new Animation(Point(posX + 8, posY + 8), 24, "shield_animation.png", assetCache);
    
    velX = 0;
    velY = 0;

    canFire = true;
    isCoolingDown = false;
    isFiringLaser = false;
    
    laserEnergy = 0;
    maxLaserEnergy = 5;
    laserCooldownRate = .1; // fractions of energy per frame
    health = 5;
}

void Starfighter::handleEvent(SDL_Event& e) {
    // if a key was pressed
    if(e.type == SDL_KEYDOWN && e.key.repeat == 0) {
        // adjust the velocity
        switch(e.key.keysym.sym) {
            case SDLK_UP: velY -= STARFIGHTER_VEL;
                break;
            case SDLK_DOWN: velY += STARFIGHTER_VEL;
                break;
            case SDLK_LEFT: velX -= STARFIGHTER_VEL;
                break;
            case SDLK_RIGHT: velX += STARFIGHTER_VEL;
                break;
            case SDLK_SPACE:
                isFiringLaser = true;
                if (isCoolingDown) {
                    Mix_PlayChannel(-1, assetCache->getSound("overload.wav"), 0);
                }
                break;
            case SDLK_a: fireMissile();
                printf ("Missile fired");
                break;
        }
    }

    // if a key was released
    else if(e.type == SDL_KEYUP && e.key.repeat == 0) {
        // adjust the velocity
        switch(e.key.keysym.sym) {
            case SDLK_UP: velY += STARFIGHTER_VEL;
                break;
            case SDLK_DOWN: velY -= STARFIGHTER_VEL;
                break;
            case SDLK_LEFT: velX += STARFIGHTER_VEL;
                break;
            case SDLK_RIGHT: velX -= STARFIGHTER_VEL;
                break;
            case SDLK_SPACE:
                isFiringLaser = false;
                break;
            case SDLK_a:
                break;
        }
    }
    
    // handle controller input
    if(e.type == SDL_CONTROLLERBUTTONDOWN) {
        switch(e.cbutton.button) {
            case SDL_CONTROLLER_BUTTON_DPAD_UP: velY -= STARFIGHTER_VEL;
                break;
            case SDL_CONTROLLER_BUTTON_DPAD_DOWN: velY += STARFIGHTER_VEL;
                break;
            case SDL_CONTROLLER_BUTTON_DPAD_LEFT: velX -= STARFIGHTER_VEL;
                break;
            case SDL_CONTROLLER_BUTTON_DPAD_RIGHT: velX += STARFIGHTER_VEL;
                break;
            case SDL_CONTROLLER_BUTTON_A:
                isFiringLaser = true;
                if (isCoolingDown) {
                    Mix_PlayChannel(-1, assetCache->getSound("overload.wav"), 0);
                }
                break;
        }
    }
    // button release
    else if (e.type == SDL_CONTROLLERBUTTONUP) {
        switch(e.cbutton.button) {
            case SDL_CONTROLLER_BUTTON_DPAD_UP: velY += STARFIGHTER_VEL;
                break;
            case SDL_CONTROLLER_BUTTON_DPAD_DOWN: velY -= STARFIGHTER_VEL;
                break;
            case SDL_CONTROLLER_BUTTON_DPAD_LEFT: velX += STARFIGHTER_VEL;
                break;
            case SDL_CONTROLLER_BUTTON_DPAD_RIGHT: velX -= STARFIGHTER_VEL;
                break;
            case SDL_CONTROLLER_BUTTON_A:
                isFiringLaser = false;
                break;
        }
    }
}

void Starfighter::update() {
    // change position on screen according to velocity and acceleration
    posX += velX;
    posY += velY;
    
    velX += accX;
    velY += accY;
    
    // If the hero went too far to the left or right
    if((posX < 0) || (posX + STARFIGHTER_WIDTH > gameBounds.w)) {
        //Move back
        posX -= velX;
    }
    
    // if the hero went too far up or down
    if((posY < 0) || (posY + STARFIGHTER_HEIGHT > gameBounds.h)) {
        //Move back
        posY -= velY;
    }
    
    // change collider box too !
    collisionBox.x = posX;
    collisionBox.y = posY;
    
    if(isCoolingDown == false) {
        if(isFiringLaser && SDL_GetTicks() > lastBulletTick + (1000 / (FIRE_RATE)))
            fireWeapon();

        laserEnergy = laserEnergy - laserCooldownRate;
    
        if(laserEnergy < 0)
            laserEnergy = 0;
    
        if (laserEnergy >= maxLaserEnergy) {
            laserEnergy = maxLaserEnergy;
            Mix_PlayChannel(-1, assetCache->getSound("overload.wav"), 0);
            isCoolingDown = true;
        }
    }
    else if (isCoolingDown == true){
        laserEnergy = laserEnergy - (laserCooldownRate * .25);
        
        if(laserEnergy < 0) {
            laserEnergy = 0;
            isCoolingDown = false;
        }
    }
}

void Starfighter::render() {
    
    Texture* texture = assetCache->getTexture("starfighter.png");
    
    if(velX > 0) {
        texture->render(posX, posY, &starfighterRightClip);
    } else if(velX < 0) {
        texture->render(posX, posY, &starfighterLeftClip);
    } else {
        texture->render(posX, posY, &starfighterClip);
    }

    shield->render(Point(posX + 8, posY + 8));
}

void Starfighter::fireWeapon() {
    if (canFire == true && isCoolingDown == false) {
        Mix_PlayChannel(-1, assetCache->getSound("fire.wav"), 0);
    
        if (bulletCounter % 2 == 0) {
            projectiles->push_back(game_obj_ptr(new Bullet(Point(posX + 0, posY + 6), assetCache, gameBounds)));
        } else {
            projectiles->push_back(game_obj_ptr(new Bullet(Point(posX + 13, posY + 6), assetCache, gameBounds)));
        }
        laserEnergy++;
    }
    bulletCounter++;
    lastBulletTick = SDL_GetTicks();
}

void Starfighter::fireMissile() {
    projectiles->push_back(game_obj_ptr(new Missile(Point(posX, posY), assetCache, gameBounds)));
}

#endif /* Starfighter_h */
