#ifndef _ADC_H_
#define _ADC_H_

#include <stdint.h>

/*----------------------------------------------------
                    ADC Structure

          Numbers of Sensors sampled
          Current ADC channel being sampled
          Array of sampled ADC Values
          Array of logic (0 or 1) sampled ADC Values
----------------------------------------------------*/

typedef struct{
  uint8_t n_canais;
  uint8_t canal_atual;
  uint8_t adc[8];
  uint8_t boo[8];
}sensores_t;

void adc_config(void);
uint8_t valores_sensores(uint8_t *buf, uint8_t n_canais, uint8_t i);
void conv_logica(uint8_t *out, uint8_t *buf, uint8_t threshold);
void mot_sensor(uint8_t *str);

#endif
