//
//  Starfighter.h
//  Starfighter
//
//  Created by Patrick Montag on 10/21/16.
//  Copyright © 2016 Patrick Montag. All rights reserved.
//

#ifndef Starfighter_h
#define Starfighter_h

#include "GameObject.h"
#include "Animation.h"

using namespace std;

static const int STARFIGHTER_WIDTH = 16;
static const int STARFIGHTER_HEIGHT = 16;
static const int FIRE_RATE = 10;
static const int STARFIGHTER_VEL = 2;
static const float BASE_COOLDOWN_RATE = 0.1;
class PlayerDeathAction;

// starfighter class
class Starfighter : public GameObject
{
public:
    Starfighter() : GameObject() {}
    Starfighter(AssetCache* ac, list<game_obj_ptr>* p, SDL_Rect &gb);
    
    void handleEvent( SDL_Event& e );
    void update();
    void render();
    void damage(float amount);
    void destroy();
    void resetPosition();
    Point getCenter();
    void fireWeapon();
    void fireMissile();
    bool isFiringLaser;
    bool isCoolingDown;
    bool isExploding;
    
    float laserEnergy;
    float maxLaserEnergy;
    float laserCooldownRate;
    static Uint32 deathEvent;
    int lastBulletTick = 0;
    uint bulletCounter;

private:
    list<game_obj_ptr>* projectiles;
    
    // Starfighter frames, banking left or right, boosting
    RECT starfighterClip;
    RECT starfighterLeftClip;
    RECT starfighterRightClip;
    
    Animation shield;

    int frameCounter = 0;
};

#endif /* Starfighter_h */
