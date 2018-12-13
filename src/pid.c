#include <avr/io.h>
#include <util/delay.h>

#include "pid.h"

/*------------------------------------------------------------------------------------

                Calculo erro

      Esquerda  Centro  Direita   Traseiro   Erro
         1         0      0           x       2
         1         1      0           x       1
         0         1      0           x       0
         0         1      1           x      -1
         0         0      1           x      -2
         1         1      1           x       0 - INTERCEPÇAO OU SEM LINHA (FAZER RETA)
         0         0      0           0      -- - FORA DE PISTA - PARAR
         0         0      0           1      -- - STOP SIGNAL - PARAR

--------------------------------------------------------------------------------------*/

int8_t calculo_erro(uint8_t valores[], uint8_t *modo){
  int8_t erro = 0;
  if ((valores[1]==0) && (valores[0]==0) && (valores[2]==1)){
    erro = -2;
    *modo = 1;
  }
  else if ((valores[1]==0) && (valores[0]==1) && (valores[2]==1)){
    erro = -1;
    *modo = 1;
  }
  else if ((valores[1]==0) && (valores[0]==1) && (valores[2]==0)){
    erro = 0;
    *modo = 1;
  }
  else if ((valores[1]==1) && (valores[0]==1) && (valores[2]==0)){
    erro = 1;
    *modo = 1;
  }
  else if ((valores[1]==1) && (valores[0]==0) && (valores[2]==0)){
    erro = 2;
    *modo = 1;
  }
  else if ((valores[1]==1) && valores[0]==1 && valores[2]==1){
    erro = 0;
    *modo = 1;
  }
  else if ((valores[1]==0) && valores[0]==0 && valores[2]==0 && valores[3] == 1){
    *modo = 0;
  }
  else if ((valores[1]==0) && valores[0]==0 && valores[2]==0 && valores[3] == 0){
    *modo = 2;
  }
  return erro;
}

int8_t calculo_pid(struct PID_variables *p1,struct PID_constants *p2){
  int8_t P,I,D,PIDvalor;

  P = p1->error;
  I = p1->sum_error + p1->error;
  D = p1->error - p1->old_error;
  PIDvalor = (p2->Kp*P) + (p2->Ki*I) + (p2->Kd*D);

  p1->old_error = P;
  p1->sum_error = I;

  return PIDvalor;
}

/*-----------------------------------------------
                  OCR0A = LEFT
                  OCR0B = RIGHT
-------------------------------------------------*/

void control_motor_PID(int8_t pid){
  uint8_t speed_right = 255;
  uint8_t speed_left = 255;


  /*-----------------------------------------------
        If error<0, reduce speed of right wheel
  -------------------------------------------------*/

  if (pid < 0){
    OCR0A = speed_left;
    OCR0B = speed_right + pid;
  }

  /*-----------------------------------------------
        If error > 0, reduce speed of left wheel
  -------------------------------------------------*/
  else if (pid > 0){
    OCR0A = speed_left - pid;
    OCR0B = speed_right;
  }

  /*-----------------------------------------------
          If error = 0, two wheels same speed
  -------------------------------------------------*/

  else {
    OCR0A = speed_left;
    OCR0B = speed_right;
  }
}

void control_motor_obstacule(void){
  OCR0A = 0;
  OCR0B = 0;

  OCR0A = 255;
  _delay_ms(500);
  OCR0B = 255;
  _delay_ms(500);
  OCR0A = 0;
  _delay_ms(500);
}
