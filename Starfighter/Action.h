//
//  Action.h
//  Starfighter
//
//  Created by Patrick Montag on 11/8/16.
//  Copyright © 2016 Patrick Montag. All rights reserved.
//

#ifndef Action_h
#define Action_h

#include "Point.h"
#include <list>
#include <functional>

using namespace std;

class GameObject;
struct Action;
typedef shared_ptr<Action> action_ptr;

struct Action {
    Action() : Action(NULL, 0) {}
    Action(GameObject* o, int d) {
        obj = o;
        duration = d;
        instanceCount++;
    }

    ~Action() {
        instanceCount--;
    }

    virtual void start() {
        started = true;
    }

    virtual void update() {
        elapsed++;
        if (elapsed >= duration) {
            if (repeat)
                elapsed = 0;
            else if (duration != -1)
                finished = true;
        }
    }

    void reset() {
        started = false;
        finished = false;
        elapsed = 0;
    }

    bool isFinished() {
        return finished;
    }

    int duration = 0; // -1 means forever
    int elapsed = 0;
    static int instanceCount;
    bool blocking = true;
    bool started = false;
    bool finished = false;
    bool repeat = false;
    GameObject* obj;
};

struct ActionList : public Action {
    ActionList() : Action() {}
    ActionList(GameObject *o) : Action(o, -1) {}
    ActionList(GameObject *o, list<action_ptr> actions);
    ActionList(list<action_ptr> &actions) {
        this->actions = std::move(actions);
    };

    // Convenience methods to set all actions to blocking or non-blocking
    static action_ptr serial(list<action_ptr> actions, bool repeat=false);
    static action_ptr parallel(list<action_ptr> actions);

    void update() {
        if (repeat) {
            // If repeat and all finished, reset all
            if (all_of(actions.begin(), actions.end(), [](action_ptr const& a) { return a->isFinished(); })) {
                for (auto const& action : actions) {
                    action->reset();
                }
            }
        } else {
            // Else, remove completed actions, we won't need them again
            actions.erase(remove_if(actions.begin(), actions.end(),
                                    [](action_ptr const& a) {
                                        return a->isFinished();
                                    }), actions.end());
            finished = actions.size() == 0;
        }

        for (auto const& action : actions) {
            if (action->isFinished())
                continue;

            if (!action->started)
                action->start();

            action->update();

            // Exit early if this action is blocking
            if (action->blocking)
                return;
        }
    }

    void add(Action* action) {
        actions.push_back(action_ptr(action));
    }
    
    list<action_ptr> actions;
};

struct MoveTo : public Action {
    MoveTo(GameObject* o, int d, Point destination);

    void start();
    void update();

    Point vel;
    Point destination;
};

struct Delay : public Action {
    Delay(float seconds);
    void start();
    void update();

    float seconds;
    int startTime;
};

struct LambdaAction : public Action {
    LambdaAction(function<void()> fn) : Action(NULL, 0), fn(fn) {};
    void update() {
        fn();
        finished = true;
    }

    function<void()> fn;
};





#endif /* Action_h */
