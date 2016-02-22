#include "RTCMatrix.h"

/*
   Name   : Jeffrey Goldsmith
   Teacher: Mr. D'Arcy
   Date   : 20 January 2016
   Course : TEI3M

   Description: LED RGB serial in binary clock matrix
*/

static const byte DATA_IN_PIN = 6;          // Data in pin for RGB matrix

Time Rtc; // Time constructor
Display Clock(DATA_IN_PIN); // Display constructor

void setup()
{
  Rtc.Sync(); // Sync system time and RTC time
}

void loop()
{
  Rtc.UpdateTime(); // Take reading from RTC and update unix time value
  Clock.DisplayTime(); // Display time for second, minute, hour, etc.
}

