#ifndef RTCMATRIX_H
#define RTCMATRIX_H

#include "Arduino.h"


struct tm
{
  int y;    // Year
  int mon;  // Month
  int d;    // Day
  int wd;   // Weekday
  int h;    // Hour
  int m;    // Minute
  int s;    // Second
};

class Time
{
  public:
    Time();
    void Sync();
    void UpdateTime();
};

class Display  
{
  public:
    Display(byte dataIn);
    void DisplayTime();  
};


#endif // RTCMATRIX_H
