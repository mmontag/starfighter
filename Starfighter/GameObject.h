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

class GameObject;
typedef std::shared_ptr<GameObject> game_obj_ptr;
typedef void (*behaviorFnPtr)(game_obj_ptr);

class GameObject {
public:
    GameObject(Point pos, AssetCache* ac) : GameObject(pos, ac, SDL_Rect()) {}
    GameObject(AssetCache* ac, const SDL_Rect &gb) : GameObject(Point(), ac, gb) {}
    GameObject(Point pos, AssetCache* ac, const SDL_Rect &gb) : pos(pos), assetCache(ac), gameBounds(gb) {
        spawnTime = SDL_GetTicks();
        instanceCount++;
    }
    ~GameObject() {
        instanceCount--;
    }

    virtual void update() {};
    virtual void render() {
        if (killed)
            return;
        texture->render(pos);
    }
    virtual Point getCenter() {
        return Point(pos.x + collisionBox.w / 2, pos.y + collisionBox.h / 2);
    }
    virtual void damage(float amount) {
        health -= amount;
        if (health <= 0) killed = true;
    }
    virtual void kill() {
        killed = true;
    }
    bool isKilled() {
        return killed;
    }
    static int instanceCount;
    bool killed = false;
    SDL_Rect collisionBox;
    
    float health;
    float damageInflicted;
    
    int spawnTime;
    Point pos;
    Point vel;
protected:
    AssetCache* assetCache;
    SDL_Rect gameBounds;
    Texture* texture;
    list<behaviorFnPtr> behaviors;

};

int GameObject::instanceCount;

#endif /* GameObject_h */
