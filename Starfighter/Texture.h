//
//  Texture.h
//  Starfighter
//
//  Created by Patrick Montag on 10/21/16.
//  Copyright © 2016 Patrick Montag. All rights reserved.
//

#ifndef Texture_h
#define Texture_h

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
    void render( Point pos );
    void render( int x, int y, SDL_Rect* clip = NULL );
    void render( int x, int y, SDL_Rect* clip, float scale);
    
    // gets image dimensions
    int getWidth();
    int getHeight();
    
private:
    // the actual hardware texture
    SDL_Texture* texture;
    SDL_Renderer* renderer;
    
    // image dimensions
    int width;
    int height;
};

Texture::Texture( SDL_Renderer* r )
{
    //Initialize
    texture = NULL;
    renderer = r;
    width = 0;
    height = 0;
}

Texture::~Texture()
{
    //Deallocate
    free();
}

bool Texture::loadFromFile( std::string path )
{
    printf("Loading file %s...\n", path.c_str());
    //Get rid of preexisting texture
    free();
    
    //The final texture
    SDL_Texture* newTexture = NULL;
    
    //Load image at specified path
    SDL_Surface* loadedSurface = IMG_Load( path.c_str() );
    if( loadedSurface == NULL )
    {
        printf( "Unable to load image %s! SDL_image Error: %s\n", path.c_str(), IMG_GetError() );
    }
    else
    {
        //Color key image
        SDL_SetColorKey( loadedSurface, SDL_TRUE, SDL_MapRGB( loadedSurface->format, 0, 0xFF, 0xFF ) );
        
        //Create texture from surface pixels
        newTexture = SDL_CreateTextureFromSurface( renderer, loadedSurface );
        if( newTexture == NULL )
        {
            printf( "Unable to create texture from %s! SDL Error: %s\n", path.c_str(), SDL_GetError() );
        }
        else
        {
            //Get image dimensions
            width = loadedSurface->w;
            height = loadedSurface->h;
        }
        
        //Get rid of old loaded surface
        SDL_FreeSurface( loadedSurface );
    }
    
    //Return success
    texture = newTexture;
    return texture != NULL;
}

void Texture::free()
{
    //Free texture if it exists
    if( texture != NULL )
    {
        SDL_DestroyTexture( texture );
        texture = NULL;
        width = 0;
        height = 0;
    }
}

void Texture::setColor( Uint8 red, Uint8 green, Uint8 blue )
{
    //Modulate texture rgb
    SDL_SetTextureColorMod( texture, red, green, blue );
}

void Texture::setBlendMode( SDL_BlendMode blending )
{
    //Set blending function
    SDL_SetTextureBlendMode( texture, blending );
}

void Texture::setAlpha( Uint8 alpha )
{
    //Modulate texture alpha
    SDL_SetTextureAlphaMod( texture, alpha );
}

void Texture::render( Point pos ) {
    render(pos.x, pos.y);
}

void Texture::render( int x, int y, SDL_Rect* clip )
{
    //Set rendering space and render to screen
    SDL_Rect renderQuad = { x, y, width, height };
    
    //Set clip rendering dimensions
    if( clip != NULL )
    {
        renderQuad.w = clip->w;
        renderQuad.h = clip->h;
    }
    
    //Render to screen
    SDL_RenderCopy( renderer, texture, clip, &renderQuad );
}

void Texture::render(int x, int y, SDL_Rect* clip, float scale) {
    //Set rendering space and render to screen
    SDL_Rect renderQuad = { x, y, static_cast<int>(width * scale), static_cast<int>(height * scale) };

    //Set clip rendering dimensions
    if( clip != NULL )
    {
        renderQuad.w = clip->w * scale;
        renderQuad.h = clip->h * scale;
    }

    //Render to screen
    SDL_RenderCopy( renderer, texture, clip, &renderQuad );
}

int Texture::getWidth()
{
    return width;
}

int Texture::getHeight()
{
    return height;
}

#endif /* Texture_h */
