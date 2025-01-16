/*
  serial-periodic-print v1.0
  Print two micros() timestamps every 10 seconds

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

// This is intended for a very basic comparison of the clocks on various Arduino boards

const unsigned long period_us = 10 * 1000L * 1000L;

unsigned long last_print_us = 0;

void setup() {
  while (!Serial) {};
  Serial.begin(250 * 1000L);
}


void loop() {
  unsigned long timestamp_us;
  while (true) {
    timestamp_us = micros();
    if (timestamp_us - last_print_us >= period_us) {
      Serial.print(timestamp_us);
      Serial.print(",");
      Serial.println(micros());
      // Don't assign timestamp_us here in case it has slipped a little
      last_print_us = last_print_us + period_us;
    }
  }
}
