/*
  ana-dig-reader v1.0
  Reads analogue and digital values in response to serial commands over D9/D8

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


#include<SoftwareSerial.h>

const int ANALOGUE_PIN = A5;
const int DIGITAL_PIN = 5;
const int RX_D8 = 8;
const int TX_D9 = 9;

SoftwareSerial SwSerial(RX_D8, TX_D9);
char tx_buffer[81] = { '\0' };


void setup() {
  Serial.begin(115200);
  SwSerial.begin(115200);
#if defined(ARDUINO_UNOR4_MINIMA) || defined(ARDUINO_UNOR4_WIFI)
  analogReadResolution(14);  // R4 has 14bit ADC
#endif
}


void loop() {
  if (SwSerial.available() > 0) {
    int rx_char = SwSerial.read();
    if (rx_char == 'R') {
      int input_ana = analogRead(ANALOGUE_PIN);
      unsigned int input_dig = digitalRead(DIGITAL_PIN) == HIGH ? 1 : 0;
      (void)snprintf(tx_buffer, sizeof(tx_buffer), "%05d,%u", input_ana, input_dig);
      SwSerial.println(tx_buffer);
    }
  }
}
