/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef GLOBALS_H
#define GLOBALS_H
#include "stm32f4xx_hal.h"

extern int16_t acceltempgyroVals[7];
extern int16_t acceltempgyroValsFiltered[7];
extern int32_t timeDiffMPU;

HAL_StatusTypeDef HAL_InitTick(uint32_t TickPriority);
void HAL_IncTick(void);

uint32_t HAL_GetTick(void);
uint32_t HAL_GetTick10u(void);
void HAL_Delay(__IO uint32_t Delay);
void HAL_Delay10u(__IO uint32_t Delay);

#endif
