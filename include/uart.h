#ifndef _UART_H_
#define _UART_H_

#ifndef BAUD
#define BAUD 38400
#endif

#ifndef BAUDRATE
#define BAUDRATE ((F_CPU)/(BAUD*16UL)-1)
#endif

#ifndef BUFFER_SIZE
#define BUFFER_SIZE 16
#endif

enum estado{
  NOT_BUSY,
  BUSY
};

typedef struct{
  enum estado rx_flag;
  enum estado tx_flag;
  unsigned char tx_buffer[BUFFER_SIZE];
  unsigned char rx_buffer[BUFFER_SIZE];
  uint8_t cnt_rx;
  uint8_t cnt_tx;
}usart_t;

void USART_Init(uint16_t ubbr);
uint8_t USART_Receive(void);
void USART_Transmit(uint8_t data);

uint8_t recepcao_modo(uint8_t valor);
uint8_t controlo_manual(uint8_t direction);
uint8_t ask_string_by_uart(void);

void send_string(char c[]);

#endif
