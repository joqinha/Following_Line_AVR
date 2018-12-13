#ifndef _ULTRASONIC_H_
#define _ULTRASONIC_H_

typedef enum edge_state_t{
	INIT_RISING,
	RISING,
	FALLING
}edge_state;

typedef struct {
	edge_state current_edge;
	edge_state next_edge;
}edge_t;

void Ultrasonic_Init(void);
void send_trigger(void);
uint8_t calc_dist_cm(uint8_t d_cm);
int8_t detet_obj(uint8_t distance, uint8_t dist_ativ);

#endif
