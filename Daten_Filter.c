#include "Daten_Filter.h"
#include "globals.h"

static uint8_t gettingGyroOffset;
static int32_t gyroOffsetSamples = 0;
static int32_t gyroOffsets[3] = {0,0,0};


void filterMain(void){
	int i;
	int32_t temp = 0;
	
	temp = acceltempgyroVals[2] + 3670; // DYNMIC??
	if(temp > 32767)
				temp = 32767;
	else if(temp < -32767)
				temp = - 32767;
	acceltempgyroVals[2] = temp;
	
	for(i = 0; i < 4; i++){
		temp = acceltempgyroValsFiltered[i]*10 + acceltempgyroVals[i];
		acceltempgyroValsFiltered[i] = temp/11;
	}
	
	angleAccel[0] =  getFastXYAngle(acceltempgyroVals[2],acceltempgyroVals[1]);
	angleAccel[1] =  getFastXYAngle(acceltempgyroVals[2],acceltempgyroVals[0]);
	angleAccel[2] =  getFastXYAngle(acceltempgyroVals[0],acceltempgyroVals[1]);
	
	if(gettingGyroOffset){
		for(i = 4; i < 7; i++){
			temp = acceltempgyroValsFiltered[i]*10 + acceltempgyroVals[i];
			acceltempgyroValsFiltered[i] = temp/11;
		}
		gyroOffsetSamples++;
		for(i = 0; i < 3; i++){
			gyroOffsets[i] += acceltempgyroValsFiltered[i+4];
		}
		if(gyroOffsetSamples>2000)
			Get_Gyro_Offset_Stopp();
	}else{
		for(i = 4; i < 7; i++){
			temp = acceltempgyroVals[i] - gyroOffsets[i-4];
			if(temp > 32767)
				temp = 32767;
			else if(temp < -32767)
				temp = - 32767;
			acceltempgyroValsFiltered[i] = temp;
			angleGyro[i-4] += acceltempgyroValsFiltered[i];
		}
	}		
}

int Get_angle_from_accle(void){
	int state = 0;
	return state;
}

void Get_Gyro_Offset_Start(void){
	if(!gettingGyroOffset){
		gettingGyroOffset = 1;
		gyroOffsetSamples = 0;
	}
}
void Get_Gyro_Offset_Stopp(void){
	if(gettingGyroOffset){
		gettingGyroOffset = 0;
		gyroOffsets[0] /= gyroOffsetSamples;
		gyroOffsets[1] /= gyroOffsetSamples;
		gyroOffsets[2] /= gyroOffsetSamples;
	}
}

unsigned int getFastXYAngle(int x, int y){
   // Fast XY vector to integer degree algorithm - Jan 2011 www.RomanBlack.com
   // Converts any XY values including 0 to a degree value that should be
   // within +/- 1 degree of the accurate value without needing
   // large slow trig functions like ArcTan() or ArcCos().
   // NOTE! at least one of the X or Y values must be non-zero!
   // This is the full version, for all 4 quadrants and will generate
   // the angle in integer degrees from 0-360.
   // Any values of X and Y are usable including negative values provided
   // they are between -1456 and 1456 so the 16bit multiply does not overflow.

   unsigned char negflag;
   unsigned char tempdegree;
   unsigned char comp;
   unsigned int degree;     // this will hold the result
   unsigned int ux;
   unsigned int uy;

   // Save the sign flags then remove signs and get XY as unsigned ints
   negflag = 0;
   if(x < 0)
   {
      negflag += 0x01;    // x flag bit
      x = (0 - x);        // is now +
   }
   ux = x;                // copy to unsigned var before multiply
   if(y < 0)
   {
      negflag += 0x02;    // y flag bit
      y = (0 - y);        // is now +
   }
   uy = y;                // copy to unsigned var before multiply

   // 1. Calc the scaled "degrees"
   if(ux > uy)
   {
      degree = (uy * 45) / ux;   // degree result will be 0-45 range
      negflag += 0x10;    // octant flag bit
   }
   else
   {
      degree = (ux * 45) / uy;   // degree result will be 0-45 range
   }

   // 2. Compensate for the 4 degree error curve
   comp = 0;
   tempdegree = degree;    // use an unsigned char for speed!
   if(tempdegree > 22)      // if top half of range
   {
      if(tempdegree <= 44) comp++;
      if(tempdegree <= 41) comp++;
      if(tempdegree <= 37) comp++;
      if(tempdegree <= 32) comp++;  // max is 4 degrees compensated
   }
   else    // else is lower half of range
   {
      if(tempdegree >= 2) comp++;
      if(tempdegree >= 6) comp++;
      if(tempdegree >= 10) comp++;
      if(tempdegree >= 15) comp++;  // max is 4 degrees compensated
   }
   degree += comp;   // degree is now accurate to +/- 1 degree!

   // Invert degree if it was X>Y octant, makes 0-45 into 90-45
   if(negflag & 0x10) degree = (90 - degree);

   // 3. Degree is now 0-90 range for this quadrant,
   // need to invert it for whichever quadrant it was in
   if(negflag & 0x02)   // if -Y
   {
      if(negflag & 0x01)   // if -Y -X
            degree = (180 + degree);
      else        // else is -Y +X
            degree = (180 - degree);
   }
   else    // else is +Y
   {
      if(negflag & 0x01)   // if +Y -X
            degree = (360 - degree);
   }
	 return degree;
 }
