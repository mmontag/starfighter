//
//  ExplodeAction.h
//  Starfighter
//
//  Created by Patrick Montag on 11/8/16.
//  Copyright © 2016 Patrick Montag. All rights reserved.
//

#ifndef ExplodeAction_h
#define ExplodeAction_h

#include "Action.h"

struct ExplodeAction : public Action {
    ExplodeAction(GameObject* o, int duration = 100) : Action(o, duration) {
        blocking = true;
    }

    void update() {
        if (elapsed % 6 == 0) {
            Mix_PlayChannel(-1, obj->assetCache->getSound("small-blast.wav"), 0);
            Point p = { obj->pos.x + rand() % obj->collisionBox.w, obj->pos.y + rand() % obj->collisionBox.h };
            obj->children.push_back(game_obj_ptr(new Animation(p, 12, "small_explosion.png", obj->assetCache)));
        }
        
        Action::update();
    }
};

#endif /* ExplodeAction_h */
