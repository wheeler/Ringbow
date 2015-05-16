/*  OctoWS2811 Rainbow.ino - Rainbow Shifting Test
    http://www.pjrc.com/teensy/td_libs_OctoWS2811.html
    Copyright (c) 2013 Paul Stoffregen, PJRC.COM, LLC

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.


  Required Connections
  --------------------
    pin 2:  LED Strip #1    OctoWS2811 drives 8 LED Strips.
    pin 14: LED strip #2    All 8 are the same length.
    pin 7:  LED strip #3
    pin 8:  LED strip #4    A 100 ohm resistor should used
    pin 6:  LED strip #5    between each Teensy pin and the
    pin 20: LED strip #6    wire to the LED strip, to minimize
    pin 21: LED strip #7    high frequency ringining & noise.
    pin 5:  LED strip #8
    pin 15 & 16 - Connect together, but do not use
    pin 4 - Do not use
    pin 3 - Do not use as PWM.  Normal use is ok.
    pin 1 - Output indicating CPU usage, monitor with an oscilloscope,
            logic analyzer or even an LED (brighter = CPU busier)
*/

#include <OctoWS2811.h>

const int ledsPerStrip = 16;

DMAMEM int displayMemory[ledsPerStrip*6];
int drawingMemory[ledsPerStrip*6];

const int config = WS2811_GRB | WS2811_800kHz;

OctoWS2811 leds(ledsPerStrip, displayMemory, drawingMemory, config);

const int numColors = 160;
int rainbowColors[numColors];
const int colorToRingRatio = numColors / ledsPerStrip;


const int linearCount = 6;
const int colorToLinearRatio = numColors / linearCount;
int linearMap[linearCount][4];

void setup() {
  
  Serial.begin(9600); // USB is always 12 Mbit/sec
  pinMode(1, OUTPUT);
  digitalWrite(1, HIGH);
  
  // SETUP COLORS
  for (int i=0; i<numColors; i++) {
    int hue = i * (360/numColors);
    int saturation = 100;
    int lightness = 2; //50
    // pre-compute the numColors rainbow colors
    rainbowColors[i] = makeColor(hue, saturation, lightness);
  }
  
  // SETUP LINEAR MAP
  for (int i=0; i < linearCount; i++) {
    if (i==0) {
      linearMap[i][0] = 0;
      linearMap[i][1] = 1;
      linearMap[i][2] = 2;
      linearMap[i][3] = 3;
    }
    if (i==1) {
      linearMap[i][0] = 4;
      linearMap[i][1] = 15;
      linearMap[i][2] = -1;
      linearMap[i][3] = -1;
    }
    if (i==2) {
      linearMap[i][0] = 5;
      linearMap[i][1] = 14;
      linearMap[i][2] = -1;
      linearMap[i][3] = -1;
    }
    if (i==3) {
      linearMap[i][0] = 6;
      linearMap[i][1] = 13;
      linearMap[i][2] = -1;
      linearMap[i][3] = -1;
    }
    if (i==4) {
      linearMap[i][0] = 7;
      linearMap[i][1] = 12;
      linearMap[i][2] = -1;
      linearMap[i][3] = -1;
    }
    if (i==5) {
      linearMap[i][0] = 8;
      linearMap[i][1] = 9;
      linearMap[i][2] = 10;
      linearMap[i][3] = 11;
    }
  }
  
  digitalWrite(1, LOW);
  leds.begin();
}


void loop() {
  
  //rainbow(100);
  //skip_rainbow(120, 2, 2);
  linearSweep();
}

void linearSweep() {
  int wait = 5000;
  int px, color;

color=0;
  for (color=0; color < numColors; color++) {
     digitalWrite(1, HIGH);
    for (int x=0; x < linearCount; x++) {
      
      int index = (color + colorToLinearRatio*x) % numColors;
      
      px = linearMap[x][0];
      if (px != -1) {
        leds.setPixel(px, rainbowColors[index]);
//        Serial.print(px); Serial.print(' '); Serial.println(rainbowColors[index]);
      }
      px = linearMap[x][1];
      if (px != -1) {
        leds.setPixel(px, rainbowColors[index]);
//        Serial.print(px); Serial.print(' '); Serial.println(rainbowColors[index]);
      }
      px = linearMap[x][2];
      if (px != -1) {
        leds.setPixel(px, rainbowColors[index]);
//        Serial.print(px); Serial.print(' '); Serial.println(rainbowColors[index]);
      }
      px = linearMap[x][3];
      if (px != -1) {
        leds.setPixel(px, rainbowColors[index]);
//        Serial.print(px); Serial.print(' '); Serial.println(rainbowColors[index]);
      }

    }
    
    //leds.setPixel(0, 0);
    
    leds.show();
    digitalWrite(1, LOW);
    delayMicroseconds(wait);
  }
}

// cycleTime is the number of milliseconds to shift through
// the entire 360 degrees of the color wheel:
// Red -> Orange -> Yellow -> Green -> Blue -> Violet -> Red
//
void rainbow(int cycleTime)
{
  int color, x, wait;

  wait = cycleTime * 1000 / ledsPerStrip;
  for (color=0; color < numColors; color++) {
    digitalWrite(1, HIGH);
    for (x=0; x < ledsPerStrip; x++) {
      int index = (color + colorToRingRatio*x) % numColors;
      leds.setPixel(x, rainbowColors[index]);
    }
    leds.show();
    digitalWrite(1, LOW);
    delayMicroseconds(wait);
  }
}

// cycleTime is the number of milliseconds to shift through
// the entire 360 degrees of the color wheel:
// Red -> Orange -> Yellow -> Green -> Blue -> Violet -> Red
//
void skip_rainbow(int cycleTime, int num_off_pix, int relative_speed)
{
  int color, x, wait;
  int off_pix = -1;
  int off_pix_mod = ledsPerStrip / num_off_pix;
  int mod_remainder = ledsPerStrip % num_off_pix;
  int off_pix_cap = ledsPerStrip - mod_remainder;
  int off_pix_divisor = (colorToRingRatio / relative_speed);

  wait = cycleTime * 1000 / ledsPerStrip;
  
  while (true) {
  for (color=0; color < numColors; color++) {

    if (num_off_pix > 0)
      off_pix = (color / off_pix_divisor) % off_pix_mod;
    
    digitalWrite(1, HIGH);
    for (x=0; x < ledsPerStrip; x++) {
      
      if (x < off_pix_cap && x%off_pix_mod == off_pix) {
        leds.setPixel(x, 0);
      }
      else {
        int index = (color +colorToRingRatio*x) % numColors;
        leds.setPixel(x, rainbowColors[index]);
      }
    }
    leds.show();
    digitalWrite(1, LOW);
    delayMicroseconds(wait);
  }
  }
  
}

