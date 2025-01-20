/*
  benchmark-random v1.1
  Check performance of Arduino random()

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

// This is intended to show performance of Renesas RA4M1 feature
// for generating true random number (TRNG) part of SCE5 module
// and look for hardware buffered entry data / exhaustion


const char *VERSION = "1.1";
const int REPS_FOR_RANDOM = 10;  // BenchBros can increase this

// Critical to have negative value at end terminating values
long pauses_ms[] = {100, 50, 25, 10, 5, 2, 1, 0, -1};
unsigned long seed = 1234UL;


void serial_print_header(bool use_value, long value, bool use_seed, long seed) {
  Serial.print("Benchmark Random() version=");
  Serial.print(VERSION);
  Serial.print(", value=");
  if (use_value) {
    Serial.print(value);
  } else {
    Serial.print("none");
  }
  Serial.print(", seed=");
  if (use_seed) {
    Serial.println(seed);
  } else {
    Serial.println("none");
  }
}


// Run random repeats time with delays from pauses_ms array
// This is slightly flawed due to delays from serial writes
void benchmark_random(int repeats, long pauses_ms[], int argc=0, long arg1=0, long arg2=0) {
  unsigned long random_duration[repeats];
  unsigned long tally;
  unsigned long t1, t2;

  for (size_t idx=0; pauses_ms[idx] >= 0; idx++) {
    long ir_pause_ms = pauses_ms[idx];
    tally = 0;
    if (argc == 0) {
      for (int rep=0; rep < repeats; rep++) {      
        delay(ir_pause_ms);
        t1 = micros();    
        tally += random();
        t2 = micros();
        random_duration[rep] = t2 - t1;
      }
    } else if (argc == 1) {
      for (int rep=0; rep < repeats; rep++) {      
        delay(ir_pause_ms);
        t1 = micros();    
        tally += random(arg1);
        t2 = micros();
        random_duration[rep] = t2 - t1;
      }
    } else if (argc == 2) {
      for (int rep=0; rep < repeats; rep++) {      
        delay(ir_pause_ms);
        t1 = micros();    
        tally += random(arg1, arg2);
        t2 = micros();
        random_duration[rep] = t2 - t1;
      }
    }
    // Calculate arithmetic mean
    float avg = 0.0;
    for (int rep=0; rep < repeats; rep++) {
      avg += random_duration[rep];
    }
    avg /= repeats;

    Serial.print("Random() perf in us (tally=");
    Serial.print(tally);
    Serial.print(", delay (ms)=");
    Serial.print(ir_pause_ms);
    Serial.print(", avg (us)=");
    Serial.print(avg, 1);
    Serial.print(") = ");
    for (int rep=0; rep < repeats - 1; rep++) {
      Serial.print(random_duration[rep]);
      Serial.print(", ");
    }
    Serial.println(random_duration[repeats - 1]);
  }
}


void setup() {
  while (!Serial) {};
  Serial.begin(115200);

  serial_print_header(true, 2025, false, 0);
  benchmark_random(REPS_FOR_RANDOM, pauses_ms, 1, 2025);
  Serial.println();
}


// The one-off unseeded benchmark run has already been performed from setup()
void loop() {
  serial_print_header(true, 2025, true, seed);
  randomSeed(seed);
  benchmark_random(REPS_FOR_RANDOM, pauses_ms, 1, 2025);
  Serial.println();
  
  seed += random();
  delay(5 * 1000L);
}
