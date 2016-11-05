//
//  Game.h
//  Starfighter
//
//  Created by Matt Montag on 11/5/16.
//  Copyright © 2016 Patrick Montag. All rights reserved.
//

#ifndef Game_h
#define Game_h

class Game {
    Game() {

    }


    void gameLoop() {
        // main loop flag
        bool quit = false;

        // event handler
        SDL_Event e;

        // initialize Asset Cache
        AssetCache* assetCache = new AssetCache(renderer);

        // TODO: move this assetCache preloading stuff to a separate method
        assetCache->addSoundToGroup("explosions", "explosion1.wav");
        assetCache->addSoundToGroup("explosions", "explosion2.wav");
        assetCache->addSoundToGroup("explosions", "explosion3.wav");
        assetCache->addSoundToGroup("explosions", "explosion2.wav");
        assetCache->addSoundToGroup("explosions", "explosion3.wav");
        assetCache->addSoundToGroup("explosions", "explosion2.wav");
        assetCache->addSoundToGroup("explosions", "explosion3.wav");

        // object lists
        list<game_obj_ptr> projectiles;
        list<game_obj_ptr> asteroids;
        list<game_obj_ptr> explosions;
        list<list<game_obj_ptr>> gameObjectLists{ projectiles, asteroids, explosions };
        Starfighter starfighter(assetCache, projectiles, gameBounds);
        Starfield starfield(renderer, gameBounds);
        Text score(Point(5, 5), assetCache, scoreBuf);
        HUD hud(&starfighter, assetCache, renderer, gameBounds);
        Map gameMap(renderer, gameBounds, "test2.tmx");

        // while application is running
        while(!quit) {
            // handle events on queue
            while(SDL_PollEvent(&e) != 0) {
                // user requests quit
                if(e.type == SDL_QUIT) {
                    quit = true;
                }
                // handle input for the hero
                starfighter.handleEvent( e );

                // handle the player death
                if (e.type == Starfighter::deathEvent) {
                    printf("Died, resetting score\n");
                    asteroidsDestroyed = 0;
                }
            }

            // -------- UPDATE

            starfighter.update();

            checkCollisions(asteroids, projectiles, explosions);
            checkCollisions(asteroids, starfighter, explosions);

            updateAllObjects(gameObjectLists);

            //-------- RENDER

            starfield.render();
            gameMap.render();

            renderAllObjects(gameObjectLists);

            starfighter.render();

            sprintf(scoreBuf, "Score: %d", asteroidsDestroyed);
            score.render();
            hud.render();

            if (asteroidCounter++ % 5 == 0)
                asteroids.push_back(game_obj_ptr(new Asteroid(assetCache, gameBounds)));

            if (asteroids.size() > 1000)
                asteroids.pop_front();

            // TODO: make object pools for each class
            for (auto gameObjectList : gameObjectLists) {
                removeDeadObjects(gameObjectList);
            }

            // printf("asteroid count: %lu, bullet count: %lu", asteroids.size(), starfighterBullets.size());

            // update screen
            SDL_RenderPresent(renderer);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);
        }
    }

    bool checkCollision(SDL_Rect a, SDL_Rect b) {
        return SDL_HasIntersection(&a, &b);
    }

    void checkCollisions(list<game_obj_ptr> &asteroids, list<game_obj_ptr> &starfighterBullets, list<game_obj_ptr> &explosions) {
        for (game_obj_ptr a : asteroids) {
            for (game_obj_ptr b : starfighterBullets) {
                if (a->killed || b->killed) continue;
                if (checkCollision(a->collisionBox, b->collisionBox)) {
                    Point center = b->getCenter();
                    Point asteroidCenter = a->getCenter();
                    b->kill();
                    a->damage();
                    if (a->killed) {
                        explosions.push_back(game_obj_ptr(new Animation(asteroidCenter, 24, "med_explosion.png", assetCache)));
                        Mix_PlayChannel(-1, assetCache->getSound("explosion1.wav"), 0);
                        asteroidsDestroyed++;
                    }
                    // printf("Score: %d\n", asteroidsDestroyed);
                    // TODO: Stop allocating so many explosions
                    else {
                        explosions.push_back(game_obj_ptr(new Animation(center, 12, "small_explosion.png", assetCache)));
                        Mix_PlayChannel(-1, assetCache->getRandomSoundFromGroup("explosions"), 0);
                    }
                }
            }
        }
    }

    void checkCollisions(list<game_obj_ptr> &asteroids, Starfighter &starfighter, list<game_obj_ptr> &explosions) {
        for (game_obj_ptr a : asteroids) {
            if (a->killed) continue;
            if (checkCollision(a->collisionBox, starfighter.collisionBox)) {
                Point center = a->getCenter();
                a->kill();
                explosions.push_back(game_obj_ptr(new Animation(center, 12, "small_explosion.png", assetCache)));
                Mix_PlayChannel(-1, assetCache->getRandomSoundFromGroup("explosions"), 0);
                starfighter.damage();
            }
        }
    }

    void removeDeadObjects(list<game_obj_ptr>& gameObjects) {
        list<game_obj_ptr>::iterator i = gameObjects.begin();
        while (i != gameObjects.end()) {
            if ((*i)->killed) {
                gameObjects.erase(i++);  // alternatively, i = items.erase(i);
            } else {
                ++i;
            }
        }
    }

    void updateAllObjects(list<list<game_obj_ptr>>& gameObjectLists) {
        for (auto gameObjectList : gameObjectLists) {
            for (auto gameObject : gameObjectList) {
                gameObject->update();
            }
        }
    }
    
    void renderAllObjects(list<list<game_obj_ptr>>& gameObjectLists) {
        for (auto gameObjectList : gameObjectLists) {
            for (auto gameObject : gameObjectList) {
                gameObject->render();
            }
        }
    }
};


#endif /* Game_h */
