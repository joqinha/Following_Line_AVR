#include <avr/io.h>
#include <avr/interrupt.h>

volatile uint8_t flag_1s,cnt_1s;
volatile uint8_t flag_trigger;

void timer2_init(void){
  TCCR2A |= (1 << WGM21);                                                       //CTC
  TCCR2B |= (1 << CS22) | (0 << CS21) | (0 << CS20);                            //Prescaler 64

  OCR2A=249; //f = 250hz
  TCNT2 = 0;
  TIMSK2 |= (1<<OCIE2A);
}

ISR(TIMER2_COMPA_vect)
{
  cnt_1s++;
  if (cnt_1s==1){
    flag_trigger=1;
  }
  if(cnt_1s==250)
  {
    flag_1s=1;
  }
}

void pisca_led(void){
  if (flag_1s==1){
    PORTD ^= (1<<PD7);
    flag_1s=0;
    cnt_1s=0;
  }
}
