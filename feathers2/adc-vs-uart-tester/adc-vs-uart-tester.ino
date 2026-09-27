/*
  adc-vs-uart-tester v1.0
  Reads an ADC value while sending data over UART to look for noisy clash

  Copyright (c) 2026 Kevin J. Walters

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

// Investigating https://github.com/espressif/arduino-esp32/issues/12944


// #ifndef ARDUINO_ARCH_ESP32
// #include <SoftwareSerial.h>
// #endif

#if ARDUINO_FEATHERS2
  // Feather A0 to A5 are 17, 18, 14, 12, 6, 5
  const int ANALOGUE_PIN = 5;  // do NOT use A5, wrong pin!
  //const int DIGITAL_PIN = 5;
  const int RX_PIN = RX;
  const int TX_PIN = TX;
  //const int RX_PIN = 38;
  //const int TX_PIN = 33;
#endif


const static char *SOFTWARE_NAME = "adc-vs-uart-tester";
const static char *SOFTWARE_VERSION = "1.0";

const unsigned long TIMEOUT_CONSOLE_MS = 2000;

#define SgButSerial Serial1
#define SERIAL_T auto

char tx_buffer[81] = { '\0' };


#if defined(ARDUINO_FEATHERS2)
#define ADC_RESOLUTION 13
const float ADC_VREF = 3.3f;
#endif



const char *hist_chars = " .:xX";
const int histogram_max = 200;
uint32_t histogram[histogram_max + 1];
// +1 to deal with 0-N needing N+1 elements, +1 for a pipe character, +1 for NUL
char histogram_ascii[histogram_max + 1 + 1 + 1];

void print_histogram(const int buf[], size_t buf_len) {
  // reset values to 0
  for (size_t bucket=0; bucket <= histogram_max; bucket++) {
    histogram[bucket] = 0;
  }

  // count any values within the range
  for (size_t idx=0; idx < buf_len; idx++) {
    int bucket = buf[idx];
    if (bucket >= 0 && bucket <= histogram_max) {
      histogram[bucket]++;
    }
  }

  uint32_t max_hidx = strlen(hist_chars) - 1;
  size_t bucket=0;
  while (bucket <= histogram_max) {
    histogram_ascii[bucket] = hist_chars[min(histogram[bucket], max_hidx)];
    bucket++;
  }
  histogram_ascii[bucket++] = '|';
  histogram_ascii[bucket] = '\0';
  Serial.println(histogram_ascii);
}


const size_t sample_count = 5 * 1000;
int adc_samples[sample_count];
bool alt_serial_tx = false;


void setup() {
  unsigned long start_ms = millis();
  Serial.begin(115200);
  while (!Serial && millis() - start_ms < TIMEOUT_CONSOLE_MS) {};
  // On ESP32 for the case where the code has just been uploaded
  // Serial will not be ready - pressing reset button will work
  // See https://forum.arduino.cc/t/esp32-delay-required-after-serial-begin/1399389

  Serial.println("Reproducing weirdness for https://github.com/espressif/arduino-esp32/issues/12944");

  Serial.println("Initialising UART");
  SgButSerial.begin(38400, SERIAL_8N1, RX_PIN, TX_PIN);
  Serial.println("Initialised");

  analogReadResolution(ADC_RESOLUTION);
}

void loop() {
  snprintf(tx_buffer, sizeof(tx_buffer), "Noise");

  // Read 1000 ADC samples and show distribution using ASCII
  
  for (int delay=0; delay < 2; delay++) {
    bool extra_delay = delay > 0;
    for (int rep=0; rep < 5; rep++) {
      Serial.print(alt_serial_tx ? 'U' : '_');  // Indicate whether UART is in use
      Serial.print(extra_delay ? 'd' : '_');    // Indicate whether tiny variable delay in use
      for (size_t idx=0; idx < sample_count; idx++) {
        //(void)SgButSerial.available();  // could this be the cause? No effect
        adc_samples[idx] = analogReadMilliVolts(ANALOGUE_PIN);
        // delayMicroseconds(50);  No effect
      
        if (alt_serial_tx) {
          // This will be very quick until the TX buffer fills up
          snprintf(tx_buffer, sizeof(tx_buffer), "%05d ", adc_samples[idx]);
          SgButSerial.print(tx_buffer);
        } else {
          // 6 chars is about 1.5ms, equivalent delay when serial isn't used
          delayMicroseconds(1000 * 1000 * (1 + 8 + 1) * (5 + 1) / 38400);
        }
        if (extra_delay) {
          if (idx % 40 == 0) {
            delayMicroseconds(idx);  // experiment with some extra delays
          }
        }
      }
      print_histogram(adc_samples, sample_count);
    }
  }

  alt_serial_tx = !alt_serial_tx;  // Toggle the UART sends for next loop
  Serial.println();
  delay(1000);
}
