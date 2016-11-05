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

SDL_Renderer* ren;
void* sdl_img_loader(const char *path) {
    printf(">>> Loading map image resource %s...\n", path);
    SDL_Texture* img = IMG_LoadTexture(ren, path);
    //SDL_SetTextureAlphaMod(img, 210);
    return img;
}

class Map {
public:
    Map(SDL_Renderer* renderer, SDL_Rect gb, char* filename) {
        ren = renderer;
        gameBounds = gb;
        tmx_img_load_func = (void* (*)(const char*))sdl_img_loader;
        tmx_img_free_func = (void  (*)(void*))      SDL_DestroyTexture;

        // Load TMX Tiled Map Editor map
        tmx_map *map = tmx_load(filename);
        if (!map) {
            tmx_perror("tmx_load");
        } else {
            printf("Loaded map!\n");
        }

        map_rect.w = map->width  * map->tile_width;
        map_rect.h = map->height * map->tile_height;
        map_rect.x = 0;  map_rect.y = 0;

        map_bmp = render_map(map);
        SDL_SetTextureBlendMode(map_bmp, SDL_BLENDMODE_BLEND);
        position = gameBounds.h;
    }

    ~Map() {
        SDL_DestroyTexture(map_bmp);
        //tmx_map_free(map);
    }

    void render() {
        position += 1;
        if (position > map_rect.h)
            position = 0;
        if (position < gameBounds.h) {
            // split render for wrapping
            // TODO: this is not quite correct yet
            SDL_Rect topSrc = { 0, map_rect.h - position, gameBounds.w, position };
            SDL_Rect bottomSrc = { 0, 0, gameBounds.w, gameBounds.h - position };
            SDL_Rect topDest = { 0, 0, gameBounds.w, position };
            SDL_Rect bottomDest = { 0, position, gameBounds.w, gameBounds.h - position };
            SDL_RenderCopy(ren, map_bmp, &topSrc, &topDest);
            SDL_RenderCopy(ren, map_bmp, &bottomSrc, &bottomDest);
        }
        SDL_Rect src = { 0, map_rect.h - position, gameBounds.w, gameBounds.h };
        SDL_RenderCopy(ren, map_bmp, &src, NULL);
    }

private:
    int position = 0;
    tmx_map* map;
    SDL_Texture *map_bmp;
    SDL_Rect map_rect;
    SDL_Rect gameBounds;

//    void* sdl_img_loader(const char *path);
    void draw_layer(tmx_map *map, tmx_layer *layer);
    void draw_image_layer(tmx_image *img);
    SDL_Texture* render_map(tmx_map *map);
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

SDL_Texture* Map::render_map(tmx_map *map) {
    SDL_Texture *res;
    tmx_layer *layers = map->ly_head;
    int w, h;

    w = map->width  * map->tile_width;  /* Bitmap's width and height */
    h = map->height * map->tile_height;

    if (!(res = SDL_CreateTexture(ren, SDL_PIXELFORMAT_RGBA8888, SDL_TEXTUREACCESS_TARGET, w, h)))
        printf("Error creating texture: %s\n", SDL_GetError());
    SDL_SetRenderTarget(ren, res);
    SDL_RenderClear(ren);
    
    

    while (layers) {
        if (layers->visible) {
            if (layers->type == L_OBJGR) {
                printf("TODO: TMX object layers currently unsupported.");
                //draw_objects(layers->content.objgr);
            } else if (layers->type == L_IMAGE) {
                draw_image_layer(layers->content.image);
            } else if (layers->type == L_LAYER) {
                printf("Drawing layer %s\n", layers->name);
                draw_layer(map, layers);
            }
        }
        layers = layers->next;
    }

    SDL_SetRenderTarget(ren, NULL);
    return res;
}




#endif /* Map_h */
