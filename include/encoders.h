#ifndef _ENCODERS_H_
#define _ENCODERS_H_

#include <stdint.h>
#include <stddef.h>

typedef struct{
  uint8_t cnt_encoder0;
  uint8_t cnt_encoder1;
  uint8_t flag_encoder;
  uint8_t tmp0;
  uint8_t tmp1;
}encoder_t;

struct encoder_values{
  uint8_t left;
  uint8_t right;
};

void encoders_init(void);
void leituras_encoder(void);
void values_encoders(struct encoder_values *values, uint8_t left_inicial, uint8_t right_inicial, uint8_t *flag);
int8_t calculo_erro_encoder(struct encoder_values *values);

void foo(uint8_t *a);

#endif
