#include <Adafruit_NeoPixel.h>

// change these values to match your board.
#define BUTTON_LEAD 12
#define LED_LEAD 6
#define NUMBER_OF_PIXELS 56
#define NUMBER_OF_MODES 2
#define NUM_LEDS 23

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUMBER_OF_PIXELS, LED_LEAD, NEO_GRB + NEO_KHZ800);

bool buttonState = LOW;
bool lastButtonState = LOW;

// The following constant is the delay between each pixel flash. It is used in all three modes, therefore, all modes
// flash at the same rate.
// If you need different flash rates, then declare different constants and use those in each mode function. Or let me know and
// I'll do it for you.
const int PIXEL_FLASH_INTERVAL = 25;

unsigned long previousMillis = millis();

int mode = 1; // Default mode is one.

byte colors[3][3] = { {0xff, 0,0xff}, //red? maybe purple?
                        {0, 0xff, 0}, //white? set to green
                        {0   , 0   , 0xff} }; //blue?

void setup() {
  pinMode(BUTTON_LEAD, INPUT_PULLUP);
  strip.begin();
  strip.show();
}

int pos = 0, dir = 1; // Position, direction of "eye"

void loop() {
  
  switch (mode) {
    case 1:
      rainbow();
      break;

    case 2:
    BouncingColoredBalls(3, colors);
      //colorWipe(strip.Color(0, 255, 0));
      break;

      case 3:
      scanner();
      break;

    default:
      mode = 1;
      break;
  }
}

/* colorWipe function 
void colorWipe(uint32_t color) {

  uint32_t i = 0;
  while (i < strip.numPixels()) {
    if (millis() - previousMillis >= PIXEL_FLASH_INTERVAL) {
      previousMillis = millis();
      i++;
    }
    strip.setPixelColor(i, color);
    strip.show();

    if (buttonListener()) {
      break;
    }
  }
}*/



/* rainbow function */
void rainbow() {
  uint16_t j = 0;
  while(j<256*3) {
    if(millis() - previousMillis >= PIXEL_FLASH_INTERVAL) {
      previousMillis = millis();
      
      uint16_t i = 0; 
      while(i < strip.numPixels()) {
        strip.setPixelColor(i, Wheel(((i * 256 / strip.numPixels()) + j) & 255));
        i++;
        if(buttonListener()) { return; }
        }
      strip.show();
      j++;
    }
    
  }
}

/* Wheel function */
uint32_t Wheel(byte WheelPos) {
  WheelPos = 255 - WheelPos;
  if(WheelPos < 85) {
    return strip.Color(255 - WheelPos * 3, 0, WheelPos * 3);
  }
  if(WheelPos < 170) {
    WheelPos -= 85;
    return strip.Color(0, WheelPos * 3, 255 - WheelPos * 3);
  }
  WheelPos -= 170;
  return strip.Color(WheelPos * 3, 255 - WheelPos * 3, 0);
}

/* monitor button press */
bool buttonListener() {
  bool modeChanged = false;

  buttonState = digitalRead(BUTTON_LEAD);

  if (buttonState != lastButtonState) {
    if (buttonState == LOW) {
      mode++;
      modeChanged = true;
      delay(250);             // Debounce delay. I checked the best parameter and 250 was it.
    }
  }
  lastButtonState = buttonState;

  return modeChanged;
}

void scanner() {
  if (millis() - previousMillis >= PIXEL_FLASH_INTERVAL) {
      previousMillis = millis();

    if(buttonListener()) { return; }
      
  int j;

  // Draw 5 pixels centered on pos.  setPixelColor() will clip any
  // pixels off the ends of the strip, we don't need to watch for that.
  strip.setPixelColor(pos - 2, 0x100010); // Dark red
  strip.setPixelColor(pos - 1, 0x800080); // Medium red
  strip.setPixelColor(pos    , 0xff00ff); // Center pixel is brightest
  strip.setPixelColor(pos + 1, 0x800080); // Medium red
  strip.setPixelColor(pos + 2, 0x100010); // Dark red

  strip.show();
  //delay(60);

  


  // Rather than being sneaky and erasing just the tail pixel,
  // it's easier to erase it all and draw a new one next time.
  for(j=-2; j<= 2; j++) strip.setPixelColor(pos+j, 0);

  // Bounce off ends of strip
  pos += dir;
  if(pos < 0) {
    pos = 1;
    dir = -dir;
  } else if(pos >= strip.numPixels()) {
    pos = strip.numPixels() - 2;
    dir = -dir;
  }
}
}


void BouncingColoredBalls(int BallCount, byte colors[][3]) {
  float Gravity = -9.81;
  int StartHeight = 1;
  
  float Height[BallCount];
  float ImpactVelocityStart = sqrt( -2 * Gravity * StartHeight );
  float ImpactVelocity[BallCount];
  float TimeSinceLastBounce[BallCount];
  int   Position[BallCount];
  long  ClockTimeSinceLastBounce[BallCount];
  float Dampening[BallCount];
  
  for (int i = 0 ; i < BallCount ; i++) {   
    ClockTimeSinceLastBounce[i] = millis();
    Height[i] = StartHeight;
    Position[i] = 0; 
    ImpactVelocity[i] = ImpactVelocityStart;
    TimeSinceLastBounce[i] = 0;
    Dampening[i] = 0.90 - float(i)/pow(BallCount,2); 
  }

  while (true) {
    for (int i = 0 ; i < BallCount ; i++) {
      TimeSinceLastBounce[i] =  millis() - ClockTimeSinceLastBounce[i];
      Height[i] = 0.5 * Gravity * pow( TimeSinceLastBounce[i]/1000 , 2.0 ) + ImpactVelocity[i] * TimeSinceLastBounce[i]/1000;
  
      if ( Height[i] < 0 ) {                      
        Height[i] = 0;
        ImpactVelocity[i] = Dampening[i] * ImpactVelocity[i];
        ClockTimeSinceLastBounce[i] = millis();
  
        if ( ImpactVelocity[i] < 0.01 ) {
          ImpactVelocity[i] = ImpactVelocityStart;
        }
      }
      Position[i] = round( Height[i] * (NUM_LEDS - 1) / StartHeight);
    }
  
    for (int i = 0 ; i < BallCount ; i++) {
      setPixel(Position[i],colors[i][0],colors[i][1],colors[i][2]);
    }

    if(buttonListener()) { return; }

    showStrip();
    setAll(0,0,0);
  }
}


void showStrip() {
 #ifdef ADAFRUIT_NEOPIXEL_H 
   // NeoPixel
   strip.show();
 #endif
 #ifndef ADAFRUIT_NEOPIXEL_H
   // FastLED
   FastLED.show();
 #endif
}

void setPixel(int Pixel, byte red, byte green, byte blue) {
 #ifdef ADAFRUIT_NEOPIXEL_H 
   // NeoPixel
   strip.setPixelColor(Pixel, strip.Color(red, green, blue));
 #endif
 #ifndef ADAFRUIT_NEOPIXEL_H 
   // FastLED
   leds[Pixel].r = red;
   leds[Pixel].g = green;
   leds[Pixel].b = blue;
 #endif
}

void setAll(byte red, byte green, byte blue) {
  for(int i = 0; i < NUM_LEDS; i++ ) {
    setPixel(i, red, green, blue); 
  }
  showStrip();
}
