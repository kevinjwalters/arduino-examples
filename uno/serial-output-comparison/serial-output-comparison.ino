/*
  serial-output-comparison v1.0
  Compare behaviour of common code use in setup() to wait for serial to be "ready"

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

// Discussed in https://arduino.stackexchange.com/questions/4556/what-does-the-line-while-serial-do-in-an-arduino-program

unsigned long timeout_serial_ms = 10 * 1000;

void flash(int count, unsigned long delay_ms=500) {
  unsigned long half_delay_ms = delay_ms / 2;
  for (int idx = 0; idx < count; idx++) {
    digitalWrite(LED_BUILTIN, HIGH);
    delay(half_delay_ms);
    digitalWrite(LED_BUILTIN, LOW);
    delay(half_delay_ms);
  }
}


void setup() {
  pinMode(LED_BUILTIN, OUTPUT);

  flash(4);
  
  // From https://docs.arduino.cc/language-reference/en/functions/communication/serial/ifSerial/
  // Indicates if the specified Serial port is ready.
  // On the boards with native USB, if (Serial) (or if(SerialUSB) on the Due)
  // indicates whether or not the USB CDC serial connection is open. For all
  // other boards, and the non-USB CDC ports, this will always return true.
  
  // while (!Serial);  // this is problematic on Leonardo if not connected to USB
  unsigned long start_ms = millis();
  while (!Serial && millis() - start_ms < timeout_serial_ms) {}; 
  
  // From https://docs.arduino.cc/language-reference/en/functions/communication/serial/begin/
  // For USB CDC serial ports (e.g. Serial on the Leonardo), Serial.begin()
  // is irrelevant. You can use any baud rate and configuration for serial
  // communication with these ports. See the list of available serial ports
  // for each board on the Serial main page.
  Serial.begin(115200);

  Serial.println("Hello this is line 1");
  Serial.println("This is line 2");
  flash(2);
}


void loop() {
  Serial.println("This is another line");
  flash(1);
  delay(5 * 1000);
}
