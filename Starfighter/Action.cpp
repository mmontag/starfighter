//
//  Action.cpp
//  Starfighter
//
//  Created by Matt Montag on 11/9/16.
//  Copyright © 2016 Patrick Montag. All rights reserved.
//

#include "Action.h"
#include "GameObject.h"

#pragma mark - ActionList

action_ptr ActionList::serial(list<action_ptr> actions, bool repeat) {
    for (auto action : actions)
        action->blocking = true;

    action_ptr actionList = action_ptr(new ActionList(actions));
    actionList->repeat = repeat;

    return actionList;
}

action_ptr ActionList::parallel(list<action_ptr> actions) {
    for (auto action : actions)
        action->blocking = false;

    action_ptr actionList = action_ptr(new ActionList(actions));
    return actionList;
}

#pragma mark - MoveTo

MoveTo::MoveTo(GameObject* o, int d, Point destination) : Action(o, d), destination(destination) {}

void MoveTo::start() {
    // Calculate constant speed based on distance to destination and duration
    vel.x = float(destination.x - obj->pos.x) / duration;
    vel.y = float(destination.y - obj->pos.y) / duration;

    Action::start();
}

void MoveTo::update() {
    obj->pos.x += vel.x;
    obj->pos.y += vel.y;

    Action::update();
}

#pragma mark - Delay

Delay::Delay(float seconds) {
    this->seconds = seconds;
    this->blocking = true;
}

void Delay::start() {
    startTime = SDL_GetTicks();

    Action::start();
}

void Delay::update() {
    if (SDL_GetTicks() - startTime > seconds * 1000) {
        finished = true;
    }
}
