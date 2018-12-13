/*----------------------------------
            Headers
-----------------------------------*/
#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

/*----------------------------------
            My_Headers
-----------------------------------*/

#include "init.h"
#include "led.h"
#include "uart.h"
#include "adc.h"
#include "ultrasonic.h"
#include "encoders.h"
#include "pid.h"
#include "m_string.h"

/*------------------------------------------------
              Enum Modo Definition

            (Modo) 0 - parado
            (Modo) 1 - AGV
            (Modo) 2 - controlo manual (bluetooth)
-------------------------------------------------*/

typedef enum modo_t{
  STOP,
  AGV,
  MANUAL,
  BACK_HOME
}modo_t;

int main(void)
{

  /*----------------------------------
            Initial Setups
  -----------------------------------*/

  init_config();
  timer0_init();
  timer2_init();
  adc_config();
  USART_Init(BAUDRATE);
  Ultrasonic_Init();
  encoders_init();

  sei();

  /*----------------------------------
              Modo Inicial
  -----------------------------------*/
  modo_t modo = STOP;

  /*--------------------------------------------
  Sensors (ADC) Variables and Values Definitions
  ---------------------------------------------*/

  sensores_t s;
  s.n_canais=4;
  s.canal_atual=0;

  /*----------------------------------------------------
  PID Variables and Values Definitions (Optical Sensors)
  -----------------------------------------------------*/
  uint8_t modo_agv = 0;

  struct PID_variables pid_v;
  struct PID_constants pid_k;

  pid_k.Kp = 35;
  pid_k.Ki = 0;
  pid_k.Kd = 10;

  int8_t PIDvalor = 0;
  int8_t error_old = 0;

  /*----------------------------------------------------
    Encoders and PID Variables and Values Definitions
                  for Straight line
  -----------------------------------------------------*/

  struct encoder_values encoders;

  struct PID_variables pid_v_encoders;
  struct PID_constants pid_k_encoders;

  pid_k_encoders.Kp = 50;
  pid_k_encoders.Ki = 0;
  pid_k_encoders.Kd = 0;

  int8_t PIDvalor_encoders = 0;

  /*----------------------------------
          Other Variables
  -----------------------------------*/
  uint8_t dist=0;
  char a[10];
  char esq[10];
  char dir[10];
  // uint8_t z = 0;

  uint8_t flag_reta = 0;
  uint8_t detect_warning = 0;

  /*----------------------------------
          Infinite Run Loop
  -----------------------------------*/
  while(1)
  {
    /*----------------------------------
                Heartbeat
    -----------------------------------*/
    pisca_led();

    /*----------------------------------------------------
        Get Modo by bluetooth and do stuff for each modo
    -----------------------------------------------------*/
    modo=recepcao_modo(modo);

    switch(modo){
      case STOP:
                PORTC |= (1 << PC4) | (1 << PC5);
                flag_reta = 0;
                break;
      case AGV:

                PORTC |= (1 << PC4);
                PORTC &=~ (1 << PC5);

                /*-----------------------------------------------
                      Get ADC values and store on Structure
                -------------------------------------------------*/
                s.canal_atual=valores_sensores(s.adc, s.n_canais, s.canal_atual);
                conv_logica(s.boo, s.adc,127);

                /*-----------------------------------------------
                                PID & Motor Action
                -------------------------------------------------*/
                pid_v.error = calculo_erro(s.boo, &modo_agv);

                switch (modo_agv) {
                  case 0:
                            OCR0A = 0;
                            OCR0B = 0;
                            break;
                  case 1:
                            PIDvalor = calculo_pid(&pid_v, &pid_k);
                            control_motor_PID(PIDvalor);
                            error_old = pid_v.error;
                            break;
                  case 2:
                            // do{
                            //   pid_v.error = calculo_erro(s.boo, &modo_agv);
                            //   control_motor_PID(-127);
                            pid_v.error = error_old;
                            PIDvalor = calculo_pid(&pid_v, &pid_k);
                            control_motor_PID(PIDvalor);
                            break;
                }

                // if (pid_v.error == 100){
                //   OCR0A = 0;
                //   OCR0B = 0;
                // }


                //mot_sensor(s.boo);


                /*----------------------------------
                            Obstacules
                -----------------------------------*/
                // if (detet_obj(dist,25)==1){
                //   OCR0A = 0;
                //   OCR0B = 0;
                // }

                // if (z == 10000){
                //   z=0;
                //
                //   m_itoa(PIDvalor, a);
                //   memcpy(a+strlen(a),"\r\n",strlen("\r\n")+1);
                // memcpy(a,"0",strlen(a)+1);
                // for (uint8_t i = 0; i < s.n_canais + 1; i++){
                //   a[i] = s.boo[i] + '0';
                // }
                // a[s.n_canais] = '\0';
                // memcpy(a+strlen(a),"\r\n",strlen("\r\n")+1);
                //   send_string(a);
                // }
                // else z++;

                break;
      case MANUAL:
                if (controlo_manual() == 2){
                  detect_warning = 0;
                }
                if (detect_warning == 0) {
                  if (detet_obj(dist,25)==1){
                    OCR0A = 0;
                    OCR0B = 0;
                    detect_warning = 1;
                  }
                }

                break;

      case BACK_HOME:

                PORTC |= (1 << PC4);
                PORTC &=~ (1 << PC5);

                /*----------------------------------
                        Get Values of Encoders
                -----------------------------------*/
                values_encoders(&encoders,0,0,&flag_reta);

                /*----------------------------------
                            Straight Line
                ----------------------------------*/

                pid_v_encoders.error = calculo_erro_encoder(&encoders);
                PIDvalor_encoders = calculo_pid(&pid_v_encoders,&pid_k_encoders);
                control_motor_PID(PIDvalor_encoders);

                /*----------------------------------
                              Obstacules
                -----------------------------------*/

                if (detet_obj(dist,25)==1){
                    OCR0A = 0;
                    OCR0B = 0;
                }


                m_itoa(encoders.left,esq);
                m_itoa(encoders.right,dir);
                //
                memcpy(a, "E", strlen("E")+1);
                memcpy(a+strlen(a), esq, strlen(esq)+1);
                memcpy(a+strlen(a), "D", strlen("D")+1);
                memcpy(a+strlen(a), dir, strlen(esq)+1);
                memcpy(a+strlen(a), "\r\n", strlen("\r\n")+1);
                send_string(a);
                break;
    }
    //leituras_encoder();
    send_trigger();
    dist=calc_dist_cm(dist);
    // if (detet_obj(dist,15)==1){
    //   control_motor_obstacule();
    // }
    //detet_obj(dist,15);

  /*----------------------------------
              END OF LOOP
  -----------------------------------*/

  }
  return 0;
}
