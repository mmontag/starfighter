//
//  Player.cpp
//  Starfighter
//
//  Created by Matt Montag on 11/8/16.
//  Copyright © 2016 Patrick Montag. All rights reserved.
//

#include <vector>

#include "GameObjectFactory.h"
#include "Texture.h"
#include "Bullet.h"
#include "Missile.h"
#include "Timer.h"
#include "Animation.h"
#include "Action.h"
#include "ExplodeAction.h"
#include "PlayerDeathAction.h"
#include "Player.h"

Uint32 Starfighter::deathEvent = SDL_RegisterEvents(1);

Starfighter::Starfighter(AssetCache* ac, list<game_obj_ptr>* p, SDL_Rect &gb) : GameObject(ac, gb) {
    projectiles = p;

    collisionBox = { 0, 0, 16, 16 };
    starfighterClip = { 0, 0, 16, 16 };
    starfighterLeftClip = { 16, 0, 16, 16 };
    starfighterRightClip = { 32, 0, 16, 16 };
    resetPosition();

    texture = assetCache->getTexture("starfighter.png");
    shield = Animation(Point(pos.x + 8, pos.y + 8), 24, "shield_animation.png", assetCache);

    isCoolingDown = false;
    isFiringLaser = false;

    laserEnergy = 0;
    maxLaserEnergy = 5;
    laserCooldownRate = BASE_COOLDOWN_RATE; // fractions of energy per frame
    health = 15;
    damageInflicted = 1;
}

void Starfighter::damage(float amount) {
    //        if (explodingCountdown > 0) return;

    health -= amount;
    if (health <= 0) {
        health = 0;
        destroy();
    } else {
        shield.reset();
    }
}

void Starfighter::destroy() {
    // TODO: Only return if actionList contains a death action
    if (static_cast<ActionList*>(actionList.get())->actions.size() > 0)
        return;

    static_cast<ActionList*>(actionList.get())->add(new ExplodeAction(this));
    static_cast<ActionList*>(actionList.get())->add(new PlayerDeathAction(this));
}

void Starfighter::resetPosition() {
    pos.x = gameBounds.w / 2 - starfighterClip.w / 2;
    pos.y = gameBounds.h - 20 - starfighterClip.h;
}

Point Starfighter::getCenter() {
    return Point(pos.x + collisionBox.w / 2, pos.y + collisionBox.h / 2);
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
    } else if (isCoolingDown == true){
        laserEnergy = laserEnergy - (laserCooldownRate * .25);

        if(laserEnergy < 0) {
            laserEnergy = 0;
            isCoolingDown = false;
        }
    }

    GameObject::update();
}

void Starfighter::render() {
    if(vel.x > 0) {
        clipRect = starfighterRightClip;
    } else if(vel.x < 0) {
        clipRect = starfighterLeftClip;
    } else {
        clipRect = starfighterClip;
    }

    shield.render(Point(pos.x + 8, pos.y + 8));
    GameObject::render();
}

void Starfighter::fireWeapon() {
    if (isCoolingDown == false) {
        Mix_PlayChannel(-1, assetCache->getSound("fire.wav"), 0);
        
        Point leftGun = Point(pos.x, pos.y + 6);
        Point rightGun = Point(pos.x + 13, pos.y + 6);

        if (bulletCounter % 2 == 0) {
            //GameObjectFactory::sprites->push_back(game_obj_ptr(new Bullet(Point(pos.x + 0, pos.y + 6), assetCache, gameBounds)));
            GameObjectFactory::create("player-bullet", leftGun);
        } else {
            //projectiles->push_back(game_obj_ptr(new Bullet(Point(pos.x + 13, pos.y + 6), assetCache, gameBounds)));
            GameObjectFactory::create("player-bullet", rightGun);
        }
        laserEnergy++;
    }
    bulletCounter++;
    lastBulletTick = SDL_GetTicks();
}

void Starfighter::fireMissile() {
    projectiles->push_back(game_obj_ptr(new Missile(Point(pos.x + 8, pos.y - 4), assetCache, gameBounds)));
}

