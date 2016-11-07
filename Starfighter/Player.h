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

static const int STARFIGHTER_WIDTH = 16;
static const int STARFIGHTER_HEIGHT = 16;
static const int FIRE_RATE = 10;
static const int STARFIGHTER_VEL = 2;
static const float BASE_COOLDOWN_RATE = 0.1;
static const int EXPLODING_FRAMES = 100;

// starfighter class
class Starfighter
{
public:
    Starfighter() {}
    Starfighter(AssetCache* ac, list<game_obj_ptr>* projectiles, SDL_Rect gameBounds);
    
    void handleEvent( SDL_Event& e );
    void update();
    void render();

    void damage(float amount) {
        if (explodingCountdown > 0) return;

        health -= amount;
        if (health <= 0) {
            health = 0;
            destroy();
        } else {
            shield->reset();
        }
    }

    void destroy() {
        if (explodingCountdown > 0) return;

        Mix_PlayChannel(-1, assetCache->getSound("big-explosion.wav"), 0);
        explodingCountdown = EXPLODING_FRAMES;
    }

    void resetPosition() {
        pos.x = gameBounds.w / 2 - starfighterClip.w / 2;
        pos.y = gameBounds.h - 20 - starfighterClip.h;
    }

    Point getCenter() {
        return Point(pos.x + collisionBox.w / 2, pos.y + collisionBox.h / 2);
    }

    Point pos;
    Point vel;
    
    void fireWeapon();
    void fireMissile();
    
    bool isFiringLaser;
    bool isCoolingDown;
    bool isExploding;
    
    float laserEnergy;
    float maxLaserEnergy;
    float laserCooldownRate;
    float health;

    SDL_Rect collisionBox;

    static Uint32 deathEvent;

    int explodingCountdown = 0;
    int lastBulletTick = 0;
    uint bulletCounter;
    
private:
    Texture* texture;
    AssetCache* assetCache;
    list<game_obj_ptr>* projectiles;
    
    // Starfighter frames, banking left or right, boosting
    SDL_Rect starfighterClip;
    SDL_Rect starfighterLeftClip;
    SDL_Rect starfighterRightClip;
    
    SDL_Rect gameBounds;
    
    Animation* shield;

    int frameCounter = 0;
};

Uint32 Starfighter::deathEvent = SDL_RegisterEvents(1);

Starfighter::Starfighter(AssetCache* ac, list<game_obj_ptr>* projectiles, SDL_Rect gb) : projectiles(projectiles) {// : GameObject(ac, gb)   {
    
    assetCache = ac;
    gameBounds = gb;
    collisionBox = { 0, 0, 16, 16 };
    starfighterClip = { 0, 0, 16, 16 };
    starfighterLeftClip = { 16, 0, 16, 16 };
    starfighterRightClip = { 32, 0, 16, 16 };
    resetPosition();

    texture = assetCache->getTexture("starfighter.png");
    shield = new Animation(Point(pos.x + 8, pos.y + 8), 24, "shield_animation.png", assetCache);

    isCoolingDown = false;
    isFiringLaser = false;
    
    laserEnergy = 0;
    maxLaserEnergy = 5;
    laserCooldownRate = BASE_COOLDOWN_RATE; // fractions of energy per frame
    health = 5;
    //damageInflicted = 1;
}

void Starfighter::handleEvent(SDL_Event& e) {
    if (e.type == SDL_KEYUP && e.key.repeat == 0) {
        switch(e.key.keysym.sym) {
            case SDLK_UP:    vel.y += STARFIGHTER_VEL; break;
            case SDLK_DOWN:  vel.y -= STARFIGHTER_VEL; break;
            case SDLK_LEFT:  vel.x += STARFIGHTER_VEL; break;
            case SDLK_RIGHT: vel.x -= STARFIGHTER_VEL; break;
            case SDLK_SPACE: isFiringLaser = false; break;
            case SDLK_a:
                break;
        }
    }

    if (e.type == SDL_CONTROLLERBUTTONUP) {
        switch(e.cbutton.button) {
            case SDL_CONTROLLER_BUTTON_DPAD_UP:     vel.y += STARFIGHTER_VEL; break;
            case SDL_CONTROLLER_BUTTON_DPAD_DOWN:   vel.y -= STARFIGHTER_VEL; break;
            case SDL_CONTROLLER_BUTTON_DPAD_LEFT:   vel.x += STARFIGHTER_VEL; break;
            case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:  vel.x -= STARFIGHTER_VEL; break;
            case SDL_CONTROLLER_BUTTON_A:           isFiringLaser = false; break;
        }
    }

    if (e.type == SDL_KEYDOWN && e.key.repeat == 0) {
        switch(e.key.keysym.sym) {
            case SDLK_UP:    vel.y -= STARFIGHTER_VEL; break;
            case SDLK_DOWN:  vel.y += STARFIGHTER_VEL; break;
            case SDLK_LEFT:  vel.x -= STARFIGHTER_VEL; break;
            case SDLK_RIGHT: vel.x += STARFIGHTER_VEL; break;
            case SDLK_SPACE:
                isFiringLaser = true;
                if (isCoolingDown) {
                    Mix_PlayChannel(-1, assetCache->getSound("overload.wav"), 0);
                }
                break;
            case SDLK_a: fireMissile(); break;
        }
    }

    if (e.type == SDL_CONTROLLERBUTTONDOWN) {
        switch (e.cbutton.button) {
            case SDL_CONTROLLER_BUTTON_DPAD_UP:     vel.y -= STARFIGHTER_VEL; break;
            case SDL_CONTROLLER_BUTTON_DPAD_DOWN:   vel.y += STARFIGHTER_VEL; break;
            case SDL_CONTROLLER_BUTTON_DPAD_LEFT:   vel.x -= STARFIGHTER_VEL; break;
            case SDL_CONTROLLER_BUTTON_DPAD_RIGHT:  vel.x += STARFIGHTER_VEL; break;
            case SDL_CONTROLLER_BUTTON_A:
                isFiringLaser = true;
                if (isCoolingDown) {
                    Mix_PlayChannel(-1, assetCache->getSound("overload.wav"), 0);
                }
                break;
        }
    }
}

void Starfighter::update() {
    if (explodingCountdown == 1) {
        SDL_Event event;
        event.type = deathEvent;
        SDL_PushEvent(&event);
        resetPosition();

        health = 5;
    }
    if (explodingCountdown > 0) {
        if (explodingCountdown % 10 == 0) {
            Point p = { pos.x + rand() % collisionBox.w, pos.y + rand() % collisionBox.h };
            projectiles->push_back(game_obj_ptr(new Animation(p, 12, "small_explosion.png", assetCache)));
        }
        explodingCountdown--;
        return;
    }


    // change position on screen according to velocity and acceleration
    pos.x += vel.x;
    pos.y += vel.y;

    // If the hero went too far to the left or right
    if((pos.x < 0) || (pos.x + STARFIGHTER_WIDTH > gameBounds.w)) {
        //Move back
        pos.x -= vel.x;
    }
    
    // if the hero went too far up or down
    if((pos.y < 0) || (pos.y + STARFIGHTER_HEIGHT > gameBounds.h)) {
        //Move back
        pos.y -= vel.y;
    }
    
    // change collider box too !
    collisionBox.x = pos.x;
    collisionBox.y = pos.y;
    
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
    // Flash while exploding
    if ((explodingCountdown / 10) % 2 == 1)
        return;

    if(vel.x > 0) {
        texture->render(pos.x, pos.y, &starfighterRightClip);
    } else if(vel.x < 0) {
        texture->render(pos.x, pos.y, &starfighterLeftClip);
    } else {
        texture->render(pos.x, pos.y, &starfighterClip);
    }

    shield->render(Point(pos.x + 8, pos.y + 8));
}

void Starfighter::fireWeapon() {
    if (isCoolingDown == false) {
        Mix_PlayChannel(-1, assetCache->getSound("fire.wav"), 0);
    
        if (bulletCounter % 2 == 0) {
            projectiles->push_back(game_obj_ptr(new Bullet(Point(pos.x + 0, pos.y + 6), assetCache, gameBounds)));
        } else {
            projectiles->push_back(game_obj_ptr(new Bullet(Point(pos.x + 13, pos.y + 6), assetCache, gameBounds)));
        }
        laserEnergy++;
    }
    bulletCounter++;
    lastBulletTick = SDL_GetTicks();
}

void Starfighter::fireMissile() {
    projectiles->push_back(game_obj_ptr(new Missile(Point(pos.x + 8, pos.y - 4), assetCache, gameBounds)));
}

#endif /* Starfighter_h */
