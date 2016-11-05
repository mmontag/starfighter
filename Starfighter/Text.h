//
//  Text.h
//  Starfighter
//
//  Created by Patrick Montag on 10/31/16.
//  Copyright © 2016 Patrick Montag. All rights reserved.
//

#ifndef Text_h
#define Text_h

#include "AssetCache.h"
#include "Point.h"

using namespace std;

class Text {
public:
    Text(Point p, AssetCache* ac, char* t) {
        pos = p;
        texture = ac->getTexture("font.png");
        text = t;
    }
    
    void setText(char* t) {
        text = t;
    }
    
    void render() {
        int origin = 0;
        int i = 0;
        while (text[i] != '\0') {
            // The sprite sheet is composed in ASCII order
            int charCode = (int)text[i];
            int col = charCode % 16;
            int row = charCode / 16;
            SDL_Rect clip = { col * 8, row * 8, 8, 8 };
            texture->render(pos.x + origin, pos.y, &clip);
            origin += 8;
            i++;
        }
    }
    
private:
    char* text;
    Texture* texture;
    Point pos;
};

#endif /* Text_h */
