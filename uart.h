#ifndef _UART_H
#define _UART_H

extern char UART5_RX_BUF[100];
extern uint8_t Serial_Flag5;


void UART5_Init(void);
void USART5_SendBits(uint8_t data);
void UART5_SendArray(uint8_t *array,uint8_t length);
void UART5_SendString(uint8_t *String1);
uint8_t Serial5_GetRxFlag(void);
void  UART5_IRQHandler(void);
void UART5_Start_Scan(void);
void UART5_ParseCode(const char *Buf, int16_t *code1, int16_t *code2);



#endif
