#include "globals.h"

int16_t acceltempgyroVals[7] = {0,0,0,0,0,0,0};
int16_t acceltempgyroValsFiltered[7] = {0,0,0,0,0,0,0};
int32_t timeDiffMPU = 0;

 int32_t angleGyro[3] = {0,0,0};
 int32_t angleAccel[3] = {0,0,0};

static __IO uint32_t uwTick = 0;
static __IO uint32_t uwTick10u = 0;

HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority)
{
  /*Configure the SysTick to have interrupt in 10us time basis*/
  HAL_SYSTICK_Config(HAL_RCC_GetHCLKFreq()/100000);

  /*Configure the SysTick IRQ priority */
  HAL_NVIC_SetPriority(SysTick_IRQn, TickPriority/2 ,0);

  /* Return function status */
  return HAL_OK;
}


void HAL_IncTick(void)
{
  uwTick10u++;
  uwTick = uwTick10u / 100;
}

uint32_t HAL_GetTick(void)
{
  return uwTick;
}

uint32_t HAL_GetTick10u(void)
{
	return uwTick10u;
}

void HAL_Delay(__IO uint32_t Delay)
{
  uint32_t tickstart = 0;
  tickstart = HAL_GetTick();
  while((HAL_GetTick() - tickstart) < Delay)
  {
  }
}

void HAL_Delay10u(__IO uint32_t Delay)
{
  uint32_t tickstart = 0;
  tickstart = HAL_GetTick10u();
  while((HAL_GetTick10u() - tickstart) < Delay)
  {
  }
}

