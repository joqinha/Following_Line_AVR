#include <avr/io.h>
#include <avr/interrupt.h>

#include "init.h"

void init_config(void){
  DDRD = (1 << PD5) | (1 << PD6); //Pin OC0B e OC0A
  DDRD |= (1 << PD7); //Pin LED

  DDRC = (1 << PC4) | (1 << PC5);
  PORTC |= (0 << PC4) | (0 << PC5);
}

void timer0_init(void){ //f=15,7khz
  TCCR0A = (1 << WGM00); // Modo 1, PWM Phase Correct

  TCCR0B = (1 << CS00);// prescaler 1

  //Channel A
  TCCR0A |= (1 << COM0A1); //Non-inverted

  //Channel B
  TCCR0A |= (1 << COM0B1); //Non-inverted

}
