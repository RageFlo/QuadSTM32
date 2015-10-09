#ifndef __STDOUT_USART_H
#define __STDOUT_USART_H
#include <stdio.h>
#include "Driver_USART.h"



void myUSART_callback(uint32_t event);
void kommuHandler(void);
int startRec(uint8_t* buffer);

int std_init (void);
int stdout_putchar (int ch) ;
int stdin_getchar (void);

int stderr_putchar (int ch);
#endif
