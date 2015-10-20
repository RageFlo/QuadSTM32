#include "Daten_Filter.h"
#include "globals.h"

static uint8_t gettingGyroOffset;
static int32_t gyroOffsetSamples = 0;
static int32_t gyroOffsets[3] = {0,0,0};


void lowPassFilterGyro(void){
	int i;
	int32_t temp = 0;
	for(i = 0; i < 4; i++){
		temp = acceltempgyroValsFiltered[i]*1 + acceltempgyroVals[i];
		acceltempgyroValsFiltered[i] = temp/2;
	}
	
	if(gettingGyroOffset && gyroOffsetSamples<1000){
		for(i = 4; i < 7; i++){
			temp = acceltempgyroValsFiltered[i]*1 + acceltempgyroVals[i];
			acceltempgyroValsFiltered[i] = temp/2;
		}
		gyroOffsetSamples++;
		for(i = 0; i < 3; i++){
			gyroOffsets[i] += acceltempgyroValsFiltered[i+4];
		}
	}else{
		for(i = 4; i < 7; i++){
			temp = acceltempgyroValsFiltered[i]*1 + acceltempgyroVals[i] - gyroOffsets[i-4];
			acceltempgyroValsFiltered[i] = temp/2;
			angleGyro[i-4] += acceltempgyroValsFiltered[i];
		}
	}		
}

void Get_Gyro_Offset_Start(void){
	gettingGyroOffset = 1;
	gyroOffsetSamples = 0;
}
void Get_Gyro_Offset_Stopp(void){
	gettingGyroOffset = 0;
	gyroOffsets[0] /= gyroOffsetSamples;
	gyroOffsets[1] /= gyroOffsetSamples;
	gyroOffsets[2] /= gyroOffsetSamples;
}
