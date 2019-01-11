#include <avr/io.h>
#include <avr/interrupt.h>

#include "adc.h"

volatile uint8_t flag_leitura, leituraH;

/*------------------------------------------
      Ativa ADC, Prescaler 64
      Zera Interrupt Flag e ativa interrupcao adc
      Inicia Conversao
------------------------------------------*/

void adc_config(void){
  ADMUX |= (1 << ADLAR);
  ADCSRA |= (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (0 << ADPS0);
  ADCSRA |= (0 << ADIF) | (1 << ADIE); 

  ADCSRA |= (1 << ADSC);
}

ISR(ADC_vect){
  leituraH = ADCH;
  flag_leitura = 1;
}

/*---------------------------------------------------------
      Read n_canais number of consecutive channels of ADC
      Stores ADC values on array buf
----------------------------------------------------------*/

uint8_t valores_sensores(uint8_t *buf, uint8_t n_canais, uint8_t i){
  if (flag_leitura == 1){
    *(buf+i)=leituraH;
    i++;
    if (i==n_canais) {i=0;}
    ADMUX = (1 << ADLAR);
    ADMUX += i;
    flag_leitura=0;

    ADCSRA |= 1<<ADSC;
  }
  return i;
}

/*---------------------------------------------------------
      Medium Value Between 2 channel of ADC
----------------------------------------------------------*/
uint8_t calibr(uint8_t canal0, uint8_t canal1){
  if (canal0 && canal1 !=0){
    return (canal0+canal1)/2;
  }
  return 0;
}

/*--------------------------------------
    posicao 0 - meio
    posicao 1 - esquerda
    posicao 2 - direita
    posicao 3 - traseiro

        Preto = 1
        Branco = 0

    Read Values of Array buf
    Stores on array out
      if ADC < threshold stores 0
      if ADC > threshold stores 1
----------------------------------------*/

void conv_logica(uint8_t *out, uint8_t *buf, uint8_t threshold){
  uint8_t n_bits=8;
  for (uint8_t i=0;i<n_bits;i++){
    if (buf[i] > threshold){
      *(out+i)=1;
    }
    else{
      *(out+i)=0;
    }
  }
}

