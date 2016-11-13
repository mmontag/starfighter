//
//  main.cpp
//  Starfighter
//
//  Created by Patrick Montag on 10/20/16.
//  Copyright © 2016 Patrick Montag. All rights reserved.
//

#include <SDL2/SDL.h>
#include <SDL2/SDL_gamecontroller.h> // game controller api
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>
#include <iostream>
#include <string>
#include <time.h> // time
#include <stdlib.h> // srand and rand
#include <vector> // vector
#include <deque>
#include <list>

#include "Game.h"

// logical resolution
const int LOGICAL_WIDTH = 320;
const int LOGICAL_HEIGHT = 280;
const SDL_Rect gameBounds = { 0, 0, LOGICAL_WIDTH, LOGICAL_HEIGHT };

// window scale factor
const int SCREEN_SCALE_FACTOR = 3;

// screen dimension constants
const int SCREEN_WIDTH = (gameBounds.w * SCREEN_SCALE_FACTOR);
const int SCREEN_HEIGHT = (gameBounds.h * SCREEN_SCALE_FACTOR);

int FRAME_TIME = 0;

//Starts up SDL and creates window
bool init();

//Frees media and shuts down SDL
void close();

//The window we'll be rendering to
SDL_Window* window = NULL;

//The window renderer
SDL_Renderer* renderer = NULL;

using namespace std;

bool init() {
    //Initialization flag
    bool success = true;
    
    //Initialize SDL
    if( SDL_Init(SDL_INIT_EVERYTHING) < 0) {
        printf("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        success = false;
    }
    else {
        // set texture filtering to linear
        if(!SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "0")) {
            printf("Warning: Linear texture filtering not enabled!");
        }
        
        SDL_GameController *controller = nullptr;
        
        for(int i = 0; i < SDL_NumJoysticks(); i++) {
            if(SDL_IsGameController(i)) {
                controller = SDL_GameControllerOpen(i);
                break;
            }
        }
        
        // connect controller
        if (controller) {
            printf("found a valid controller, named: %s\n", SDL_GameControllerName(controller));
        }

        // init audio
        if(Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 256) < 0)
            printf("Mix_OpenAudio error: %s\n", SDL_GetError());
        Mix_AllocateChannels(32);

        // load support for the MOD sample/music formats
        int flags = MIX_INIT_MODPLUG | MIX_INIT_MP3;
        int inited = Mix_Init(flags);
        if((inited & flags) != flags) {
            printf("Mix_Init error: %s\n", Mix_GetError());
        }
        // print music decoders available
        int i,max=Mix_GetNumMusicDecoders();
        for(i=0; i<max; ++i)
            printf("Music decoder %d: %s\n", i, Mix_GetMusicDecoder(i));
        Mix_Music *music;
        music = Mix_LoadMUS("lifefrce.mp3");
        Mix_VolumeMusic(MIX_MAX_VOLUME/16);
        if(!music) {
            printf("Mix_LoadMUS error: %s\n", Mix_GetError());
        } else {
            // play music forever
            if(Mix_PlayMusic(music, -1) == -1) {
                printf("Mix_PlayMusic error: %s\n", Mix_GetError());
            }
        }

        //Create window
        window = SDL_CreateWindow("SDL Tutorial", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
        if(window == NULL) {
            printf("Window could not be created! SDL Error: %s\n", SDL_GetError());
            success = false;
        }
        else {
            // Create vsynced renderer for window
            renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC );
            if( renderer == NULL) {
                printf( "Renderer could not be created! SDL Error: %s\n", SDL_GetError());
                success = false;
            }
            else {
                SDL_RenderSetLogicalSize(renderer, gameBounds.w, gameBounds.h);

                // initialize PNG loading
                int imgFlags = IMG_INIT_PNG;
                if(!(IMG_Init(imgFlags) & imgFlags)) {
                    printf("SDL_image could not initialize! SDL_image Error: %s\n", IMG_GetError());
                    success = false;
                }
            }
        }
    }

    return success;
}

void close() {
    //Destroy window
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    window = NULL;
    renderer = NULL;
    
    //Quit SDL subsystems
    Mix_CloseAudio();
    IMG_Quit();
    SDL_Quit();
}

int main(int argc, char* args[]) {
    IMG_Init(IMG_INIT_PNG);
    
    // Start up SDL and create window
    if(!init()) {
        printf("Failed to initialize!\n");
    } else {
        Game game(renderer, gameBounds);
        game.run();
    }
    // free resources and close SDL
    close();
    return 0;
}
