#ifndef UART_H
#define UART_H

#include <stdint.h>

void UART_Init(void);
void UART_Write(char data);
void UART_Write_Text(const char* text);

#endif // UART_H
