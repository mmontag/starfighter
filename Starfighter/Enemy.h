 //
//  Enemy.h
//  Starfighter
//
//  Created by Matt Montag on 11/6/16.
//  Copyright © 2016 Patrick Montag. All rights reserved.
//

#ifndef Enemy_h
#define Enemy_h

#include "GameObject.h"
#include "GameObjectFactory.h"
#include "AssetCache.h"
#include "ExplodeAction.h"
#include "Player.h"

#include <list>
#include <functional>

enum EnemyBehavior {
    SWOOP_DOWN,
    WOBBLE,
    LEFT_TO_RIGHT,
    ZIG_ZAG
};

class EnemyDeathAction : public Action {
public:
    EnemyDeathAction(GameObject* o) : Action(o, 1) {};
    void update() {
        obj->kill();
        Mix_PlayChannel(-1, obj->assetCache->getSound("explosion1.wav"), 0);
        GameObjectFactory::create("explosion", obj->getCenter());

        Action::update();
    }
};

class Enemy : public GameObject {
public:
    Enemy(AssetCache* ac,
          SDL_Rect &gb,
          Starfighter* s,
          EnemyBehavior behavior = {}) : GameObject(ac, gb) {
        starfighter = s;
        texture = assetCache->getTexture("enemy1.png");
        collisionBox = { (int)pos.x, (int)pos.y, texture->getWidth(), texture->getHeight() };
        health = 5;
        damageInflicted = 2;
        pos = Point(0, 0);

        switch (behavior) {
            case LEFT_TO_RIGHT:
                actionList = ActionList::serial({
                    action_ptr(new MoveTo(this, 1, Point(-collisionBox.w, 120))),
                    action_ptr(new MoveTo(this, 120, Point(gameBounds.w, 120))),
                });
                break;
            case SWOOP_DOWN:
                actionList = ActionList::serial({
                    action_ptr(new MoveTo(this, 1, Point(gameBounds.w * 0.25, -40))),
                    action_ptr(new MoveTo(this, 120, Point(gameBounds.w * 0.50, gameBounds.h - 50))),
                    action_ptr(new MoveTo(this, 120, Point(gameBounds.w * 0.75, -40))),
                });
                break;
            case WOBBLE:
                break;
            case ZIG_ZAG:
                actionList = ActionList::parallel({
                    ActionList::serial({
                        action_ptr(new Delay(0.3)),
                        action_ptr(new MoveTo(this, 60, Point(gameBounds.w * 0.75, 50))),
                        action_ptr(new Delay(0.3)),
                        action_ptr(new MoveTo(this, 60,  Point(gameBounds.w * 0.25, 50))),
                    }, true),
                    ActionList::serial({
                        action_ptr(new Delay(0.1)),
                        action_ptr(new LambdaAction(bind(&Enemy::fire, this))),
                    }, true),
                });
                break;
        }
    }

    void damage(float amount) {
        health -= amount;

        if (health <= 0) {
            static_cast<ActionList*>(actionList.get())->add(new ExplodeAction(this, 80));
            static_cast<ActionList*>(actionList.get())->add(new EnemyDeathAction(this));
        }

        GameObject::damage(amount);
    }

    void fire() {
        // Vector pointing towards player
        Point p = {
            pos.x + collisionBox.w / 2,
            pos.y + collisionBox.h
        };
        Point v = {
            starfighter->getCenter().x - p.x,
            starfighter->getCenter().y - p.y
        };
        float normal = 1 / sqrt(v.x * v.x + v.y * v.y);
        v.x *= normal * bulletSpeed;
        v.y *= normal * bulletSpeed;

        //Mix_PlayChannel(-1, assetCache->getSound("pellet.wav"), 0);
        GameObjectFactory::create("enemy-bullet", p, v);
    }
private:
    Starfighter* starfighter;
    float bulletSpeed = 4.1;
};

class FireAction : public Action {
public:
    FireAction(Enemy* o) : Action(o, 0) {};
    void update() {
        static_cast<Enemy*>(obj)->fire();
        Action::update();
    }
};



#endif /* Enemy_h */
