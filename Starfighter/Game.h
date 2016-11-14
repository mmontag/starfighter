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
#ifndef USE_GPU
#include "Map.h"
#endif

struct ScriptEvent {
    EnemyBehavior behavior;
    int time;
};

// Static member initializers for classes without .cpp files
int Action::instanceCount;
int GameObject::instanceCount;

class Game {
public:
    Game(RENDERER* r, const SDL_Rect& gb) {
        renderer = r;
        gameBounds = gb;
        assetCache = new AssetCache(renderer);
        preloadAssets();

        gameObjectLists = { &projectiles, &destructibles, &sprites, &hostiles };
        starfighter = Starfighter(assetCache, &projectiles, gameBounds);
        starfield = Starfield(renderer, gameBounds);
#ifndef USE_GPU
        hud = HUD(&starfighter, assetCache, renderer, gameBounds);
        gameMap = new Map(renderer, gameBounds, "test2.tmx");
#endif
        GameObjectFactory::assetCache = assetCache;
        GameObjectFactory::sprites = &sprites;
        GameObjectFactory::hostiles = &hostiles;
        GameObjectFactory::projectiles = &projectiles;
        GameObjectFactory::destructibles = &destructibles;
        GameObjectFactory::starfighter = &starfighter;
        GameObjectFactory::gameBounds = gb;
    }

    RENDERER* renderer;
    SDL_Rect gameBounds;
    AssetCache* assetCache;
    char scoreTextBuf[200];
    char debugTextBuf[200];
    uint asteroidCounter = 0;
    list<game_obj_ptr> projectiles;
    list<game_obj_ptr> destructibles;
    list<game_obj_ptr> sprites;
    list<game_obj_ptr> hostiles;
    list<list<game_obj_ptr>*> gameObjectLists;
    Starfighter starfighter;
    Starfield starfield;
#ifndef USE_GPU
    Map* gameMap;
#endif
    HUD hud;
    int gameStartTime;
    int points = 0;
    list<ScriptEvent> enemyScript = {
        { ZIG_ZAG, 1000 },
        { ZIG_ZAG, 2000 },
        { ZIG_ZAG, 3000 },
        { ZIG_ZAG, 4000 },
        { ZIG_ZAG, 5000 },
        { LEFT_TO_RIGHT, 6000 },
        { SWOOP_DOWN, 9000 },
        { SWOOP_DOWN, 10000 },
        { SWOOP_DOWN, 11000 },
        { SWOOP_DOWN, 12000 },
    };
    list<ScriptEvent>::iterator currentScriptEvent = enemyScript.begin();

//    void setupGLStuff(){
//        /* Enable smooth shading */
//        glShadeModel( GL_SMOOTH );
//
//        /* Set the background black */
//        glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
//
//        /* Depth buffer setup */
//        glClearDepth( 1.0f );
//
//        /* Enables Depth Testing */
//        glEnable( GL_DEPTH_TEST );
//
//        /* The Type Of Depth Test To Do */
//        glDepthFunc( GL_LEQUAL );
//
//        /* Really Nice Perspective Calculations */
//        glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
//
//        /* Height / width ration */
//        GLfloat ratio;
//        ratio = ( GLfloat )gameBounds.w / ( GLfloat )gameBounds.h;
//
//        /* Setup our viewport. */
//        glViewport( 0, 0, ( GLsizei )gameBounds.w, ( GLsizei )gameBounds.h );
//
//        /* change to the projection matrix and set our viewing volume. */
//        glMatrixMode( GL_PROJECTION );
//        glLoadIdentity( );
//
//        /* Set our perspective */
//        gluPerspective( 45.0f, ratio, 0.1f, 100.0f );
//
//        /* Make sure we're chaning the model view and not the projection */
//        glMatrixMode( GL_MODELVIEW );
//
//        /* Reset The View */
//        glLoadIdentity( );
//    }
//
//    void renderGLStuff() {
//        /* Set the background black */
//        glClearColor( 0.0f, 0.0f, 0.0f, 0.0f );
//        /* Clear The Screen And The Depth Buffer */
//        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
//
//        /* Move Left 1.5 Units And Into The Screen 6.0 */
//        glLoadIdentity();
////        glTranslatef( -1.5f, 0.0f, -6.0f );
//
//        glBegin( GL_TRIANGLES );            /* Drawing Using Triangles */
//        glVertex3f(  0.0f,  0.0f, 0.0f ); /* Top */
//        glVertex3f( -1.0f, -1.0f, 0.0f ); /* Bottom Left */
//        glVertex3f(  1.0f, -1.0f, 0.0f ); /* Bottom Right */
//        glEnd( );                           /* Finished Drawing The Triangle */
//
//        /* Move Right 3 Units */
////        glTranslatef( 3.0f, 0.0f, 0.0f );
////        glBegin( GL_QUADS );                /* Draw A Quad */
////        glVertex3f( -1.0f,  1.0f, 0.0f ); /* Top Left */
////        glVertex3f(  1.0f,  1.0f, 0.0f ); /* Top Right */
////        glVertex3f(  1.0f, -1.0f, 0.0f ); /* Bottom Right */
////        glVertex3f( -1.0f, -1.0f, 0.0f ); /* Bottom Left */
////        glEnd( );                           /* Done Drawing The Quad */
//    }

    void run() {
        // main loop flag
        bool quit = false;
        gameStartTime = SDL_GetTicks();

        // event handler
        SDL_Event e;

        // object lists
        Text scoreText(Point(5, 5), assetCache, scoreTextBuf);
        Text debugText(Point(5, gameBounds.h - 13), assetCache, debugTextBuf);

        // OpenGL
      //  setupGLStuff();

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
                            GameObjectFactory::create("enemy");
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
                destructibles.push_front(game_obj_ptr(new Enemy(assetCache, gameBounds, &starfighter,
                                                                currentScriptEvent->behavior)));
                currentScriptEvent++;
            }

            starfighter.update();

            checkCollisions(&destructibles, &projectiles);
            checkCollisions(&destructibles, starfighter);
            checkCollisions(&hostiles, starfighter);
//            if (gameMap->collision(starfighter.collisionBox)) {
//                starfighter.damage(1);
//            }

            updateAllObjects(gameObjectLists);
            //generateAsteroids(destructibles);

            // ------- REMOVE

            for (list<game_obj_ptr>* gameObjectList : gameObjectLists) {
                removeDeadObjects(gameObjectList);
            }

            // ------- RENDER

            starfield.render();

           // renderGLStuff();
#ifndef USE_GPU
            gameMap->render();
#endif

            renderAllObjects(gameObjectLists);
            starfighter.render();

#ifndef USE_GPU
            gameMap->renderAbove();
#endif
            sprintf(scoreTextBuf, "Score: %d", points);
            scoreText.render();

            sprintf(debugTextBuf, "go: %d / d: %lu / p: %lu / t: %d / a: %d",
                    GameObject::instanceCount, destructibles.size(), projectiles.size(), Texture::instanceCount, Action::instanceCount);
            debugText.render();
#ifndef USE_GPU
            hud.render();
#endif

#ifdef USE_GPU
            GPU_Flip(renderer);
            GPU_Clear(renderer);
#else
            SDL_RenderPresent(renderer);
            SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
            SDL_RenderClear(renderer);
#endif
        }
    }

    void reset() {
#ifndef USE_GPU
        gameMap->resetPosition();
#endif
        gameStartTime = SDL_GetTicks();
        currentScriptEvent = enemyScript.begin();
        projectiles.clear();
        hostiles.clear();
        destructibles.clear();
        sprites.clear();
    }

    void preloadAssets() {
        // TODO: move this assetCache preloading stuff to a separate method
        assetCache->addSoundToGroup("explosions", "explosion1.wav");
        assetCache->addSoundToGroup("explosions", "explosion2.wav");
        assetCache->addSoundToGroup("explosions", "explosion3.wav");
        assetCache->addSoundToGroup("explosions", "explosion2.wav");
        assetCache->addSoundToGroup("explosions", "explosion3.wav");
        assetCache->addSoundToGroup("explosions", "explosion2.wav");
        assetCache->addSoundToGroup("explosions", "explosion3.wav");
    }

    void generateAsteroids(list<game_obj_ptr> &asteroids) {
        if (asteroidCounter++ % 10 == 0)
            asteroids.push_back(game_obj_ptr(new Asteroid(assetCache, gameBounds)));
    }

    bool checkCollision(SDL_Rect a, SDL_Rect b) {
        return SDL_HasIntersection(&a, &b);
    }
    
    // check for projetiles against enemies / destructibles
    
    void checkCollisions(list<game_obj_ptr>* destructibles, list<game_obj_ptr>* projectiles) {
        for (game_obj_ptr d : *destructibles) {
            for (game_obj_ptr p : *projectiles) {
                if (d->killed || p->killed) continue;
                if (checkCollision(d->collisionBox, p->collisionBox)) {
                    p->kill();
                    d->damage(p->damageInflicted);
                    if (d->killed) {
                        sprites.push_back(game_obj_ptr(new Animation(d->getCenter(), 32, "enemy_explosion.png", assetCache)));
                        Mix_PlayChannel(-1, assetCache->getSound("explosion1.wav"), 0);
                        points++;
                    } else {
                        sprites.push_back(game_obj_ptr(new Animation(p->getCenter(), 12, "small_explosion.png", assetCache)));
                        Mix_PlayChannel(-1, assetCache->getRandomSoundFromGroup("explosions"), 0);
                    }
                }
            }
        }
    }

    // check for starfighter flying into stuff
    
    void checkCollisions(list<game_obj_ptr>* destructibles, Starfighter &starfighter) {
        for (game_obj_ptr d : *destructibles) {
            if (d->killed) continue;
            if (checkCollision(d->collisionBox, starfighter.collisionBox)) {
                d->damage(starfighter.damageInflicted);
                starfighter.damage(d->damageInflicted);
                sprites.push_back(game_obj_ptr(new Animation(d->getCenter(), 12, "small_explosion.png", assetCache)));
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
