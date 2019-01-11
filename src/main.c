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
            (Modo) 3 - "Calibração"
-------------------------------------------------*/

typedef enum modo_t{
  STOP,
  AGV,
  MANUAL,
  CAL
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

  uint8_t threshold = 127; //Default Value of Threshold for Logical Conversion (0 or 1) of ADC Samples

  /*----------------------------------------------------
  PID Variables and Values Definitions (Optical Sensors)
  -----------------------------------------------------*/
  uint8_t modo_agv = 1;
  uint8_t intersection = 0;

  struct PID_variables pid_v;
  struct PID_constants pid_k;

  pid_k.Kp = 40; //35, 10
  pid_k.Ki = 0;
  pid_k.Kd = 30;

  int8_t PIDvalor = 0;
  int8_t error_old = 0;

  /*----------------------------------------------------
    Encoders and PID Variables and Values Definitions
                  for Straight line
  -----------------------------------------------------*/

  struct encoder_values encoders_values;

  struct encoder_values inicio_reta;
  struct encoder_values reta_e;

  struct PID_variables pid_v_encoders;
  struct PID_constants pid_k_encoders;

  pid_k_encoders.Kp = 2;
  pid_k_encoders.Ki = 0;
  pid_k_encoders.Kd = 1;

  int16_t PIDvalor_encoders = 0;

  
  char *string; //String with Values of Encoders
  uint8_t reta = 0; //Flag No-Line Sector

  /*----------------------------------
          Other Variables
  -----------------------------------*/
  uint8_t direction = 0; 
  
  uint8_t dist=0; //Distância a Objetos

  struct flag_counter flag; //Structure with values of flags of the counters

  uint8_t n_leds = 0; //0 = HeartBeat Led Blinks, 1 = HeartBeat and Status Led Blinks 

  static const uint8_t distance = 25; //Minium Distance of Objects

  /*----------------------------------
          Infinite Run Loop
  -----------------------------------*/
  while(1)
  {
    /*----------------------------------
                Heartbeat
    -----------------------------------*/
    pisca_led(n_leds);

    /*----------------------------------------------------
        Get Modo by bluetooth and do 'stuff' for each modo
    -----------------------------------------------------*/
    modo=recepcao_modo(modo);

    switch(modo){
      case STOP:
                PORTC |= (1 << PC4) | (1 << PC5);
                n_leds = 0;
                break;
      case AGV:

                PORTC |= (1 << PC4);
                PORTC &=~ (1 << PC5);

                n_leds = 0;

                /*----------------------------------
                              Obstacules
                -----------------------------------*/

                if (detet_obj(dist,distance) == 1){
                  PORTC |= (1 << PC4) | (1 << PC5);
                }

                /*-----------------------------------------------
                      Get ADC values and store on Structure
                -------------------------------------------------*/
                s.canal_atual=valores_sensores(s.adc, s.n_canais, s.canal_atual);
                conv_logica(s.boo, s.adc, threshold);

                /*-----------------------------------------------
                                PID & Motor Action
                -------------------------------------------------*/
                pid_v.error = calculo_erro(s.boo, &modo_agv, &intersection);

                switch (modo_agv) {
                  case 0:

                            if(intersection == 1){
                              PORTC |= (1 << PC4) | (1 << PC5);
                              pid_v.error = calculo_erro(s.boo, &modo_agv, &intersection);                              
                              modo_agv = 0;
                            }
                            else{
                              modo_agv = 2;
                              reta = 0;
                            }
                            break;
                  case 1:
                            PIDvalor = calculo_pid(&pid_v, &pid_k);
                            control_motor_PID(PIDvalor);
                            error_old = pid_v.error;
                            reta = 0;
                            break;
                  case 2:
                            pid_v.error = error_old;

                            if(pid_v.error == 0){

                                        /*STRAIGHT LINE*/

                                /*----------------------------------
                                           Get Values of Encoders
                                  -----------------------------------*/
                                if (reta == 0){
                                  values_encoders(&inicio_reta);
                                  values_encoders(&encoders_values);
                                  reta = 1;
                                }

                                reta_e.left = encoders_values.left - inicio_reta.left;
                                reta_e.right = encoders_values.right - inicio_reta.right;

                                /*-------------- --------------------
                                            Straight Line
                                ----------------------------------*/

                                pid_v_encoders.error = calculo_erro_encoder(&reta_e);
                                PIDvalor_encoders = calculo_pid(&pid_v_encoders,&pid_k_encoders);
                                control_motor_PID(PIDvalor_encoders);
                            }

                            else{ //90Degrees
                              
                              //Keep turning
                              PIDvalor = calculo_pid(&pid_v, &pid_k);
                              control_motor_PID(PIDvalor);
                              reta = 0;
                            }
                            break;
                }
                // flag_counters(&flag);
                // if (modo_agv == 1){
                //   reset_flag_counter2();
                // }
                // if (flag.c2 == 1){
                //   modo = STOP;
                // }
                break;
                
      case MANUAL:
                n_leds = 0;
                direction = controlo_manual(direction);
                // if (direction != 2){
                //   if (detet_obj(dist,distance) == 1){
                //     PORTC |= (1 << PC4) | (1 << PC5);
                //   }
                // }
                break;

      case CAL:
                n_leds = 1;
                s.canal_atual=valores_sensores(s.adc, s.n_canais, s.canal_atual);

                //Medium Value Between sensor on channel 0 and channel 1 (Center and Left Sensor)
                threshold = calibr(s.adc[0],s.adc[1]);

                //Medium Value Between sensor on channel 0 and channel 3 (Center and Back Sensor)
                //threshold = calibr(s.adc[0],s.adc[3]); 
                break;
    }
    /*----------------------------------
           Send Trigger Ultrasonic
                    and 
        Calculate distance of objects
    -----------------------------------*/
    send_trigger();
    dist=calc_dist_cm(dist);

    /*----------------------------------
           Get Values of Encoders
    -----------------------------------*/
    values_encoders(&encoders_values);

    /*---------------------------------------------
          Transmit Encoders Values when Requested
    ----------------------------------------------*/    
    if (ask_string_by_uart() == 1){
      string = leituras_encoder(encoders_values);
      send_string(string);
    }

  /*----------------------------------
              END OF LOOP
  -----------------------------------*/
  }
  return 0;
}
