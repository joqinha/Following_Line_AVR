#include <avr/io.h>
#include <avr/interrupt.h>

#include "adc.h"

volatile uint8_t flag_leitura, leituraH;

void adc_config(void){
  ADMUX |= (1 << ADLAR);
  ADCSRA |= (1 << ADEN) | (1 << ADPS2) | (1 << ADPS1) | (0 << ADPS0); //Ativa ADC, Prescaler 64
  ADCSRA |= (0 << ADIF) | (1 << ADIE); //Zera Interrupt Flag e ativa interrupcao adc

  ADCSRA |= (1 << ADSC); //Inicia Conversao
}

ISR(ADC_vect){
  leituraH = ADCH;
  flag_leitura = 1;
}

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

/*---------------------------
    posicao 0 - meio
    posicao 1 - esquerda
    posicao 2 - direita
    posicao 3 - traseiro

        Preto = 1
        Branco = 0
----------------------------*/

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

void mot_sensor(uint8_t *str){
  uint8_t n_bits = 8;
  uint8_t z = 0;
  for(uint8_t i=0;i<n_bits;i++){
    z |= (str[i] << (n_bits-1-i));
  }
  switch (z) {
    case 0b10000000:
    case 0b10010000: OCR0A = 255;
                     OCR0B = 255;
                     break;
    case 0b01000000:
    case 0b01010000: OCR0A = 0;
                     OCR0B = 255;
                     break;
    case 0b00100000:
    case 0b00110000: OCR0A = 255;
                     OCR0B = 0;
                     break;
    case 0b11000000:
    case 0b11010000: OCR0A = 127;
                     OCR0B = 255;
                     break;
    case 0b10100000:
    case 0b10110000: OCR0A = 255;
                     OCR0B = 127;
                     break;
    case 0b11100000:
    case 0b11110000:
                     OCR0A = 255;
                     OCR0B = 255;
                     break;
    case 0b00010000:
    case 0b00000000:
    default:         OCR0A = 0;
                     OCR0B = 0;
                     break;
  }
}
