//
//  GameObjectFactory.h
//  Starfighter
//
//  Created by Patrick Montag on 11/8/16.
//  Copyright © 2016 Patrick Montag. All rights reserved.
//

#ifndef GameObjectFactory_h
#define GameObjectFactory_h

#include "GameObject.h"

class Starfighter;
class AssetCache;

class GameObjectFactory {
public:
    static game_obj_ptr create(string type, Point p = {}, Point v = {});
    static SDL_Rect gameBounds;
    static AssetCache* assetCache;
    static Starfighter* starfighter;
    static list<game_obj_ptr>* sprites;
    static list<game_obj_ptr>* hostiles;
    static list<game_obj_ptr>* projectiles;
    static list<game_obj_ptr>* destructibles;
};

#endif /* GameObjectFactory_h */
