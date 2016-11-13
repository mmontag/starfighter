//
//  Texture.cpp
//  Starfighter
//
//  Created by Matt Montag on 11/8/16.
//  Copyright © 2016 Patrick Montag. All rights reserved.
//

#include "Texture.h"

int Texture::instanceCount;

Texture::Texture( SDL_Renderer* r )
{
    //Initialize
    texture = NULL;
    renderer = r;
    width = 0;
    height = 0;
    instanceCount++;
}

Texture::~Texture()
{
    //Deallocate
    free();
    instanceCount--;
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

void Texture::render( Point pos, SDL_Rect* clip )
{
    render(pos.x, pos.y, clip);
}

void Texture::render( int x, int y, SDL_Rect* clip )
{
    //Set rendering space and render to screen
    SDL_Rect renderQuad = { x, y, width, height };

    if (clip->w == 0 || clip->h == 0)
        clip = NULL;

    //Set clip rendering dimensions
    if( clip != NULL)
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
