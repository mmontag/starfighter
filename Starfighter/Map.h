//
//  Map.h
//  Starfighter
//
//  Created by Matt Montag on 11/4/16.
//  Copyright © 2016 Patrick Montag. All rights reserved.
//
//  Yo! Use http://www.mapeditor.org/ to edit maps!
//

#ifndef Map_h
#define Map_h

#import "tmx.h"
#import <map>
#import <list>

SDL_Renderer* ren;
void* sdl_img_loader(const char *path) {
    printf(">>> Loading map image resource %s...\n", path);
    SDL_Texture* img = IMG_LoadTexture(ren, path);
    return img;
}

struct MapLayer {
    SDL_Texture* texture;
    float parallaxSpeed;
};

class Map {
public:
    Map() {}
    Map(SDL_Renderer* renderer, SDL_Rect gb, char const* filename) {
        ren = renderer;
        gameBounds = gb;
        tmx_img_load_func = (void* (*)(const char*))sdl_img_loader;
        tmx_img_free_func = (void  (*)(void*))      SDL_DestroyTexture;

        // Load TMX Tiled Map Editor map
        gameMap = tmx_load(filename);
        if (!gameMap) {
            tmx_perror("tmx_load");
        } else {
            printf("Loaded map!\n");
        }

        map_rect.w = gameMap->width  * gameMap->tile_width;
        map_rect.h = gameMap->height * gameMap->tile_height;
        map_rect.x = 0;  map_rect.y = 0;

        mapLayers = render_map(gameMap);


        resetPosition();
        getCollidingLayers();
        saveSurfaces();
    }

    ~Map() {
        for (auto mapLayer : mapLayers)
            SDL_DestroyTexture(mapLayer.texture);
        //tmx_map_free(map);
    }

    void resetPosition() {
        position = map_rect.h - gameBounds.h;
    }

    void getCollidingLayers() {
        tmx_layer* layer = gameMap->ly_head;
        while (layer) {
            tmx_property* prop = layer->properties;
            while (prop) {
                if (strcmp(prop->name, "collides") == 0 && strcmp(prop->value, "true") == 0) {
                    collidingLayers.push_back(layer);
                }
                prop = prop->next;
            }
            layer = layer->next;
        }
    }

    float getParallax(tmx_layer* layer) {
        tmx_property* prop = layer->properties;
        while (prop) {
            if (strcmp(prop->name, "parallax") == 0) {
                return strtof(prop->value, NULL);
            }
            prop = prop->next;
        }
        return 1.0;
    }

    void saveSurfaces() {
        tmx_tileset* tileset = gameMap->ts_head;
        while(tileset) {
            SDL_Surface* surface = IMG_Load(tileset->image->source);
            tileSurfaceMap[tileset->image] = surface;
            tileset = tileset->next;
        }
    }

    SDL_Rect gameToTileRect(SDL_Rect rect) {
        return SDL_Rect{
            int(rect.x / gameMap->tile_width),
            int(((rect.y + position + map_rect.h) % map_rect.h) / gameMap->tile_height),
            int(rect.w / gameMap->tile_width),
            int(rect.h / gameMap->tile_height)
        };
    }

    bool collision(SDL_Rect bbox) {
        // As currently drawn, what map coordinates intersect with the subject's bounding box?
        SDL_Rect tileBbox = gameToTileRect(bbox);
        // For each intersecting tile coordinate...
        for (int row = tileBbox.y; row <= tileBbox.y + tileBbox.h; row++) {
            if (row >= gameMap->height) break;
            for (int col = tileBbox.x; col <= tileBbox.x + tileBbox.w; col++) {
                if (col >= gameMap->width) break;
                // For each colliding layer...
                for (auto layer : collidingLayers) {
                    // What tileset appears at these coordinates?
                    int id = row * gameMap->width + col;
                    unsigned int gid = gid_clear_flags(layer->content.gids[id]);
                    tmx_tile* tile = gameMap->tiles[gid];
                    if (tile != NULL) {
                        tmx_image* im = tile->tileset->image;
                        SDL_Surface* surface = tileSurfaceMap[im];
                        SDL_Rect rect = {
                            static_cast<int>(tile->ul_x),
                            static_cast<int>(tile->ul_y),
                            static_cast<int>(gameMap->tile_width),
                            static_cast<int>(gameMap->tile_height)
                        };

                        // Scan the pixels in this tile for opacity
                        for (int x = rect.x; x < rect.x + rect.w; x++) {
                            for (int y = rect.y; y < rect.y + rect.h; y++) {
                                Uint32* pixel = (Uint32*)surface->pixels + y * surface->pitch + x * sizeof *pixel;
                                Uint8 ignored, alpha = 0;
                                SDL_GetRGBA(*pixel, surface->format, &ignored, &ignored, &ignored, &alpha);
                                if (alpha >= 128) {
                                    return true;
                                }
                            }
                        }
                    }
                }
            }
        }

        return false;
    }

    void render() {
        position -= 1;
        if (position < 0) {
            if (loop == false)
                position = 0;
            else
                position = map_rect.h;
        }
//        if (position < gameBounds.h) {
//            // split render for wrapping
//            // TODO: this is not quite correct yet
//            SDL_Rect topSrc = { 0, map_rect.h - position, gameBounds.w, position };
//            SDL_Rect bottomSrc = { 0, 0, gameBounds.w, gameBounds.h - position };
//            SDL_Rect topDest = { 0, 0, gameBounds.w, position };
//            SDL_Rect bottomDest = { 0, position, gameBounds.w, gameBounds.h - position };
//            SDL_RenderCopy(ren, map_bmp, &topSrc, &topDest);
//            SDL_RenderCopy(ren, map_bmp, &bottomSrc, &bottomDest);
//        }
        for (auto mapLayer : mapLayers) {
            if (mapLayer.parallaxSpeed > 1)
                continue;
            SDL_Rect src = { 0, int(position * mapLayer.parallaxSpeed), gameBounds.w, gameBounds.h };
            SDL_RenderCopy(ren, mapLayer.texture, &src, NULL);
        }
    }

    void renderAbove() {
        for (auto mapLayer : mapLayers) {
            if (mapLayer.parallaxSpeed <= 1)
                continue;
            SDL_Rect src = { 0, int(position * mapLayer.parallaxSpeed), gameBounds.w, gameBounds.h };
            SDL_RenderCopy(ren, mapLayer.texture, &src, NULL);
        }
    }

private:
    bool loop = false;
    int position;
    tmx_map* gameMap;
    list<MapLayer> mapLayers;
    SDL_Rect map_rect;
    SDL_Rect gameBounds;
    map<tmx_image*, SDL_Surface*> tileSurfaceMap;
    list<tmx_layer*> collidingLayers;

//    void* sdl_img_loader(const char *path);
    void draw_layer(tmx_map *map, tmx_layer *layer);
    void draw_image_layer(tmx_image *img);
    list<MapLayer> render_map(tmx_map *map);
    unsigned int gid_clear_flags(unsigned int gid);
};

unsigned int Map::gid_clear_flags(unsigned int gid) {
    return gid & TMX_FLIP_BITS_REMOVAL;
}

void Map::draw_layer(tmx_map *map, tmx_layer *layer) {
    unsigned long i, j;
    unsigned int gid;
    float op;
    tmx_tileset *ts;
    tmx_image *im;
    SDL_Rect srcrect, dstrect;
    SDL_Texture* tileset;
    op = layer->opacity;
    for (i=0; i<map->height; i++) {
        for (j=0; j<map->width; j++) {
            gid = gid_clear_flags(layer->content.gids[(i*map->width)+j]);
            if (map->tiles[gid] != NULL) {
                ts = map->tiles[gid]->tileset;
                im = map->tiles[gid]->image;
                srcrect.x = map->tiles[gid]->ul_x;
                srcrect.y = map->tiles[gid]->ul_y;
                srcrect.w = dstrect.w = ts->tile_width;
                srcrect.h = dstrect.h = ts->tile_height;
                dstrect.x = (int)j*ts->tile_width;
                dstrect.y = (int)i*ts->tile_height;
                /* TODO Opacity and Flips */
                if (im) {
                    tileset = (SDL_Texture*)im->resource_image;
                }
                else {
                    tileset = (SDL_Texture*)ts->image->resource_image;
                }
                SDL_RenderCopy(ren, tileset, &srcrect, &dstrect);
            }
        }
    }
}


void Map::draw_image_layer(tmx_image *img) {
    SDL_Rect dim;

    dim.x = dim.y = 0;
    SDL_QueryTexture((SDL_Texture*)img->resource_image, NULL, NULL, &(dim.w), &(dim.h));

    SDL_RenderCopy(ren, (SDL_Texture*)img->resource_image, NULL, &dim);
    
}

list<MapLayer> Map::render_map(tmx_map *map) {
    SDL_Texture* texture;
    tmx_layer* layer = map->ly_head;
    int w, h;

    w = map->width  * map->tile_width;  /* Bitmap's width and height */
    h = map->height * map->tile_height;
    list<MapLayer> mapLayers;

    while (layer) {
        if (layer->visible) {
            if (!(texture = SDL_CreateTexture(ren, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h)))
                printf("Error creating texture: %s\n", SDL_GetError());
            SDL_SetTextureBlendMode(texture, SDL_BLENDMODE_BLEND);
            SDL_SetRenderTarget(ren, texture);
            SDL_SetTextureAlphaMod(texture, 255 * layer->opacity);
            SDL_RenderClear(ren);
            mapLayers.push_back(MapLayer{texture, getParallax(layer)});
            
            if (layer->type == L_OBJGR) {
                printf("TODO: TMX object layers currently unsupported.");
                //draw_objects(layers->content.objgr);
            } else if (layer->type == L_IMAGE) {
                draw_image_layer(layer->content.image);
            } else if (layer->type == L_LAYER) {
                printf("Drawing layer %s\n", layer->name);
                draw_layer(map, layer);
            }
        }
        layer = layer->next;
    }

    SDL_SetRenderTarget(ren, NULL);
    return mapLayers;
}




#endif /* Map_h */
