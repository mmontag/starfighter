//
//  Timer.h
//  Starfighter
//
//  Created by Patrick Montag on 10/21/16.
//  Copyright © 2016 Patrick Montag. All rights reserved.
//

#ifndef Timer_h
#define Timer_h

//The application time based timer
class Timer
{
public:
    //Initializes variables
    Timer();
    
    //The various clock actions
    void Start();
    void Stop();
    void Pause();
    void Resume();
    void Clear();
    
    //Gets the timer's time
    Uint32 getTicks();
    
    //Checks the status of the timer
    bool isStarted();
    bool isPaused();
    
private:
    //The clock time when the timer started
    Uint32 mStartTicks;
    
    //The ticks stored when the timer was paused
    Uint32 mPausedTicks;
    
    //The timer status
    bool mPaused;
    bool mStarted;
};

#endif /* Timer_h */
