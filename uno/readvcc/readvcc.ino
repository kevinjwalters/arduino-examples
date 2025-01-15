// Tweaked version of readVcc from https://forum.arduino.cc/t/readvcc-function-using-new-uno-r4-ra4m1-adc/1150116/19

#define R4_READ_VOLTAGE_EVERY_TIME 1
 
const long intVREFcalc = 1100L * 1024L;
const long intVREFbin = intVREFcalc;
const float intVREFadj = 1.0f;

// function to read actual supply voltage Vcc (in mV) of Arduino
// - adapted to also work with new UNO R4
// - returns 0 if board/chip is unsupported
long readVcc() {
  long result = 0;
  // setting correct bits in ADMUX register to read internal 1.1 V ref against AVcc (based on which AVR chip, e.g. Uno R3: REFS1 = 0, REFS0 = 1, MUX3:0 = 1110),
  // or directly read Vcc if newer Uno R4 (RA4M1 chip)
  #if defined(ARDUINO_ARCH_AVR)  // for AVR boards (such as Arduino Uno R3)
  
    #if defined(__AVR_ATmega168__) || defined(__AVR_ATmega328__) || defined(__AVR_ATmega328P__)  // if e.g. (very) old Arduino, or Nano, or Uno R3 is used
      ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
    #elif defined(__AVR_ATmega32U4__) || defined(__AVR_ATmega1280__) || defined(__AVR_ATmega2560__) // if e.g. Arduino Leonardo, or Mega is used
      ADMUX = _BV(REFS0) | _BV(MUX4) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
    #elif defined(__AVR_ATtiny24__) || defined(__AVR_ATtiny44__) || defined(__AVR_ATtiny84__) // if using different kinds of ATtiny 
      ADMUX = _BV(MUX5) | _BV(MUX0);
    #elif defined(__AVR_ATtiny25__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny85__) // if using different kinds of ATtiny
      ADMUX = _BV(MUX3) | _BV(MUX2);
    #else                          
      return 0;
    #endif  

      delay(2);                           // Wait 2 ms for Vref to settle after change
      ADCSRA |= _BV(ADSC);                // Write ADSC bit to 1 in "ADC Control and Status Register A" for starting single ADC conversion

      while (bit_is_set(ADCSRA, ADSC)) {};  // when ADC conversion is over, ADSC bit will go back to 0
      // ADC generates 10-bit result (containing measurement of 1.1V int. ref.) in ADC data registers: ADCH (high) and ADCL (low)
      result = ADCL;                      // if result is left adjusted & requires <=8-bit precision, ADCH is sufficient, otherwise: ADCL read first (important!), then ADCH
      result |= ADCH << 8;                // ADCL, then ADCH (to ensure registers hold data of same conversion), 8-place bit shift to left: obtain full-precision 10 bit result
      result = intVREFbin / result;       // Calculate Vcc (in mV); intVREFbin = ~1100 mV * 1024
  #elif defined(ARDUINO_ARCH_RENESAS)     // for RA4M1 boards (such as Arduino Uno R4)
    // analogReference only reads voltage first time - tut
  #if R4_READ_VOLTAGE_EVERY_TIME
    /// Code borrowed from https://github.com/arduino/ArduinoCore-renesas/blob/main/cores/arduino/analog.cpp
    int arefs = 0;
    analogReference(AR_INTERNAL);
    delayMicroseconds(5);
    for (int i = 0; i < 10; i++) {
      arefs += analogRead(AVCC_MEASURE_PIN);
    }
    analogReference(AR_DEFAULT);
    float Vcc = (float)arefs * AR_INTERNAL_VOLTAGE * AVCC_MULTIPLY_FACTOR / 1024.0f / 10.0f;
#else
  float Vcc = analogReference();        // analogReference() reads Vcc, based on internal reference, and returns value as a float
#endif  // R4_READ_EVERY_TIME
    result = Vcc * intVREFadj * 1000.0f;        //  intVREF * 1000;   // result is corrected for incorrect internal reference (fixed factor) and converted to mV
  #else
    // return result Vcc of 0 if chip is not supported by this function
  #endif
  return result;
}


void setup() {
  while (!Serial) {};
  Serial.begin(115200);
}


void loop() {
  Serial.print("Vcc=");
  Serial.print(readVcc());
  Serial.println("mV");
  delay(1000);
}
