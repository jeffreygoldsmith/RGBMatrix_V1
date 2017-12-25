//
// Include libraries.
//
#include "Arduino.h"
#include "RTCMatrix.h"
#include <Time.h>
#include <Adafruit_NeoPixel.h>
#include <Wire.h>
#include "RTClib.h"


date tm;

static const byte DIN_PIN = 6;             // Data in pin
static const byte LED_NUM = 64;            // Number of LEDs on RGB matrix
static const byte ROW_OFFSET = 8;          // Amount of LEDs per row on matrix
static const byte ROW_NUM = 7;             // Number of rows used on matrix
static const byte APM_ROW = 5;             // Row to display whether it is AM or PM
static const byte APM_HOUR = 12;           // Amount of hours before AM-PM transition
static const byte COLOUR = 30;             // Amount of red, green, or blue to display on matrix

static const byte row[] = { 8, 7, 6, 4, 3, 2, 1 };         // Row number of time measurements
static const byte bitLength[] = { 6, 6, 5, 3, 5, 4, 8 };   // Bit length of time measurements

unsigned long unixPrev;     // Lagging value of unix time
byte hourPrev;              // Lagging value of hours
bool isDay = true;
bool isNight = true;

Adafruit_NeoPixel matrix = Adafruit_NeoPixel(LED_NUM, DIN_PIN, NEO_GRB); // Create new NeoPixel object
RTC_DS1307 rtc; // Create new RTC object


//
// Function to take time and display value on matrix.
//
void bitTime(int t, byte tLength, byte row)
{
  for (int i = 0; i < tLength; i++)
  {
    bool bitBool = bitRead(t, i); // Check each bit in t to be high or low
    matrix.setPixelColor(ROW_OFFSET * row - 1 - i, 30 * bitBool, 0, 0); // If bit is high set LED to be high, else set low
  }
  matrix.show();
}


//
// Function to take time_t variable and decode into individual measurements of time.
//
date Decode(time_t ts)
{
  int d, m, y;
  date tm;

  tm.s = ts % 60; // Seconds
  ts /= 60;

  tm.m = ts % 60; // Minutes
  ts /= 60;

  tm.h = ts % 24; // Hours
  ts /= 24;

  ts = ts * 100 + 36525 * 2 - 5950; // Move start of epoch to 1 March 1968 and scale
  y = (ts + 75) / 36525; // Years since start of epoch
  d = (175 + ts - y * 36525) / 100 * 10; // Days since start of year (times 10)
  m = (d - 6) / 306; // Months since start of year

  tm.y = 1968 + y + (m >= 10); // Year
  tm.mon = 1 + (m + 2) % 12; // Month
  tm.d = (4 + d - m * 306) / 10; // Day
  tm.wd = 1 + (5 + ts / 100) % 7; // Day of week (Sunday = 1)

  return tm;
}


//
// Class to control time and RTC.
//

//
// Time::Time() -- Class constructor
//
Time::Time()
{
  rtc.begin(); // Initialize RTC
}


//
// Function to initialize RTC.
//
void Time::Sync()
{
  DateTime now = rtc.now(); // Take reading from RTC and update current time
  unixPrev = now.unixtime(); // Set lagging value of unix time to be initial unix time
}


//
// Time::UpdateTime() -- Update time based on readings from RTC.
//
void Time::UpdateTime()
{
  DateTime now = rtc.now(); // Take reading from RTC and update current time

  if (now.unixtime() - unixPrev == 1) // Check for second transition
  {
    tm = Decode(now.unixtime()); // Compute and decode current time
  }
  unixPrev = now.unixtime(); // Set lagging value of unix time
}


//
// Class to control RGB matrix.
//


//
// Display::Display() -- Class constructor
//
Display::Display(byte dataIn)
{
  pinMode(dataIn, OUTPUT); // Set data in pin to output
  matrix.begin(); // Initialize matrix
  matrix.setBrightness(25); // Set initial brightness
  matrix.show(); // Set all LEDs to off initially and update matrix
}


//
// Display::DisplayTime() -- Display time on matrix.
//
void Display::DisplayTime()
{
  bitTime(tm.y % 100, bitLength[6], row[6]); // Display time on matrix
  bitTime(tm.mon, bitLength[5], row[5]);
  bitTime(tm.d, bitLength[4], row[4]);
  bitTime(tm.wd, bitLength[3], row[3]);
  tm.h % 12 == 0 ? bitTime(12, bitLength[2], row[2]) : bitTime(tm.h % 12, bitLength[2], row[2]);
  bitTime(tm.m, bitLength[1], row[1]);
  bitTime(tm.s, bitLength[0], row[0]);

  if (tm.h < 12) // Check for AM
  {
    for (byte i = 0; i < ROW_OFFSET; i++) // Set APM row to Green
      matrix.setPixelColor(ROW_OFFSET * APM_ROW - i - 1, 0, COLOUR, 0);
  }
  else
  { // If not AM assume PM
    for (byte i = 0; i < ROW_OFFSET; i++) // Set APM row to Blue
      matrix.setPixelColor(ROW_OFFSET * APM_ROW - i - 1, 0, 0, COLOUR);
  }

  if (tm.h >= 8 && tm.h <= 20) // Check if time is before 8AM or after 8PM
  {
    if (isDay == true)
    {
      matrix.setBrightness(30); // Set brightness to day mode
      isDay = false;
      isNight = true;
    }
  }
  else
  { // If not assume time is between 8AM and 8PM
    if (isNight == true)
    {
      matrix.setBrightness(10); // Set brightness to night mode
      isDay = true;
      isNight = false;
    }
  }
  matrix.show(); // Update matrix

}
