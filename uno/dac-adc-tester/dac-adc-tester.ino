/*
  dac-adc-tester v1.1
  Generate waveforms on DAC and send serial commands to remote Arduino to measure them
 
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


const int DAC_A0 = A0;
const int TRI_WAVE_MIN = 0;
const int TRI_WAVE_MAX = 4095;
const int SQUARE_LOW_COUNT = 50;
const int SQUARE_HIGH_COUNT = 50;

char rx_buffer[81] = { '\0' };


void setup() {
  Serial.begin(115200);
  Serial1.begin(38400);  // this is TX (D0) & RX (D1)
  
  analogWriteResolution(12);  // R4 DAC is 12bit
  // pinMode() is for the digital only input/output, setting may interfere with DAC
  // See https://forum.arduino.cc/t/arduino-zero-dac-analog-output-cutoff-below-the-rail-voltage/505268/4
  // pinMode(DAC_A0, OUTPUT)  // DO NOT DO THIS FOR DAC
  
  while (Serial1.available() > 0) {
    (void)Serial1.read();  // discard any buffered data
  }
}


void loop() {
  delay(10'000);

  // Output triangle wave
  // and request values over serial from second Arduino board
  for (int cycle=0; cycle < 10; cycle++) {
    bool triangle = (cycle < 5);
    int count = 0;
    int output = triangle ? TRI_WAVE_MIN : TRI_WAVE_MAX;
    int direction = 1;
    
    while (true) {
      analogWrite(DAC_A0, output);
      Serial1.write("R");
      if (Serial1.available() > 0) {
        // Pad with leading 0s for constant width
        size_t idx = snprintf(rx_buffer, sizeof(rx_buffer), "%010lu,", micros());
        while (Serial1.available() > 0) {
          int rx_char = Serial1.read();
          if (rx_char < 0 || rx_char == '\n') {
            rx_buffer[idx] = '\0';
          } else {
            rx_buffer[idx] = rx_char;
          }
          ++idx;
          if (idx >= sizeof(rx_buffer) - 1) {
            // At the last character in array, leave this undisturbed as a NUL
            // and conclude reading
            break;
          }
        }
        Serial.println(rx_buffer);
      }
      ++count;

      // Change the output variable for triangle or square waveform
      if (triangle) {
        output += direction;
        if (output > TRI_WAVE_MAX) {
          direction = -1;
          output = TRI_WAVE_MAX + direction;
        } else if (output < TRI_WAVE_MIN) {
          break;
        }
      } else {
        if (count == SQUARE_HIGH_COUNT) {
          output = TRI_WAVE_MIN;
        } else if (count >= SQUARE_LOW_COUNT + SQUARE_HIGH_COUNT) {
          break;
        }
      }
    }
  }

  // Ensure DAC output is back at 0V
  analogWrite(DAC_A0, 0);
}
