/*
  ana-dig-reader v1.3
  Reads analogue and digital values in response to serial commands

  Copyright (c) 2024, 2026 Kevin J. Walters

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


// #define USE_ADC_RAW "unruly"  // for analogRead on all ESP32 family


#ifndef ARDUINO_ARCH_ESP32
#include <SoftwareSerial.h>
#endif

#ifdef ARDUINO_ESP32_DEV  // for ESP32 DevKitC clone
  const int ANALOGUE_PIN = 36;  // GPIO pin next to EN
  const int DIGITAL_PIN = 36;
  const int RX_PIN = 26;
  const int TX_PIN = 27;
#elif ARDUINO_FEATHERS2
  // Feather A0 to A5 are 17, 18, 14, 12, 6, 5
  const int ANALOGUE_PIN = 5;  // do NOT use A5, wrong pin!
  const int DIGITAL_PIN = 5;
  const int RX_PIN = RX;
  const int TX_PIN = TX;
#elif ARDUINO_ARCH_ESP32   // for Xiao ESP32-xN boards
  const int ANALOGUE_PIN = A0;
  const int DIGITAL_PIN = A0;
  const int RX_PIN = D7;
  const int TX_PIN = D6;
#elif ARDUINO_TEENSY41  /// For Teensy 4.1
  const int ANALOGUE_PIN = 19;  // A5
  const int DIGITAL_PIN = 19;
  const int RX_PIN = 15;        // RX3
  const int TX_PIN = 14;        // TX3
#else
  const int ANALOGUE_PIN = A5;
  const int DIGITAL_PIN = 5;
  const int RX_PIN = 8;
  const int TX_PIN = 9;
#endif


const static char *SOFTWARE_NAME = "ana-dig-reader";
const static char *SOFTWARE_VERSION = "1.3";

const char ANADIG_CMD = 'R';
const char V_ANA_CMD = 'C';
const char V_DIG_CMD = 'F';
const char INFO_CMD = 'I';

// Serial1 only viable one from https://forum.seeedstudio.com/t/xiao-esp32c6-uarts/292856
#ifdef ARDUINO_ESP32_DEV  // for ESP32 DevKitC clone
  #define SgButSerial Serial2
  // #define SERIAL_T (typeid(Serial2)) // HardwareSerial
  #define SERIAL_T auto
#elif ARDUINO_ARCH_ESP32  // for Xiao ESP32-xN boards
  #define SgButSerial Serial1
  // #define SERIAL_T (typeid(Serial1)) // HardwareSerial
  #define SERIAL_T auto
#elif ARDUINO_TEENSY41  // for Teensy 4.1
  #define SgButSerial Serial3   // TX 14, RX 15
  #define SERIAL_T auto
  // #define SERIAL_T (typeid(Serial3))  // HardwareSerial
#else
  SoftwareSerial SgButSerial(RX_PIN, TX_PIN);
  #define SERIAL_T SoftwareSerial   // can't use auto for C++14 on R3 :(
#endif

char tx_buffer[81] = { '\0' };
int input_analogue[255];

const unsigned long TIMEOUT_CONSOLE_MS = 2000;

#define ADC_RAW 101
#define ADC_CALIBRATEDMV 102

#if defined(ARDUINO_UNOR4_MINIMA) || defined(ARDUINO_UNOR4_WIFI)
  // Renesas RA4M1 on R4 has 14bit ADC
  #define ADC_READ ADC_RAW
  #define ADC_RESOLUTION  14
  const float ADC_VREF = 5.0f;
#elif defined(ARDUINO_ARCH_ESP32)
  // ESP32s seem to have 12bit ADC
  // apart from 13bit ESP32-S2 and ESP32-S3 which 
  // will be ignored for now
  #if defined(USE_ADC_RAW)
    #define ADC_READ ADC_RAW
    // These aren't visible in Arduino IDE
    #pragma message("*** Using unruly analogRead() on ESP32 family ***")
  #else
    #define ADC_READ ADC_CALIBRATEDMV
  #endif
  #define ADC_RESOLUTION 12
  const float ADC_VREF = 3.3f;
#elif defined(TEENSYDUINO)
  // IMXRT1060 series
  #define ADC_READ ADC_RAW
  #define ADC_RESOLUTION  12
  const float ADC_VREF = 3.3f;
#else
  // The classic, solid AVR 10bit ADC
  #define ADC_READ ADC_RAW
  #define ADC_RESOLUTION 10
  const float ADC_VREF = 5.0f;
#endif

// Board name from Arduino IDE shown in comments
#if defined(ARDUINO_UNOR4_MINIMA) 
  const static char *BOARD_MANU = "Arduino";
  const static char *BOARD_NAME = "UNO R4 Minima";
  const static char *BOARD_MCU  = "Renesas RA4M1";

#elif defined(ARDUINO_UNOR4_WIFI)  // Arduino UNO R4 WiFi
  const static char *BOARD_MANU = "Arduino";
  const static char *BOARD_NAME = "UNO R4 WiFi";
  const static char *BOARD_MCU  = "Renesas RA4M1";

#elif defined(ARDUINO_AVR_UNO)
  const static char *BOARD_MANU = "Arduino";
  const static char *BOARD_NAME = "UNO R3";
  const static char *BOARD_MCU  = "ATmega328P";

#elif defined(ARDUINO_AVR_LEONARDO)
  const static char *BOARD_MANU = "Arduino";
  const static char *BOARD_NAME = "UNO Leonardo";
  const static char *BOARD_MCU  = "ATmega32u4";

#elif defined(ARDUINO_XIAO_ESP32C5)
  const static char *BOARD_MANU = "Seeed Studio";
  const static char *BOARD_NAME = "ESP32C5";
  const static char *BOARD_MCU  = "ESP32-C5";

#elif defined(ARDUINO_XIAO_ESP32C6)  // XIAO_ESP32C6
  const static char *BOARD_MANU = "Seeed Studio";
  const static char *BOARD_NAME = "ESP32C6";
  const static char *BOARD_MCU  = "ESP32-C6";

#elif defined(ARDUINO_ESP32_DEV)     // ESP32 Dev Module
  const static char *BOARD_MANU = "Espressif";
  const static char *BOARD_NAME = "ESP32-DevKitC";
  const static char *BOARD_MCU  = "ESP32";

#elif defined(ARDUINO_FEATHERS2) // UM FeatherS2
  const static char *BOARD_MANU = "Unexpected Maker";
  const static char *BOARD_NAME = "FeatherS2";
  const static char *BOARD_MCU  = "ESP32-S2";

#elif defined(ARDUINO_TEENSY41) // Teensy 4.1
  const static char *BOARD_MANU = "PJRC";
  const static char *BOARD_NAME = "Teensy 4.1";
  const static char *BOARD_MCU  = "IMXRT1062";

#else

  #error "Unsupported board..."
#endif

#if defined(ARDUINO_XIAO_ESP32C5) || defined(ARDUINO_XIAO_ESP32C6)
// Active low boards
#define LED_ON  LOW
#define LED_OFF HIGH
#else
#define LED_ON  HIGH
#define LED_OFF LOW
#endif


void flash(int count) {
#ifdef LED_BUILTIN
  for (int i=0; i < count; i++) {
    digitalWrite(LED_BUILTIN, LED_ON);  
    delay(300);
    digitalWrite(LED_BUILTIN, LED_OFF);
    delay(300);
  }
#endif
}


int readWithTimeout(SERIAL_T &sgbutser, unsigned long timeout_us) {
  unsigned long start_us = micros();
  int rx_char = -1;
  do {
    if (sgbutser.available() > 0) {
      rx_char = sgbutser.read();
      break;
    }
  } while (micros() - start_us <= timeout_us);
  return rx_char;
}


void setup() {
  unsigned long start_ms = millis();
  Serial.begin(115200);
  while (!Serial && millis() - start_ms < TIMEOUT_CONSOLE_MS) {};
  // On ESP32 for the case where the code has just been uploaded
  // Serial will not be ready - pressing reset button will work
  // See https://forum.arduino.cc/t/esp32-delay-required-after-serial-begin/1399389

  Serial.println("Initialising SG to BUT serial");
#ifdef ARDUINO_ARCH_ESP32
  SgButSerial.begin(38400, SERIAL_8N1, RX_PIN, TX_PIN);
#else
  SgButSerial.begin(38400);
#endif
  Serial.println("Initialised");
#if ADC_RESOLUTION != 10
  analogReadResolution(ADC_RESOLUTION);
#endif

  // initialize digital pin LED_BUILTIN as an output.
#ifdef LED_BUILTIN
  pinMode(LED_BUILTIN, OUTPUT);
#endif
  
  // Any board specific code
  
  flash(2);
}


void loop() {
  if (SgButSerial.available() > 0) {
    int rx_char = SgButSerial.read();
    if (rx_char == ANADIG_CMD) {
      int input_ana = analogRead(ANALOGUE_PIN);
      unsigned int input_dig = digitalRead(DIGITAL_PIN) == HIGH ? 1 : 0;
      (void)snprintf(tx_buffer, sizeof(tx_buffer),
                     "%05d,%u", input_ana, input_dig);
      SgButSerial.println(tx_buffer);
    } else if (rx_char == V_ANA_CMD) {
      int sample_count = readWithTimeout(SgButSerial, 100 * 1000)  - ' ';
      // Read all the analogue values without any pausing into an array
      for (int i=0; i < sample_count; i++) {
        
#if ADC_READ == ADC_RAW
        int input_ana = analogRead(ANALOGUE_PIN);
#elif ADC_READ == ADC_CALIBRATEDMV
        // ESP32 special for "calibrated" result from Espressif ADC weirdness
        // uint32_t return type limits resolution 
        int input_ana = analogReadMilliVolts(ANALOGUE_PIN); 
#endif
        // write the result over serial - this will slow the loop
        (void)snprintf(tx_buffer, sizeof(tx_buffer),
                       i == (sample_count - 1) ? "%05d\n" : "%05d " , input_ana);
        SgButSerial.print(tx_buffer);
      }
    } else if (rx_char == INFO_CMD) {
      Serial.println("info cmd");
      flash(3);
      (void)snprintf(tx_buffer, sizeof(tx_buffer), "\"INFO\",\"%s\",", SOFTWARE_NAME);
      SgButSerial.print(tx_buffer);
      (void)snprintf(tx_buffer, sizeof(tx_buffer), "\"%s\",", SOFTWARE_VERSION);
      SgButSerial.print(tx_buffer);
      (void)snprintf(tx_buffer, sizeof(tx_buffer), "\"%s\",", BOARD_MANU);
      SgButSerial.print(tx_buffer);
      (void)snprintf(tx_buffer, sizeof(tx_buffer), "\"%s\",", BOARD_NAME);
      SgButSerial.print(tx_buffer);
      (void)snprintf(tx_buffer, sizeof(tx_buffer), "\"%s\",", BOARD_MCU);
      SgButSerial.print(tx_buffer);
      (void)snprintf(tx_buffer, sizeof(tx_buffer), "\"Arduino\",");
      SgButSerial.print(tx_buffer);
      // AVR don't have %f on printf functions
      char adc_vref_str[20];  // not sure how large this needs to be for general case
      dtostrf(ADC_VREF, 0, 1, adc_vref_str);
      (void)snprintf(tx_buffer, sizeof(tx_buffer),
                     "\"adc_bits=%d;aref=%s;input_pin=%d;read=%s\"",
                     ADC_RESOLUTION, adc_vref_str, ANALOGUE_PIN,
                     ADC_READ == ADC_RAW ? "raw" : (ADC_READ == ADC_CALIBRATEDMV ? "calibratedmv" : "?"));
      SgButSerial.println(tx_buffer);
    }
    // TODO - implement the digital commands
  }
}
