/*
  gpio-input-tester v1.0
  Show d8 (pulled up) and d5 input values on the onboard LED (D13)

  Copyright (c) 2025 Kevin J. Walters

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


const int DIGITAL_PIN_D5 = 5;
const int DIGITAL_PULLEDUP_PIN_D8 = 8;


void setup() {
  pinMode(DIGITAL_PULLEDUP_PIN_D8, INPUT_PULLUP);
  pinMode(LED_BUILTIN, OUTPUT);
}


void loop() {
  // Read D5 and D8 value and turn on LED if D5 is HIGH or D8 is low
  bool d5_in = digitalRead(DIGITAL_PIN_D5) == HIGH;
  bool d8_in = digitalRead(DIGITAL_PULLEDUP_PIN_D8) == HIGH;
  digitalWrite(LED_BUILTIN, d5_in | !d8_in);
}
