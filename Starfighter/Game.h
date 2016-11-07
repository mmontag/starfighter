//
//  Game.h
//  Starfighter
//
//  Created by Matt Montag on 11/5/16.
//  Copyright © 2016 Patrick Montag. All rights reserved.
//

#ifndef Game_h
#define Game_h

#include "AssetCache.h"
#include "Player.h"
#include "Enemy.h"
#include "Bullet.h"
#include "Missile.h"
#include "Asteroid.h"
#include "Animation.h"
#include "Point.h"
#include "Starfield.h"
#include "Text.h"
#include "HUD.h"
#include "Map.h"

struct ScriptEvent {
    list<EnemyBehavior> behaviors;
    int time;
};

class Game {
public:
    Game(SDL_Renderer* r, const SDL_Rect& gb) {
        renderer = r;
        gameBounds = gb;
        assetCache = new AssetCache(renderer);
        gameObjectLists = { &projectiles, &destructibles, &explosions, &hostiles };
        starfighter = Starfighter(assetCache, &projectiles, gameBounds);
        starfield = Starfield(renderer, gameBounds);
        hud = HUD(&starfighter, assetCache, renderer, gameBounds);
        gameMap = new Map(renderer, gameBounds, "test2.tmx");
    }

    SDL_Renderer* renderer;
    SDL_Rect gameBounds;
    AssetCache* assetCache;
    char scoreTextBuf[200];
    char debugTextBuf[200];
    uint asteroidCounter = 0;
    list<game_obj_ptr> projectiles;
    list<game_obj_ptr> destructibles;
    list<game_obj_ptr> explosions;
    list<game_obj_ptr> hostiles;
    list<list<game_obj_ptr>*> gameObjectLists;
    Starfighter starfighter;
    Starfield starfield;
    Map* gameMap;
    HUD hud;
    int gameStartTime;
    int points = 0;
    list<ScriptEvent> enemyScript = {
        { {SWOOP_DOWN}, 1000 },
        { {SWOOP_DOWN}, 2000 },
        { {SWOOP_DOWN}, 3000 },
        { {SWOOP_DOWN}, 4000 },

        { {SWOOP_DOWN}, 9000 },
        { {SWOOP_DOWN}, 10000 },
        { {SWOOP_DOWN}, 11000 },
        { {SWOOP_DOWN}, 12000 },
    };
    list<ScriptEvent>::iterator currentScriptEvent = enemyScript.begin();

    void run() {
        // main loop flag
        bool quit = false;
        gameStartTime = SDL_GetTicks();

        // event handler
        SDL_Event e;

        // TODO: move this assetCache preloading stuff to a separate method
        assetCache->addSoundToGroup("explosions", "explosion1.wav");
        assetCache->addSoundToGroup("explosions", "explosion2.wav");
        assetCache->addSoundToGroup("explosions", "explosion3.wav");
        assetCache->addSoundToGroup("explosions", "explosion2.wav");
        assetCache->addSoundToGroup("explosions", "explosion3.wav");
        assetCache->addSoundToGroup("explosions", "explosion2.wav");
        assetCache->addSoundToGroup("explosions", "explosion3.wav");

        // object lists
        Text scoreText(Point(5, 5), assetCache, scoreTextBuf);
        Text debugText(Point(5, gameBounds.h - 13), assetCache, debugTextBuf);

        while(!quit) {

            // ------- HANDLE

            while(SDL_PollEvent(&e) != 0) {
                if(e.type == SDL_QUIT) {
                    quit = true;
                }

                starfighter.handleEvent( e );

                if (e.type == SDL_KEYDOWN && e.key.repeat == 0) {
                    switch (e.key.keysym.sym) {
                        case SDLK_r:
                            reset();
                            break;
                        case SDLK_e:
                            destructibles.push_back(game_obj_ptr(new Enemy(assetCache, gameBounds, &starfighter, &hostiles)));
                    }
                }

                // handle the player death
                if (e.type == Starfighter::deathEvent) {
                    printf("Died, resetting score\n");
                    reset();
                    points = 0;
                }
            }

            // ------- UPDATE

            // Execute game script
            while (currentScriptEvent != enemyScript.end() &&
                   SDL_GetTicks() - gameStartTime > currentScriptEvent->time) {
                destructibles.push_front(game_obj_ptr(new Enemy(assetCache, gameBounds, &starfighter, &hostiles,
                                                                currentScriptEvent->behaviors)));
                currentScriptEvent++;
            }

            starfighter.update();

            checkCollisions(&destructibles, &projectiles, &explosions);
            checkCollisions(&destructibles, starfighter, &explosions);
            checkCollisions(&hostiles, starfighter, &explosions);
            if (gameMap->collision(starfighter.collisionBox)) {
                starfighter.damage(1);
            }

            updateAllObjects(gameObjectLists);
            generateAsteroids(destructibles);

            // ------- REMOVE

            for (list<game_obj_ptr>* gameObjectList : gameObjectLists) {
                removeDeadObjects(gameObjectList);
            }

            // ------- RENDER

            starfield.render();
            gameMap->render();

            renderAllObjects(gameObjectLists);

            starfighter.render();
            sprintf(scoreTextBuf, "Score: %d", points);
            scoreText.render();
            sprintf(debugTextBuf, "ast: %lu / proj: %lu / obj: %d",
                    destructibles.size(), projectiles.size(), GameObject::instanceCount);
            debugText.render();
            hud.render();

            SDL_RenderPresent(renderer);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);
        }
    }

    void reset() {
        gameMap->resetPosition();
        gameStartTime = SDL_GetTicks();
        currentScriptEvent = enemyScript.begin();
        projectiles.clear();
        hostiles.clear();
        destructibles.clear();
    }

    void generateAsteroids(list<game_obj_ptr> &asteroids) {
        if (asteroidCounter++ % 10 == 0)
            asteroids.push_back(game_obj_ptr(new Asteroid(assetCache, gameBounds)));
    }

    bool checkCollision(SDL_Rect a, SDL_Rect b) {
        return SDL_HasIntersection(&a, &b);
    }
    
    // check for projetiles against enemies / destructibles
    
    void checkCollisions(list<game_obj_ptr>* destructibles, list<game_obj_ptr>* projectiles, list<game_obj_ptr>* explosions) {
        for (game_obj_ptr d : *destructibles) {
            for (game_obj_ptr p : *projectiles) {
                if (d->killed || p->killed) continue;
                if (checkCollision(d->collisionBox, p->collisionBox)) {
                    p->kill();
                    printf("Enemy health: %f\n", d->health);
                    d->damage(p->damageInflicted);
                    printf("Dmg: %f\n", p->damageInflicted);
                    printf("Enemy health: %f\n", d->health);
                    if (d->killed) {
                        explosions->push_back(game_obj_ptr(new Animation(d->getCenter(), 24, "med_explosion.png", assetCache)));
                        Mix_PlayChannel(-1, assetCache->getSound("explosion1.wav"), 0);
                        points++;
                    } else {
                        explosions->push_back(game_obj_ptr(new Animation(p->getCenter(), 12, "small_explosion.png", assetCache)));
                        Mix_PlayChannel(-1, assetCache->getRandomSoundFromGroup("explosions"), 0);
                    }
                }
            }
        }
    }

    void checkCollisions(list<game_obj_ptr>* destructibles, Starfighter &starfighter, list<game_obj_ptr>* explosions) {
        for (game_obj_ptr d : *destructibles) {
            if (d->killed) continue;
            if (checkCollision(d->collisionBox, starfighter.collisionBox)) {
                Point center = d->getCenter();
                d->damage(1); //(starfighter.damageInflicted);
                starfighter.damage(d->damageInflicted);
                explosions->push_back(game_obj_ptr(new Animation(center, 12, "small_explosion.png", assetCache)));
                Mix_PlayChannel(-1, assetCache->getRandomSoundFromGroup("explosions"), 0);
            }
        }
    }

    void removeDeadObjects(list<game_obj_ptr>* gameObjects);

    void updateAllObjects(list<list<game_obj_ptr>*> &gameObjectLists);
    
    void renderAllObjects(list<list<game_obj_ptr>*> &gameObjectLists);
};

void Game::removeDeadObjects(list<game_obj_ptr> *gameObjects) {
    list<game_obj_ptr>::iterator i = gameObjects->begin();
    while (i != gameObjects->end()) {
        if ((*i)->isKilled()) {
            gameObjects->erase(i++);  // alternatively, i = items.erase(i);
        } else {
            ++i;
        }
    }
}

void Game::updateAllObjects(list<list<game_obj_ptr>*> &gameObjectLists) {
    for (auto gameObjectList : gameObjectLists) {
        for (auto gameObject : *gameObjectList) {
            gameObject->update();
        }
    }
}

void Game::renderAllObjects(list<list<game_obj_ptr>*> &gameObjectLists) {
    for (auto gameObjectList : gameObjectLists) {
        for (auto gameObject : *gameObjectList) {
            gameObject->render();
        }
    }
}

#endif /* Game_h */
