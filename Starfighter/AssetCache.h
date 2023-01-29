//
//  AssetCache.h
//  Starfighter
//
//  Created by Patrick Montag on 10/30/16.
//  Copyright © 2016 Patrick Montag. All rights reserved.
//

#ifndef AssetCache_h
#define AssetCache_h

#include <map>
#include <string>
#include <random>
#include <iterator>
#include <SDL_mixer.h>

#include "Texture.h"
using namespace std;

class AssetCache {
private:
    map<string, Texture*> textures;
    map<string, Mix_Chunk*> sounds;
    map<string, vector<Mix_Chunk*>*> soundGroups;
    RENDERER* renderer;

    template<typename Iter, typename RandomGenerator>
    Iter select_randomly(Iter start, Iter end, RandomGenerator& g) {
        uniform_int_distribution<> dis(0, (int)(distance(start, end) - 1));
        advance(start, dis(g));
        return start;
    }

    template<typename Iter>
    Iter select_randomly(Iter start, Iter end) {
        static random_device rd;
        static mt19937 gen(rd());
        return select_randomly(start, end, gen);
    }

public:
    AssetCache(RENDERER* r) : renderer(r) {};

    void addSoundToGroup(string groupId, string filename) {
        vector<Mix_Chunk*>* list = new vector<Mix_Chunk*>();
        if (soundGroups.find(groupId) == soundGroups.end()) {
            soundGroups[groupId] = list;
        } else {
            list = soundGroups[groupId];
        }
        Mix_Chunk* sound = getSound(filename);
        list->push_back(sound);
    }

    Mix_Chunk* getRandomSoundFromGroup(string groupId) {
        if (soundGroups.find(groupId) != soundGroups.end()) {
            vector<Mix_Chunk*>* list = soundGroups[groupId];
            return *select_randomly(list->begin(), list->end());
        } else {
            printf("Sound group %s not found", groupId.c_str());
            return NULL;
        }
    }

    Mix_Chunk* getSound(string filename) {
        if (sounds.find(filename) == sounds.end()) {
            Mix_Chunk* sound = Mix_LoadWAV(filename.c_str());
            if (!sound) {
                printf("Mix_LoadWAV error: %s\n", Mix_GetError());
            } else {
                printf("Loaded %s\n", filename.c_str());
            }
            sounds[filename] = sound;
            return sound;
        } else {
            return sounds[filename];
        }
    };

    Texture* getTexture(string filename) {
        if (textures.find(filename) == textures.end()) {
            Texture* texture = new Texture(renderer);
            if(!texture->loadFromFile(filename)) {
                printf("Failed to load %s\n", filename.c_str());
                return NULL;
            } else {
                printf("Loaded %s\n", filename.c_str());
            }
            textures[filename] = texture;
            return texture;
        } else {
            return textures[filename];
        }
    };
};

#endif /* AssetCache_h */
