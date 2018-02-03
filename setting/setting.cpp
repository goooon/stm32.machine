#include "./Setting.h"
#include "../api/Tracer.h"
extern "C"
{
	#include "../api/i2c.h"
}
static u32 g_MeasureFixPulse;
static u32 g_MeasureFixDist;
static s32 g_whellFixPulse;
static s32 g_whellFixDist;
static s32 g_RoundPerMin;
static u32 g_MainAxisAngleInPulse;

void Setting::initSetting(){
	u8 temp[8] = {1,2,3,4,5,6,7,8};
  u8 read[8] = {0};
	I2C1_init();
	At24c02_write(0,0,temp,8);
  delay_ms(50);
	At24c02_read(0,0,read,8);
  for(int i = 0; i < 8; ++i){
		LOG_I("%d ",read[i]);
	}
}
void Setting::setMeasureFixPulse(u32 dist,u32 count){
	 g_MeasureFixPulse = count;
	 g_MeasureFixDist = dist;
}
void  Setting::getMeasureFixPulse(u32& dist,u32& count){
	 count = g_MeasureFixPulse;
	 dist = g_MeasureFixDist;
}
	 
void Setting::setWhellFixPulse(s32 dist,s32 count){
	 g_whellFixPulse = count;
	 g_whellFixDist = dist;
}
void Setting::getWhellFixPulse(s32& dist,s32& count){
	 dist = g_whellFixDist;
	 count = g_whellFixPulse;
}

void Setting::setRoundPerMin(s32 count){
	 g_RoundPerMin = count;
}
s32 Setting::getRoundPerMin(){
	 return g_RoundPerMin;
}

void Setting::setMainAxisAngleInPulse(u32 pulse){
   g_MainAxisAngleInPulse = pulse;
}
u32 Setting::getMainAxisAngleInPulse(){ 
	return g_MainAxisAngleInPulse;
}

static bool g_male;
void Setting::setIsMale(bool male){
	g_male = male;
}
bool Setting::getIsMale(){
	return g_male;
}

static u32 g_toothCount = 4;
void Setting::setToothCount(u32 count){
	g_toothCount = count;
}
u32  Setting::getToothCount(){
	return g_toothCount;
}
static u32 g_inputs[4] = { 223,117,81,46};
u32  Setting::getInput(u32 i){
	 if(i > 3)return 0;
	 return g_inputs[i];
}
void Setting::setInput(u32 i,u32 u){
	if(i > 3)return ;
	g_inputs[i] = u;
}
u32  Setting::getPulseCountPerCircle(){
	return 1024;
}
u32 Setting::getDistMMCountPerTooth(){
	return 635;
}
sint Setting::pulseToDistMM(sint pulse){
	signed long roundDist = getDistMMCountPerTooth() * Setting::getToothCount();
	signed long dist = roundDist * pulse  / (signed long)getPulseCountPerCircle();
	return dist;
}