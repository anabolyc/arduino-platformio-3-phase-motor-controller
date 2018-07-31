#ifndef _MAIN_H
#define _MAIN_H

#ifdef ARDUINO
#include <pins_arduino.h>
#include <Arduino.h>
#endif

#define OUT_PIN_DIR_1 DDRD
#define OUT_PIN_1     _BV(PIND5)
#define OUT_PIN_2     _BV(PIND6)
#define OUT_PIN_DIR_2 DDRB
#define OUT_PIN_3     _BV(PINB1)
#define OUT_PIN_4     _BV(PINB2)

#define IN_PIN_DIR    DDRC
#define IN_PIN        PINC0
#define IN_PIN_SPEED  _BV(IN_PIN)

#define PWM_CLK_8     0B010
#define PWM_CLK_64    0B011
#define PWM_CLK_256   0B100
#define PWM_CLK_1024  0B101
#define PWM_CLK       PWM_CLK_64

void setup(void);
void loop(void);
uint16_t adc_read(uint8_t pin);
uint16_t freq_ratio();
void spin_up();

// void * operator new(size_t size);
// void operator delete(void * ptr);

// void * operator new(size_t size)
// {
//   return malloc(size);
// }

// void operator delete(void * ptr)
// {
//   free(ptr);
// }

int main(void)
{
  setup();
  for (;;)
  {
    loop();
  }
}

#endif