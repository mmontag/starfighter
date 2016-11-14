//
//  Texture.h
//  Starfighter
//
//  Created by Patrick Montag on 10/21/16.
//  Copyright © 2016 Patrick Montag. All rights reserved.
//

#ifndef Texture_h
#define Texture_h

//#undef USE_GPU

#ifdef USE_GPU
#include "SDL_gpu.h"
#define RECT GPU_Rect
#define MAKERECT(x,y,w,h) {float(x), float(y), float(w), float(h)}
#define TEXTURE GPU_Image
#define LOADTEXTURE(_, path) GPU_LoadImage(path)
#define FREETEXTURE GPU_FreeImage
#define RENDERER GPU_Target
#else
#define RECT SDL_Rect
#define MAKERECT(x,y,w,h) {int(x), int(y), int(w), int(h)}
#define TEXTURE SDL_Texture
#define LOADTEXTURE(ren, path) IMG_LoadTexture(ren, path)
#define FREETEXTURE SDL_DestroyTexture
#define RENDERER SDL_Renderer
#endif

#include <SDL.h>
#include <string>

#include "Point.h"

// texture wrapper class
class Texture
{
public:
    // initializes variables
    Texture(RENDERER* r);
    // deallocates memory
    ~Texture();
    
    // loads image at specified path
    bool loadFromFile( std::string path );
    
    // deallocates texture
    void free();
    
    // set color modulation
    void setColor( Uint8 red, Uint8 green, Uint8 blue );
    
    // set blending
    void setBlendMode( SDL_BlendMode blending );
    
    // set alpha modulation
    void setAlpha( Uint8 alpha );
    
    // renders texture at given point
    void render( Point pos, RECT* clip = NULL);
    void render( int x, int y, RECT* clip = NULL );
    void render( int x, int y, RECT* clip, float scale);
    
    // gets image dimensions
    int getWidth();
    int getHeight();
    
    static int instanceCount;
    
private:
    // the actual hardware texture
    TEXTURE* texture;
    RENDERER* renderer;

    // image dimensions
    int width;
    int height;
};

#endif /* Texture_h */
