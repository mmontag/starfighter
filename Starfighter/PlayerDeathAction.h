//
//  PlayerDeathAction.h
//  Starfighter
//
//  Created by Matt Montag on 11/8/16.
//  Copyright © 2016 Patrick Montag. All rights reserved.
//

#ifndef PlayerDeathAction_h
#define PlayerDeathAction_h

#include "GameObject.h"
#include "Player.h"

class PlayerDeathAction : public Action {
public:
    PlayerDeathAction(GameObject* o) : Action(o, 1) {};
    void update() {
        SDL_Event event;
        event.type = Starfighter::deathEvent;
        SDL_PushEvent(&event);
        static_cast<Starfighter*>(obj)->resetPosition();
        obj->health = 5;

        Action::update();
    }
};

#endif /* PlayerDeathAction_h */
