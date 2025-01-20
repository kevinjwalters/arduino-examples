/*
  benchmark-random v1.0
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

// Critical to have negative value at end terminating values
const int REPS_FOR_RANDOM = 5;
long pauses_ms[] = {1000, 500, 250, 100, 50, 25, 10, 5, 2, 1, 0, -1};
unsigned long seed = 1234UL;

void setup() {
  while (!Serial) {};
  Serial.begin(115200);

  Serial.println("Seed=None");
  benchmark_random(REPS_FOR_RANDOM, pauses_ms);
  Serial.println();
}

// Run random repeats time with delays from pauses_ms array
// This is slightly flawed due to delays from serial writes
void benchmark_random(int repeats, long pauses_ms[]) {
  unsigned long random_duration[repeats];
  unsigned long tally;
  unsigned long t1, t2;

  for (size_t idx=0; pauses_ms[idx] >= 0; idx++) {
    long ir_pause_ms = pauses_ms[idx];
    tally = 0;
    for (int rep=0; rep < repeats; rep++) {      
      delay(ir_pause_ms);
      t1 = micros();    
      tally += random();
      t2 = micros();
      random_duration[rep] = t2 - t1;
    }

    Serial.print("Random() perf in us (tally=");
    Serial.print(tally);
    Serial.print(", delay (ms)=");
    Serial.print(ir_pause_ms);
    Serial.print(") = ");
    for (int rep=0; rep < repeats - 1; rep++) {
      Serial.print(random_duration[rep]);
      Serial.print(", ");
    }
    Serial.println(random_duration[repeats - 1]);
  }
}


// The one-off unseeded benchmark run has already been performed from setup()
void loop() {
  Serial.print("Seed=");
  Serial.println(seed);
  randomSeed(seed);
  benchmark_random(REPS_FOR_RANDOM, pauses_ms);
  Serial.println();
  
  seed += random();
  delay(5 * 1000L);
}
