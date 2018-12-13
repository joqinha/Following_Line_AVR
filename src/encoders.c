#include <avr/io.h>
#include <avr/interrupt.h>

#include "encoders.h"
#include "uart.h"
#include "m_string.h"

volatile encoder_t encoders;

void encoders_init(void){
  EICRA  = (1 << ISC01) | (1 << ISC00);          //INT0 Rising Edge
  EICRA |= (1 << ISC11) | (1 << ISC10);         //INT1 Rising Edge
  EIMSK  = (1 << INT0)  | (1 << INT1);
}

/*------------------------------------------
      INT0 - Left Wheel Encoder
      INT1 - Right Wheel Encoder
------------------------------------------*/

ISR(INT0_vect){
  encoders.cnt_encoder0++;
  if (encoders.cnt_encoder0 == 255){
      encoders.cnt_encoder0 = 0;
  }
  // if (encoders.cnt_encoder0 == 5){
  //   encoders.tmp0 = encoders.cnt_encoder0;
  //   encoders.tmp1 = encoders.cnt_encoder1;
  //   encoders.flag_encoder = 1;
  //   encoders.cnt_encoder0 = 0;
  //   encoders.cnt_encoder1 = 0;
  // }
}

ISR(INT1_vect){
  encoders.cnt_encoder1++;

  if (encoders.cnt_encoder1 == 255){
      encoders.cnt_encoder1 = 0;
  }

  // if (encoders.cnt_encoder1 == 5){
  //   encoders.tmp0 = encoders.cnt_encoder0;
  //   encoders.tmp1 = encoders.cnt_encoder1;
  //   encoders.flag_encoder = 1;
  //   encoders.cnt_encoder0 = 0;
  //   encoders.cnt_encoder1 = 0;
  // }
}

void leituras_encoder(void){
  char esq[8];
  char dir[8];
  char res[16];
  if (encoders.flag_encoder == 1){

    //Reset Flag
    encoders.flag_encoder = 0;

    /*----------------------------------
              Integer to char
    -----------------------------------*/

    m_itoa(encoders.tmp0, esq);
    m_itoa(encoders.tmp1, dir);

    encoders.tmp0=0;
    encoders.tmp1=0;

    /*-----------------------------------------------
                        String

          "E LEFT_ENCODER D RIGHT_ENCODER\r\n"
    -----------------------------------------------*/

    memcpy(res, "E", strlen("E")+1);
    memcpy(res+strlen(res), esq, strlen(esq)+1);
    memcpy(res+strlen(res), "D", strlen("D")+1);
    memcpy(res+strlen(res), dir, strlen(dir)+1);
    memcpy(res+strlen(res), "\r\n", strlen("\r\n")+1);

    send_string(res);
  }
}


void values_encoders(struct encoder_values *values, uint8_t left_inicial, uint8_t right_inicial, uint8_t *flag){
  if (flag == 0){
    encoders.cnt_encoder0 = left_inicial;
    encoders.cnt_encoder1 = right_inicial;
    *flag = 1;
  }
  values->left = encoders.cnt_encoder0;
  values->right = encoders.cnt_encoder1;
}

int8_t calculo_erro_encoder(struct encoder_values *values){
  return (values->left - values->right);
}
