#ifndef _ENCODERS_H_
#define _ENCODERS_H_

#include <stdint.h>
#include <stddef.h>

struct encoder_values{
  int16_t left;
  int16_t right;
};

void encoders_init(void);
char* leituras_encoder(struct encoder_values encoders);
void values_encoders(struct encoder_values *values);
int8_t calculo_erro_encoder(struct encoder_values *values);

#endif
