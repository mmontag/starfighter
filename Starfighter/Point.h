//
//  Point.h
//  Starfighter
//
//  Created by Matt Montag on 10/30/16.
//  Copyright © 2016 Patrick Montag. All rights reserved.
//

#ifndef Point_h
#define Point_h

struct Point {
    float x = 0;
    float y = 0;
    Point() {};
    Point(float _x, float _y) : x(_x), y(_y) {};
};

#endif /* Point_h */
