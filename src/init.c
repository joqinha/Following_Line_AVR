#include <avr/io.h>
#include <avr/interrupt.h>

#include "init.h"

volatile struct counter cnt;
volatile struct flag_counter flag;

#define HeartBeatPin PD7 //Heartbeat PIN
#define StatusPin PB1 //Status led

#define counter2 3 //N (3) Segundos

void init_config(void){
  DDRD = (1 << PD5) | (1 << PD6); //Pin OC0B e OC0A

  DDRD |= (1 << HeartBeatPin); 

  DDRB |= (1 << StatusPin);     

  DDRC = (1 << PC4) | (1 << PC5);
  PORTC |= (0 << PC4) | (0 << PC5);
}

/*-----------------------------------------------------
                Timer0 Config 
                Fast PWM
                Prescaler 1
                Non-inverted
                f=31,25khz
-----------------------------------------------------*/

void timer0_init(void){ 
  TCCR0A = (1 << WGM01) | (1 << WGM00); 

  TCCR0B = (1 << CS00);

  //Channel A
  TCCR0A |= (1 << COM0A1);

  //Channel B
  TCCR0A |= (1 << COM0B1); 

}

/*-----------------------------------------------------
                Timer2 Config 
                CTC Prescaler 64
                f = 250hz
-----------------------------------------------------*/

void timer2_init(void){
  TCCR2A |= (1 << WGM21);
  TCCR2B |= (1 << CS22) | (0 << CS21) | (0 << CS20);

  OCR2A=249;
  TCNT2 = 0;
  TIMSK2 |= (1<<OCIE2A);
}

ISR(TIMER2_COMPA_vect)
{
  cnt.c1++;
  if (cnt.c1 == 1){
    flag.begin = 1;
  }
  if(cnt.c1==250)
  {
    flag.c1=1;
    cnt.c1 = 0;
    cnt.c2++;
    if (cnt.c2 == counter2){
      cnt.c2 = 0;
      flag.c2 = 1;
    }
  }
}

void flag_counters(struct flag_counter *values){
  values->begin = flag.begin;
  values->c1 = flag.c1;
  values->c2 = flag.c2;
}

void reset_flag_counter_begin(void){
  flag.begin = 0;
}

void reset_flag_counter1(void){
  flag.c1 = 0;
}

void reset_flag_counter2(void){
  flag.c2 = 0;
}

// /*-----------------------------------------------------
                
// ------------------OLD-----------------------------------

//                 Timer0 Config 
//                 PWM Phase Correct
//                 Prescaler 1
//                 Non-inverted
//                 f=15,7khz
// -----------------------------------------------------*/

// void timer0_init(void){ 
//   TCCR0A = (1 << WGM00); 

//   TCCR0B = (1 << CS00);

//   //Channel A
//   TCCR0A |= (1 << COM0A1);

//   //Channel B
//   TCCR0A |= (1 << COM0B1); 

// }