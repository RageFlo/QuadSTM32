/*-----------------------------------------------------------------------------
 * Name:    stdout_USART.c
 * Purpose: STDOUT USART Template
 * Rev.:    1.0.0
 *-----------------------------------------------------------------------------*/
 
/* Copyright (c) 2013 - 2015 ARM LIMITED

   All rights reserved.
   Redistribution and use in source and binary forms, with or without
   modification, are permitted provided that the following conditions are met:
   - Redistributions of source code must retain the above copyright
     notice, this list of conditions and the following disclaimer.
   - Redistributions in binary form must reproduce the above copyright
     notice, this list of conditions and the following disclaimer in the
     documentation and/or other materials provided with the distribution.
   - Neither the name of ARM nor the names of its contributors may be used
     to endorse or promote products derived from this software without
     specific prior written permission.
   *
   THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
   AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
   IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
   ARE DISCLAIMED. IN NO EVENT SHALL COPYRIGHT HOLDERS AND CONTRIBUTORS BE
   LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
   CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
   SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
   INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
   CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
   ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
   POSSIBILITY OF SUCH DAMAGE.
   ---------------------------------------------------------------------------*/
 

#include "stdout_USART.h"
#include "globals.h"
//-------- <<< Use Configuration Wizard in Context Menu >>> --------------------
 
// <h>STDOUT USART Interface
 
//   <o>Connect to hardware via Driver_USART# <0-255>
//   <i>Select driver control block for USART interface
#define USART_DRV_NUM           3
 
//   <o>Baudrate
#define USART_BAUDRATE          57600
 
// </h>
 
 
#define _USART_Driver_(n)  Driver_USART##n
#define  USART_Driver_(n) _USART_Driver_(n)
 
extern ARM_DRIVER_USART  USART_Driver_(USART_DRV_NUM);
#define ptrUSART       (&USART_Driver_(USART_DRV_NUM))
 
#define BUFFERSIZE_IN 100
#define BUFFERSIZE_COM 10
 
static uint8_t input_buf[BUFFERSIZE_IN];
static uint8_t command[BUFFERSIZE_COM];
static uint8_t kommuConnected = 0;
static uint8_t kommuNoPing = 0;
static const uint8_t maxPingsMissed = 40;



int buildCommand(uint8_t* buffer, uint8_t* command){
	static int currentPos = 0;
	static int commandPos = 0;
	static uint8_t reciving = 0;
	uint8_t currentChar = 0;
	static uint8_t gotStart = 0;
	uint8_t done = 0;
	int i;
	int recCount = ptrUSART->GetRxCount();
	while((recCount > currentPos || currentPos > recCount) & !done){
		currentChar = buffer[currentPos];
		if(currentChar == 0x02){
			gotStart = 1;
			for( i = 0; i < BUFFERSIZE_COM; i++){
				command[i] = 0;
			}
			commandPos = 0;
		}else if(!gotStart){
			//out of message
		}else if(currentChar == 0x03){
			gotStart = 0;
			if(commandPos > 0)
				done = 1;
		}else if(commandPos < BUFFERSIZE_COM){
			command[commandPos] = currentChar;
			commandPos++;
		}else{
			puts("command not closed :(");
		}
		currentPos++;
		if(currentPos >= 100){
			currentPos = 0;
		}
	}
	
	if(!reciving){
		startRec(buffer);
		reciving = 1;
	}		
	return done;
}

int sendCommand(char * toSend,int lenght){
	//char buffer[lenght+2];
	int i;
	stdout_putchar(0x02);
	for(i = 0; i < lenght; i++){
		stdout_putchar(toSend[i]);
	}
	stdout_putchar(0x03);
	
	return 0;
}
void myUSART_callback(uint32_t event)
{
	switch (event)
	{
	case ARM_USART_EVENT_RECEIVE_COMPLETE: 
	startRec(input_buf);
	break;
	case ARM_USART_EVENT_TRANSFER_COMPLETE:
	case ARM_USART_EVENT_SEND_COMPLETE:
	case ARM_USART_EVENT_TX_COMPLETE:
	break;

	case ARM_USART_EVENT_RX_TIMEOUT:
	break;

	case ARM_USART_EVENT_RX_OVERFLOW:
		puts("Lost data");
	break;
	case ARM_USART_EVENT_TX_UNDERFLOW:
	break;
	}
}

void kommuHandler(void){
	int i;
	int16_t smallMPU;
	char buffer[4] = {0,0,0,0};
	while(buildCommand(input_buf,command)){
		switch(command[0]){
		case 'a':
			if(kommuConnected){
				sendCommand("a",1);
				kommuNoPing = 0;
			}
		break;
		case 'b':
			sendCommand("c",1);
			kommuConnected = 1;
			kommuNoPing = 0;
			HAL_GPIO_WritePin(GPIOD,GPIO_PIN_12,GPIO_PIN_SET);
		break;
		}		
	}
	if(kommuConnected){
		for(i = 0; i < 7; i++){
			buffer[0]='v';
			buffer[1]=i;
			buffer[2]=(uint8_t)(acceltempgyroValsFiltered[i]>>8);
			buffer[3]=(uint8_t)(acceltempgyroValsFiltered[i]);
			sendCommand(buffer,4);
		}
	}
	if(kommuNoPing > maxPingsMissed){ //Max ping loss?
		kommuConnected = 0;
		HAL_GPIO_WritePin(GPIOD,GPIO_PIN_12,GPIO_PIN_RESET);
	}else{
		kommuNoPing++;
	}		
}

int startRec(uint8_t* buffer){
	if (ptrUSART->Receive(buffer, BUFFERSIZE_IN) != ARM_DRIVER_OK) {
    return (-1);
  }
	return 0;
}

/**
  Initialize stdout
 
  \return          0 on success, or -1 on error.
*/
int std_init (void) {
  int32_t status;
 
  status = ptrUSART->Initialize(myUSART_callback);
  if (status != ARM_DRIVER_OK) return (-1);
 
  status = ptrUSART->PowerControl(ARM_POWER_FULL);
  if (status != ARM_DRIVER_OK) return (-1);
 
  status = ptrUSART->Control(ARM_USART_MODE_ASYNCHRONOUS |
                             ARM_USART_DATA_BITS_8       |
                             ARM_USART_PARITY_NONE       |
                             ARM_USART_STOP_BITS_1       |
                             ARM_USART_FLOW_CONTROL_NONE,
                             USART_BAUDRATE);
  if (status != ARM_DRIVER_OK) return (-1);

  status = ptrUSART->Control(ARM_USART_CONTROL_TX , 1);
	status = ptrUSART->Control(ARM_USART_CONTROL_RX , 1);
  if (status != ARM_DRIVER_OK) return (-1);

  return (0);
}
 
/**
  Put a character to the stdout
 
  \param[in]   ch  Character to output
  \return          The character written, or -1 on write error.
*/
int stdout_putchar (int ch) {
  uint8_t buf[1];
 
  buf[0] = ch;
  if (ptrUSART->Send(buf, 1) != ARM_DRIVER_OK) {
    return (-1);
  }
  while (ptrUSART->GetTxCount() != 1);
  return (ch);
}

/**
  Get a character from stdin
 
  \return     The next character from the input, or -1 on read error.
*/
int stdin_getchar (void) {
  uint8_t buf[1];
 
  if (ptrUSART->Receive(buf, 1) != ARM_DRIVER_OK) {
    return (-1);
  }
  while (ptrUSART->GetRxCount() != 1);
  return (buf[0]);
}

int stderr_putchar (int ch){
	return stdout_putchar(ch);
}
