/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef DATEN_FILTER_H
#define DATEN_FILTER_H
#include "stm32f4xx_hal.h"

void lowPassFilterGyro(void);

void Get_Gyro_Offset_Start(void);
void Get_Gyro_Offset_Stopp(void);
#endif
