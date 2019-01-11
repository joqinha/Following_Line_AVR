#include <avr/io.h>
#include <avr/interrupt.h>

#include "encoders.h"
#include "m_string.h"

volatile struct encoder_values encoders;

void encoders_init(void){
  DDRD &=~ (1 << PD2);	//Left Encoder	
	DDRD &=~ (1 << PD3);	//Right Encoder
	PORTD |= (1 << PD2);  //Pull-UP R
	PORTD |= (1 << PD3);  //Pull-UP R


  EICRA  = (1 << ISC01) | (1 << ISC00);          //INT0 Rising Edge
  EICRA |= (1 << ISC11) | (1 << ISC10);         //INT1 Rising Edge

  EIMSK  = (1 << INT0)  | (1 << INT1);
}

/*------------------------------------------
      INT0 - Left Wheel Encoder
      INT1 - Right Wheel Encoder
------------------------------------------*/

ISR(INT0_vect){
  if (PINC & 0b00010000){
    encoders.left++;
  }
  if (PINC & 0b00100000){
    encoders.left--;
  }
  if (encoders.left == -32767 || encoders.left == 32767){
      encoders.left = 0;
  }
}

ISR(INT1_vect){
  if (PINC & 0b00010000){
    encoders.right++;
  }
  else if (PINC & 0b00100000){
    encoders.right--;
  }
  if (encoders.right == -32767 || encoders.right == 32767){
      encoders.right = 0;
  }
}

char* leituras_encoder(struct encoder_values encoders){
  char esq[12];
  char dir[12];
  static char res[32];

  /*----------------------------------
             Integer to char
  -----------------------------------*/

  m_itoa(encoders.left,esq);
  m_itoa(encoders.right, dir);

  /*-----------------------------------------------
                       String

          "E=LEFT_ENCODER&D=RIGHT_ENCODER\r\n"
  -----------------------------------------------*/

  memcpy(res, "E=", strlen("E=")+1);
  memcpy(res+strlen(res), esq, strlen(esq)+1);
  memcpy(res+strlen(res), "&D=", strlen("&D=")+1);
  memcpy(res+strlen(res), dir, strlen(dir)+1);
  memcpy(res+strlen(res), "\r\n", strlen("\r\n")+1);

  return res;
}


void values_encoders(struct encoder_values *values){
  values->left = encoders.left;
  values->right = encoders.right;
}

int8_t calculo_erro_encoder(struct encoder_values *values){
  return (values->left - values->right);
}
