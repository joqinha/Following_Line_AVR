#ifndef _PID_H_
#define _PID_H_

struct PID_variables{
  int8_t error;
  int8_t old_error;
  int8_t sum_error;
};

struct PID_constants{
  int8_t Kp;
  int8_t Ki;
  int8_t Kd;
};

int8_t calculo_erro(uint8_t valores[], uint8_t *modo);
int8_t calculo_pid(struct PID_variables *p1,struct PID_constants *p2);
void control_motor_PID(int8_t pid);
void control_motor_obstacule(void);

#endif
