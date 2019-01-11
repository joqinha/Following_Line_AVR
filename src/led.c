#include <avr/io.h>
#include <avr/interrupt.h>

#include "init.h"

struct flag_counter flag;

void pisca_led(uint8_t n_leds){
  flag_counters(&flag);
  if (flag.c1==1){
    if (n_leds == 1){
      PORTB ^= (1 << PB1);
    }
    else {
      PORTB &=~(1 << PB1);
    }
    PORTD ^= (1<<PD7);
    reset_flag_counter1();
  }
}