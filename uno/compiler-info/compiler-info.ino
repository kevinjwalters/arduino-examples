/*
  compiler-info v1.0
  Print compiler C++ standard, flags and variable sizes

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


#ifdef ARDUINO_AVR_UNO
  #define MODEL "UNO R3"
#elif ARDUINO_AVR_LEONARDO
  #define MODEL "Leonardo"
#elif ARDUINO_UNOR4_MINIMA
  #define MODEL "UNO R4 Minima"
#elif ARDUINO_UNOR4_WIFI
  #define MODEL "UNO R4 WiFi"
#else
  #define MODEL "unknown to cpp if conditions"
#endif


const char *cpp_ver_table[][2] = {{"199711", "C++98 / C++03"},
                                  {"201103", "C++11"},
                                  {"201402", "C++14"},
                                  {"201703", "C++17"},
                                  {"202002", "C++20"},                                                                                                                                        
                                  {"202302", "C++23"},
                                  {NULL, NULL}};

String decode_version(long cpp_ver_number) {
  String cpp_ver_str(cpp_ver_number);

  for (size_t idx; cpp_ver_table[idx][0] != NULL; idx++) {
    if (cpp_ver_str == cpp_ver_table[idx][0]) {
      return String(cpp_ver_table[idx][1]);
    }
  }
  return String("?");
}


void setup() {
  while (!Serial) {};
  Serial.begin(115200);
}


void loop() {
  Serial.print("Board model: ");
  Serial.println(MODEL);

	Serial.print("Compiler version: ");
  Serial.println(__VERSION__);

  Serial.print("C++ version: ");
  Serial.print(__cplusplus);
  Serial.print(" (");
  Serial.print(decode_version(__cplusplus));
  Serial.println(")");

  Serial.print("sizeof(int, long, pointer): ");
  Serial.print(sizeof(int)); Serial.print(", ");
  Serial.print(sizeof(long)); Serial.print(", ");
  Serial.println(sizeof(void *));

  Serial.print("sizeof(float, double, long long): ");
  Serial.print(sizeof(float)); Serial.print(", ");
  Serial.print(sizeof(double)); Serial.print(", ");
  Serial.println(sizeof(long long));

  delay(60 * 1000L);
}
