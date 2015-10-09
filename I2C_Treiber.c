#include "I2C_Treiber.h"
#include <stdio.h>
#define SLAVE_ADDR (MPU6050_ADDRESS_AD0_HIGH<<1)
#define TIMEOUT 2000

static I2C_HandleTypeDef hnd;

void HAL_I2C_MspInit(I2C_HandleTypeDef *hi2c)
{
  GPIO_InitTypeDef  GPIO_InitStruct;

  //Enable the GPIO-clock
  __GPIOB_CLK_ENABLE();
 
  //Enable the I2C-Clock
  __I2C1_CLK_ENABLE();
 
  //Configure GPIOB Pin_6 as SCL for I2C
  GPIO_InitStruct.Pin       = GPIO_PIN_6;
  GPIO_InitStruct.Mode      = GPIO_MODE_AF_OD;
  GPIO_InitStruct.Pull      = GPIO_PULLUP;
  GPIO_InitStruct.Speed     = GPIO_SPEED_HIGH;
  GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
  //init GPIOB PIN6 (SCL-Pin)
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
 
  //Configure GPIOB Pin_7 as SDA for USART
  GPIO_InitStruct.Pin = GPIO_PIN_7;
  GPIO_InitStruct.Alternate = GPIO_AF4_I2C1;
  //init GPIOB PIN7 (SDA-Pin)
  HAL_GPIO_Init(GPIOB, &GPIO_InitStruct);
 
  //Setup the Interruptlevel
  HAL_NVIC_SetPriority(I2C1_EV_IRQn, 0, 1);
	HAL_NVIC_SetPriority(I2C1_ER_IRQn, 0, 1);
  //enable the interrupt for USART1
  HAL_NVIC_EnableIRQ(I2C1_EV_IRQn);
	HAL_NVIC_EnableIRQ(I2C1_ER_IRQn);
}

void I2C1_Handler(void) {
	//Call Hal_I2C_EventHandler
	HAL_I2C_EV_IRQHandler(&hnd);  //Eventhandler - Dieser kümmert sich automatisch um das erzeugen von Startconditions, ACKs, NACKs, Stopbits und Stopcondition
	HAL_I2C_ER_IRQHandler(&hnd);  //Errorhandler
}

int initMPU(void){
	int initOkay = -1;
	HAL_I2C_StateTypeDef state;
	uint8_t tempByte = 13;
	uint8_t buffer[10] = {0,0,0,0,0,0,0,0,0,0};
	hnd.Instance = I2C1;
	hnd.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
	hnd.Init.ClockSpeed	= 400000;
	hnd.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
	hnd.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
	hnd.Init.DutyCycle = I2C_DUTYCYCLE_2;
	hnd.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
	hnd.Init.OwnAddress1 = 0x00;
	
	HAL_I2C_Init(&hnd);
	__HAL_I2C_ENABLE(&hnd);
	state = HAL_I2C_GetState(&hnd); 
	if(state == HAL_I2C_STATE_READY){
		initOkay = 0;
	}
	buffer[0]=MPU6050_RA_PWR_MGMT_1;
	buffer[1]=0x80;
	printf("READ: %u",SCCB_Read(MPU6050_RA_WHO_AM_I));
	printf("error: %u",HAL_I2C_GetError(&hnd));
	return initOkay;

}
uint8_t SCCB_Write(uint8_t addr, uint8_t data)
{
    uint8_t ret=0;
    uint8_t buf[2] = {0, 0};
		buf[0] = addr;
		buf[1] = data;
    //__disable_irq();
    if (HAL_I2C_Master_Transmit(&hnd, SLAVE_ADDR, buf, 2, TIMEOUT) != HAL_OK) {
        ret=0xFF;
    }
    //__enable_irq();
    return ret;
}

uint8_t SCCB_Read(uint8_t addr)
{
    uint8_t data=0;
		uint8_t res =  HAL_I2C_Master_Transmit(&hnd, SLAVE_ADDR, &addr, 1, TIMEOUT);
	printf("res: %u",res);
    //__disable_irq();
    if (res != HAL_OK) {
        data = 0xFF;
        goto error_w;
    }
    if (HAL_I2C_Master_Receive(&hnd, SLAVE_ADDR, &data, 1, TIMEOUT) != HAL_OK) {
        data = 0xFF;
    }
error_w:
    //__enable_irq();
    return data;
}