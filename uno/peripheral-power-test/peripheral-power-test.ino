/*
  Peripheral power test v1.0

  This program sets D8 high.
  The onboard LED is flashed three times to signify the start of the test
  a) RGB pixels are illuminated incrementally on D9 using the WS2812B RGB protocol.
  b) Servo will sweep between 0 and 180 at various speeds controlled using D10.
  
  The actions a) or b) run together but for testing its recommened to
  connect/power only one peripheral at a time.

  Copyright (c) 2024 Kevin J. Walters

  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files (the "Software"), to deal
  in the Software without restriction, including without limitation the rights
  to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
  copies of the Software, and to permit persons to whom the Software is
  furnished to do so, subject to the following conditions:

  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
  IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
  FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
  AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
  LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
  OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
  SOFTWARE.
*/


#include <Servo.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
  #include <avr/power.h>
#endif

#define HIGH_PIN 8
#define RGBPIXELS_PIN 9
#define SERVO_PIN 10

// If NUM_PIXELS is changed then review SERVO_SPEED_LOOPS
#define NUM_PIXELS 12
#define SERVO_SPEED_LOOPS 4  // Double speed every N loops

#define SERVO_MIN 0
#define SERVO_MAX 180

Adafruit_NeoPixel pixels(NUM_PIXELS, RGBPIXELS_PIN, NEO_GRB + NEO_KHZ800);
Servo myservo;

// Default servo range is 544 to 2400
// https://www.arduino.cc/reference/en/libraries/servo/attach/

uint32_t pixel_black, pixel_white;  // initialised in setup()


void pixels_off(void) {
  for (int idx = 0; idx < NUM_PIXELS; idx++) {
    pixels.setPixelColor(idx, pixel_black);
  }  
  pixels.show();
}


// the setup function runs once when you press reset or power the board
void setup() {
  Serial.begin(115200);
  while (! Serial);  // wait for it

  pixel_black = pixels.Color(0, 0, 0);
  pixel_white = pixels.Color(255, 255, 255);

  pinMode(LED_BUILTIN, OUTPUT);

  pinMode(HIGH_PIN, OUTPUT);
  pixels.begin();
  pixels_off();
  if (false) {
    // This is to allow the current for a single pixel to be measured
    // Unbranded ring of 12 50mm ones, 44mA at 5V, 36mA at 3.3V
    // all twelve at 5V 403mA
    pixels.setPixelColor(0, pixel_white);
    pixels.show();
    delay(3000);
    pixels.setPixelColor(0, pixel_black);
    pixels.show();
    delay(60 * 1000);
  }
  pinMode(SERVO_PIN, OUTPUT);
  // servo attach is called in loop()
}


void loop() {
  // Flash onboard LED three times to signify start
  for (int idx = 0; idx < 3; idx++) {
    digitalWrite(LED_BUILTIN, HIGH);  // onboard LED
    delay(1000);  // 1000ms = 1s
    digitalWrite(LED_BUILTIN, LOW);
    delay(1000); 
  }

  // Now start the simultaneous pixel lighting and servo movement
  myservo.attach(SERVO_PIN);
  int start_pos = SERVO_MIN, end_pos = SERVO_MAX;
  int old_start_pos = end_pos;
  int degree_step = 1;
  int reps = 1;
  int duration_s = 2;
  for (int idx = 0; idx < NUM_PIXELS; idx++) {
    pixels.setPixelColor(idx, pixel_white);
    pixels.show();

    unsigned long step_pause_ms = degree_step * duration_s * 1000 / reps / 180;
    unsigned int step_pause_us = 0;
    if (step_pause_ms < 16) {
      step_pause_us = (unsigned long)degree_step * duration_s * 1000 * 1000 / reps / 180;
      Serial.println(step_pause_ms);
      Serial.println(step_pause_us);
    }
    for (int swing = 0; swing < reps; swing++) {      
      int pos = start_pos;
      while (pos != end_pos) {
        //int pos = map(step, 0, steps - 1, start_pos, end_pos);
        myservo.write(pos);
        if (step_pause_us != 0) {
          delayMicroseconds(step_pause_us);
        } {
          delay(step_pause_ms);
        }
        // Move the servo position but keep within limits
        if (end_pos > start_pos) {
          pos += degree_step;
          if (pos > SERVO_MAX) {
            pos = SERVO_MAX;
          }
        } else {
          pos -= degree_step;
          if (pos < SERVO_MIN) {
            pos = SERVO_MIN;
          }
        }
      }
      
      // Swap start and end servo positions
      old_start_pos = start_pos;
      start_pos = end_pos;
      end_pos = old_start_pos;
    }

    // For a ring of 12 every 4 pixels double the number of servo movements
    if (idx % SERVO_SPEED_LOOPS == SERVO_SPEED_LOOPS - 1) {
      reps *= 2;
    }
  }
  pixels_off();
  myservo.detach();
  pinMode(SERVO_PIN, OUTPUT);

  delay(10 * 1000);
}
