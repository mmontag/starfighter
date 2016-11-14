//
//  Texture.cpp
//  Starfighter
//
//  Created by Matt Montag on 11/8/16.
//  Copyright © 2016 Patrick Montag. All rights reserved.
//

#include "Texture.h"
#include <SDL_image.h>

int Texture::instanceCount;

Texture::Texture(RENDERER* r)
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
    //Get rid of preexisting texture
    free();

    //Load image at specified path
#ifdef USE_GPU
    texture = GPU_LoadImage(path.c_str());
    GPU_SetImageFilter(texture, GPU_FILTER_NEAREST); // Nice & crispy pixels 😉
    width = texture->w;
    height = texture->h;
#else
    //The final texture
    SDL_Texture* newTexture = NULL;
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
    texture = newTexture;
#endif
    //Return success
    return texture != NULL;
}

void Texture::free()
{
    //Free texture if it exists
    if( texture != NULL )
    {
        FREETEXTURE(texture);
        texture = NULL;
        width = 0;
        height = 0;
    }
}
//
//void Texture::setColor( Uint8 red, Uint8 green, Uint8 blue )
//{
//    //Modulate texture rgb
//
//    SDL_SetTextureColorMod( texture, red, green, blue );
//}
//
//void Texture::setBlendMode( SDL_BlendMode blending )
//{
//    //Set blending function
//    SDL_SetTextureBlendMode( texture, blending );
//}
//
//void Texture::setAlpha( Uint8 alpha )
//{
//    //Modulate texture alpha
//    SDL_SetTextureAlphaMod( texture, alpha );
//}

void Texture::render( Point pos, RECT* clip )
{
    render(pos.x, pos.y, clip);
}

void Texture::render( int x, int y, RECT* clip )
{
    //Set rendering space and render to screen
    RECT renderQuad = MAKERECT( x, y, width, height );

    if (clip->w == 0 || clip->h == 0)
        clip = NULL;

    //Set clip rendering dimensions
    if( clip != NULL)
    {
        renderQuad.w = clip->w;
        renderQuad.h = clip->h;
    }

    //Render to screen
#ifdef USE_GPU
    GPU_BlitRect(texture, clip, renderer, &renderQuad);
#else
    SDL_RenderCopy( renderer, texture, clip, &renderQuad );
#endif
}

void Texture::render(int x, int y, RECT* clip, float scale) {
    //Set rendering space and render to screen
    RECT renderQuad = MAKERECT(x, y, width * scale, height * scale);

    //Set clip rendering dimensions
    if( clip != NULL )
    {
        renderQuad.w = clip->w * scale;
        renderQuad.h = clip->h * scale;
    }

    //Render to screen
#ifdef USE_GPU
    GPU_BlitRect(texture, clip, renderer, &renderQuad);
#else
    SDL_RenderCopy( renderer, texture, clip, &renderQuad );
#endif
}

int Texture::getWidth()
{
    return width;
}

int Texture::getHeight()
{
    return height;
}
