#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

#include "init.h"
#include "ultrasonic.h"

struct flag_counter flag;

volatile uint16_t ticks_t1;
volatile uint16_t ticks_t2;

volatile edge_t edge;

#define  Trigger_pin	PD4	/* Trigger pin */
#define	 Echo_Pin PB0 /*Echo pin*/

void Ultrasonic_Init(void)
{
  	DDRD |= (1 << Trigger_pin); 
  	PORTD |= (0 << Trigger_pin);

	DDRB |= (0 << Echo_Pin);
	PORTB |= (1 << Echo_Pin);

  	//Timer1 Initi

	TIMSK1 = (1 << ICIE1); //Input Capture interrupt
	edge.current_edge = INIT_RISING;
	edge.next_edge = INIT_RISING;

  	TCCR1B = (1 << ICES1) | (1 << ICNC1); //Rising Edge & Noise cancelation
  	TCCR1B |= (1 << CS12); //Prescaler 256

	//TIMSK1 |= (1 << TOIE1); //Overflow Interrupt
}

void send_trigger(void){
	flag_counters(&flag);
	if (flag.begin==1){
		PORTD |= (1 << Trigger_pin);
		_delay_us(20);
		PORTD &=~(1 << Trigger_pin);
		reset_flag_counter_begin();
	}
}

ISR(TIMER1_CAPT_vect) {
	switch (edge.next_edge) {
	   case INIT_RISING:
		                  ticks_t1 = ICR1;
		                  TCCR1B &= ~(1 << ICES1); /*Next Interrupt on Falling edge*/
		                  edge.next_edge = FALLING;
		                  break;

     case RISING:
		 									ticks_t1 = ICR1;
		                  TCCR1B &= ~(1 << ICES1); /*Next Interrupt on Falling edge*/
		                  edge.current_edge = RISING;
                      edge.next_edge = FALLING;
                      break;

	   case FALLING:
		                  ticks_t2 = ICR1;
		                  TCCR1B |= (1 << ICES1); /*Next Interrupt on Rising edge*/
		                  edge.current_edge = FALLING;
		                  edge.next_edge = RISING;
		                  break;
		default:
		                  break;
	}
}

uint8_t calc_dist_cm(uint8_t d_cm){
  	if (edge.current_edge == FALLING) {
		uint16_t dticks = 0;
		uint16_t t_us = 0;
		if (ticks_t2 > ticks_t1){
			dticks = ticks_t2 - ticks_t1;
		}
		else{
			dticks = (65535 - ticks_t1) + ticks_t2;
		}
		t_us = dticks * 32 ;
		d_cm = t_us / 58;
	}
	return d_cm;
}

int8_t detet_obj(uint8_t distance, uint8_t dist_ativ){
	if (distance < dist_ativ) {
		PORTB |= (1 << PB1);
		return 1;
	}
	else {
		PORTB &=~ (1 << PB1);
		return 0;
	}
	return -1;
}
