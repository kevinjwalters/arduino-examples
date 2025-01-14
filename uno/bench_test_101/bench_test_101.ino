// Ardunio Forum: "Benchmark STM32 vs ATMega328 (nano) vs SAM3X8E (due) vs MK20DX256 (teensy 3.2)"
// From https://forum.arduino.cc/t/benchmark-stm32-vs-atmega328-nano-vs-sam3x8e-due-vs-mk20dx256-teensy-3-2/414630

// Version 2.0
// Added fp64lib for AVR
// Replaced some code with macro to reduce duplication of code

//Version 1.01
//  -Implemented Riva's modifications (to avoid the impact of serial transmission in the calibration)
//  -Not the compiler option is the same on all the platforms (Thanks Westfw)

#pragma GCC optimize ("-O1")

#ifdef __AVR__
#define DOUBLE_LIBRARY
#endif

#ifdef DOUBLE_LIBRARY
#include <fp64lib.h>  // 64bit library for AVR
#endif

#define LOOP_COUNT 30000

int ia,ib,ic,id,ie,ig,ih,ii;
long int la,lb,lc,ld,le,lg,lh,li,N;
unsigned long benchmark_start_ms;
long int runs=0;
float fa,fb,fc,fd,fe,fg;
double da,db,dc,dd,de,df,dg;
#ifdef DOUBLE_LIBRARY
float64_t tda;
float64_t tdb;
#endif

long elapsed;
long calib;


#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)
#define print_results(name_str, unit_str, count, duration)   Serial.print(" " name_str "(" STR(count) ") bench = "); Serial.print(duration); Serial.print("us"); Serial.print(" "); Serial.print((float)(count) / (float)(duration), 4); Serial.println(unit_str)


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
  Serial.begin(250000);

  delay(1 * 1000);
}


void loop() {
  runs++;
  flash(3);
  
  ie=random(1,2);
  benchmark_start_ms = millis();
  elapsed=micros();

  for (ic=ie; ic<(ie+LOOP_COUNT); ic++) //this syntax avoid compiler semplifications
  {
      
  }
  elapsed=micros()-elapsed;
  
  Serial.print(" Start Time = ");
  Serial.print(benchmark_start_ms / 1e3);
  Serial.println("s");
  print_results("INT_LOOP", "MIPS", LOOP_COUNT, elapsed);
  
  la=(long)random(1,2);
  lb=(long)random(1,1000);
  fb=0;
  fg=0;
  le=random(1,2);
  elapsed=micros();
  for (lc=le; lc<(le+LOOP_COUNT); lc++) //this syntax avoid compiler simplifications
  {
      
  }
  elapsed=micros()-elapsed;
  calib=elapsed;
  print_results("LONG_LOOP", "MIPS", LOOP_COUNT, elapsed);

  fa=(float)random(1234,12345);
  fb=(float)random(3,1000);
  fb=0;
  fg=0;
  le=random(1,2);
  elapsed=micros();
  for (lc=le; lc<(le+LOOP_COUNT); lc++)
  {
    fb=fb*fa;       
  }
  elapsed=micros()-elapsed;
  print_results("32BIT_MUL", "MFLOPS", LOOP_COUNT, elapsed - calib);  

  le=random(1,2);
  elapsed=micros();
  for (lc=le; lc<(le+LOOP_COUNT); lc++)
  {
    fb=fb/fa;       
  }
  elapsed=micros()-elapsed;
  print_results("32BIT_DIV", "MFLOPS", LOOP_COUNT, elapsed - calib);
 
#ifdef DOUBLE_LIBRARY
  tda = fp64_long_to_float64(random(456, 7890));
  tdb = fp64_long_to_float64(random(1, LOOP_COUNT));
  le=random(1,2);
  elapsed=micros();
  for (lc=le; lc<(le+LOOP_COUNT); lc++)
  {
    tdb = fp64_mul(tdb, tda);       
  }
  elapsed=micros()-elapsed; 
  print_results("64BIT_MUL", "MFLOPS", LOOP_COUNT, elapsed - calib);

  tda = fp64_long_to_float64(random(456, 7890));
  tdb = fp64_long_to_float64(random(1, LOOP_COUNT));
  le=random(1,2);
  elapsed=micros();
  for (lc=le; lc<(le+LOOP_COUNT); lc++)
  {
    tdb = fp64_div(tdb, tda);       
  }
  elapsed=micros()-elapsed; 
  print_results("64BIT_DIV", "MFLOPS", LOOP_COUNT, elapsed - calib);
#else
  le=random(1,2);
  elapsed=micros();
  for (lc=le; lc<(le+LOOP_COUNT); lc++)
  {
    db=db*da;       
  }
  elapsed=micros()-elapsed; 
  print_results("64BIT_MUL", "MFLOPS", LOOP_COUNT, elapsed - calib);

  da=(double)random(456, 7890);
  db=(double)random(1, LOOP_COUNT);
  db=0;
  dg=0;
  elapsed=micros();
  for (lc=le; lc<(le+LOOP_COUNT); lc++)
  {
    db=db/da;       
  }
  elapsed=micros()-elapsed;
  print_results("64BIT_DIV", "MFLOPS", LOOP_COUNT, elapsed - calib);
#endif  // DOUBLE_LIBRARY

  Serial.println("-------------------------------------------");
  flash(1);
  delay(5 * 1000);
}
