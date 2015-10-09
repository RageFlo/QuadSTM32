#ifndef __STDOUT_USART_H
#define __STDOUT_USART_H
#include <stdio.h>
#include "Driver_USART.h"

int std_init (void);
 
/**
  Put a character to the stdout
 
  \param[in]   ch  Character to output
  \return          The character written, or -1 on write error.
*/
int stdout_putchar (int ch) ;
/**
  Get a character from stdin
 
  \return     The next character from the input, or -1 on read error.
*/
int stdin_getchar (void);

int stderr_putchar (int ch);

void myUSART_callback(uint32_t event);
void kommuHandler(void);

#endif
