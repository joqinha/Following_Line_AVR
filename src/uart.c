#include <avr/io.h>
#include <avr/interrupt.h>
#include <string.h>

#include "uart.h"

/*----------------------------------
      Static Functions Prototypes
-----------------------------------*/

static void enable_uart_transmission(void);
static void enable_uart_reception(void);

/*----------------------------------
      Global Structure usart_t
-----------------------------------*/

volatile usart_t usart;

void USART_Init(uint16_t ubbr){
    UBRR0H = (uint8_t)(ubbr>>8);                      // shift the register right by 8 bits
    UBRR0L = (uint8_t) ubbr;                         // set baud rate

    UCSR0B  = (1<<TXEN0)  | (1<<RXEN0);              // enable receiver and transmitter
    UCSR0B |= (1<<RXCIE0);                        //enable reception interrupt

    UCSR0C  = (1<<UCSZ00) | (1<<UCSZ01);            // 8bit, 1 stop bit
}

ISR(USART_RX_vect)
{
  if(usart.tx_flag != BUSY){
    usart.rx_buffer[usart.cnt_rx] = UDR0;
    if (usart.rx_buffer[usart.cnt_rx] == '\r'){
      usart.rx_flag = NOT_BUSY;
      if(usart.cnt_rx >= BUFFER_SIZE){
        usart.cnt_rx = 0;
      }
    }
    else {
      usart.rx_flag = BUSY;
      usart.cnt_rx++;
    }
  }
}

ISR(USART_TX_vect){
  if (usart.rx_flag != BUSY){
    if (usart.tx_buffer[usart.cnt_tx] != '\n'){
      usart.cnt_tx++;
      UDR0 = usart.tx_buffer[usart.cnt_tx];

      if (usart.cnt_tx >= BUFFER_SIZE){
        usart.cnt_tx=0;
      }
    }
    else {
      usart.cnt_tx = 0;
      usart.tx_flag = NOT_BUSY;
      enable_uart_reception();
    }
  }
}



uint8_t recepcao_modo(uint8_t valor)
{
  if (usart.rx_flag==NOT_BUSY){
    for(uint8_t i = 0; i < usart.cnt_rx; i++){
      if (usart.rx_buffer[i]=='M'){
        switch(usart.rx_buffer[i+1]){
          case 'S':
                    valor=0;
                    usart.cnt_rx = 0;
                    break;
          case '1':
                    valor=1;
                    usart.cnt_rx = 0;
                    break;
          case '2':
                    valor=2;
                    usart.cnt_rx = 0;
                    break;
          case '3':
                    valor=3;
                    usart.cnt_rx = 0;
                    break;
          default:
                    usart.cnt_rx = 0;
                    break;
        }
      }
    }
  }
  return valor;
}

uint8_t controlo_manual(void){
  if (usart.rx_flag==NOT_BUSY){
    for(uint8_t i = 0; i < usart.cnt_rx; i++){
      if (usart.rx_buffer[i]=='D'){
        switch (usart.rx_buffer[i+1]) {
          case 'L':
                  OCR0A=127;
                  OCR0B=255;
                  usart.cnt_rx = 0;
                  return 1;
                  break;
          case 'R':
                  OCR0B=127;
                  OCR0A=255;
                  usart.cnt_rx = 0;
                  return 1;
                  break;
          case 'F':
                  PORTC |= (1 << PC4);
                  PORTC &=~ (1 << PC5);
                  OCR0A=255;
                  OCR0B=255;
                  usart.cnt_rx = 0;
                  return 1;
                  break;
          case 'B':
                  PORTC |= (1 << PC5);
                  PORTC &=~ (1 << PC4);
                  OCR0A=255;
                  OCR0B=255;
                  usart.cnt_rx = 0;
                  return 2;
                  break;
          default:
                  usart.cnt_rx = 0;
                  break;
        }
      }
    }
  }
  return 0;
}

void send_string(char c[]){
  if (usart.tx_flag !=BUSY && usart.rx_flag != BUSY){
    for (uint8_t i = 0; i < strlen(c); i++){
      usart.tx_buffer[usart.cnt_tx] = c[i];
      usart.cnt_tx++;
    }
    enable_uart_transmission();
    usart.tx_flag = BUSY;
    usart.cnt_tx = 0;
    UDR0 = usart.tx_buffer[usart.cnt_tx];
  }
}

static void enable_uart_transmission(void){
  UCSR0B  |= (1<<TXCIE0);
  UCSR0B  &=~ (1<<RXCIE0);
}

static void enable_uart_reception(void){
  UCSR0B  &=~ (1<<TXCIE0);
  UCSR0B  |= (1<<RXCIE0);
}




/*----------------------------------
            old and Examples
-----------------------------------*/

// uint8_t USART_Receive(void)
// {
//   while (!(UCSR0A & (1<<RXC0)));
//   return UDR0;
// }
//
// void USART_Transmit(uint8_t data)
// {
//   while ( !(UCSR0A & (1<<UDRE0)));
//   UDR0 = data;
// }

// void appendchar(char c){
//   usart.tx_buffer[position] = c;
//   position++;
//   if (position >= BUFFER_SIZE){
//     position = 0;
//   };
// }

// void m_write(char c[]){
//   for (uint8_t i = 0; i < strlen(c); i++){
//     appendchar(c[i]);
//   }
//   if (UCSR0A & (1 << UDRE0)){
//     UDR0 = 0;
//   }
// }
