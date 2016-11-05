//
//  GameObject.h
//  Starfighter
//
//  Created by Patrick Montag on 11/4/16.
//  Copyright © 2016 Patrick Montag. All rights reserved.
//

#ifndef GameObject_h
#define GameObject_h

#include "Point.h"
#include "Texture.h"
#include "AssetCache.h"

class GameObject {
public:
    GameObject(Point pos, AssetCache* ac) : GameObject(pos, ac, SDL_Rect()) {}
    GameObject(AssetCache* ac, const SDL_Rect &gb) : GameObject(Point(), ac, gb) {}
    GameObject(Point pos, AssetCache* ac, const SDL_Rect &gb) : pos(pos), assetCache(ac), gameBounds(gb) {}
    ~GameObject() {
        texture->free();
    }
    
    virtual void update() {};
    virtual void render() {
        texture->render(pos);
    }
    virtual Point getCenter() {
        return Point(pos.x + collisionBox.w / 2, pos.y + collisionBox.h / 2);
    }
    virtual void damage() {};
    virtual void kill() {
        killed = true;
    }
    bool killed = false;
    SDL_Rect collisionBox;
protected:
    AssetCache* assetCache;
    SDL_Rect gameBounds;
    Texture* texture;
    Point pos;
    Point vel;
};

typedef std::shared_ptr<GameObject> game_obj_ptr;

#endif /* GameObject_h */
