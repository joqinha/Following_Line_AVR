#ifndef _INIT_H_
#define _INIT_H_

#ifndef F_CPU
#define F_CPU 8000000UL // 8 MHz clock speed
#endif

void init_config();
void timer0_init();

#endif
