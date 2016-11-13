//
//  GameObjectFactory.cpp
//  Starfighter
//
//  Created by Matt Montag on 11/10/16.
//  Copyright © 2016 Patrick Montag. All rights reserved.
//

#include "GameObjectFactory.h"
#include "Animation.h"
#include "Bullet.h"
#include "Enemy.h"
#include "Player.h"


SDL_Rect GameObjectFactory::gameBounds;
Starfighter* GameObjectFactory::starfighter;
AssetCache* GameObjectFactory::assetCache = 0;
list<game_obj_ptr>* GameObjectFactory::sprites;
list<game_obj_ptr>* GameObjectFactory::hostiles;
list<game_obj_ptr>* GameObjectFactory::projectiles;
list<game_obj_ptr>* GameObjectFactory::destructibles;

game_obj_ptr GameObjectFactory::create(string type, Point p, Point v) {
    if (type == "explosion") {
        printf("GameObjectFactory create explosion!\n");
        auto obj =  game_obj_ptr(new Animation(p, 32, "enemy_explosion.png", assetCache, true, 10));
        hostiles->push_back(obj);
        return obj;
    }
    if (type == "enemy-bullet") {
        auto obj = game_obj_ptr(new Bullet(p, v, assetCache, gameBounds, "bullet_orange.png"));
        hostiles->push_back(obj);
        return obj;
    }
    if (type == "player-bullet") {
        auto obj = game_obj_ptr(new Bullet(p, Point(0, -8), assetCache, gameBounds, "laser.png"));
        projectiles->push_back(obj);
        return obj;
    }
    if (type == "enemy") {
        auto obj = game_obj_ptr(new Enemy(assetCache, gameBounds, starfighter));
        destructibles->push_back(obj);
        return obj;
    }

    printf("WARNING: tried to create unregistered object '%s'\n", type.c_str());
    return NULL;
}
