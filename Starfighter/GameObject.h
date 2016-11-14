//
//  GameObject.h
//  Starfighter
//
//  Created by Patrick Montag on 11/4/16.
//  Copyright © 2016 Patrick Montag. All rights reserved.
//

#ifndef GameObject_h
#define GameObject_h

#include <list>

#include "Point.h"
#include "Texture.h"
#include "AssetCache.h"
#include "Action.h"

//struct ActionList;

class GameObject;
typedef shared_ptr<GameObject> game_obj_ptr;
typedef void (*behaviorFnPtr)(game_obj_ptr);


enum CollisionGroups {
    ENEMY,
    PLAYER,
};

class GameObject {
public:
    GameObject() {
        instanceCount++;
    }
    GameObject(Point pos, AssetCache* ac) : GameObject(pos, ac, SDL_Rect()) {}
    GameObject(AssetCache* ac, const SDL_Rect &gb) : GameObject(Point(), ac, gb) {}
    GameObject(Point pos, AssetCache* ac, const SDL_Rect &gb) : pos(pos), assetCache(ac), gameBounds(gb) {
        spawnTime = SDL_GetTicks();
        actionList = action_ptr(new ActionList());
        instanceCount++;
    }
    ~GameObject() {
        instanceCount--;
    }

    virtual void update() {
        actionList->update();

        if (outOfBoundsKill) {
            if (pos.y < 0 - texture->getHeight() ||
                pos.y > gameBounds.h ||
                pos.x < 0 - texture->getWidth() ||
                pos.x > gameBounds.w) {
                killed = true;
                return;
            }
        }

        collisionBox.x = pos.x;
        collisionBox.y = pos.y;

        for (auto child : children) {
            child->update();
        }
    }

    virtual void render() {
        if (killed)
            return;

        texture->render(pos, &clipRect);

        for (auto child : children) {
            child->render();
        }
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
    bool outOfBoundsKill = false;
    SDL_Rect collisionBox;
    
    float health;
    float damageInflicted;
    
    int spawnTime;
    Point pos;
    Point vel;
    AssetCache* assetCache;
    list<game_obj_ptr> children;
protected:
    SDL_Rect gameBounds;
    RECT clipRect = {0,0,0,0};
    Texture* texture;
    list<behaviorFnPtr> behaviors;
    action_ptr actionList;
};

#endif /* GameObject_h */
