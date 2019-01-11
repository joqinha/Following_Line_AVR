#ifndef _INIT_H_
#define _INIT_H_

#ifndef F_CPU
#define F_CPU 8000000UL // 8 MHz clock speed
#endif

struct counter {
  uint8_t c1;
  uint8_t c2;
};

struct flag_counter {
  uint8_t begin;
  uint8_t c1;
  uint8_t c2;
};

void init_config();
void timer0_init();
void timer2_init(void);
void flag_counters(struct flag_counter *values);
void reset_flag_counter_begin(void);
void reset_flag_counter1(void);
void reset_flag_counter2(void);

#endif
