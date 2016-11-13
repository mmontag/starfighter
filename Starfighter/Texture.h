//
//  Texture.h
//  Starfighter
//
//  Created by Patrick Montag on 10/21/16.
//  Copyright © 2016 Patrick Montag. All rights reserved.
//

#ifndef Texture_h
#define Texture_h

#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "Point.h"

// texture wrapper class
class Texture
{
public:
    // initializes variables
    Texture(SDL_Renderer* r);
    
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
    void render( Point pos, SDL_Rect* clip = NULL);
    void render( int x, int y, SDL_Rect* clip = NULL );
    void render( int x, int y, SDL_Rect* clip, float scale);
    
    // gets image dimensions
    int getWidth();
    int getHeight();
    
    static int instanceCount;
    
private:
    // the actual hardware texture
    SDL_Texture* texture;
    SDL_Renderer* renderer;
    
    // image dimensions
    int width;
    int height;
};

#endif /* Texture_h */
