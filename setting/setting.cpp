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
static u32 g_MainAxisRotateCount;
struct Config{
	  u32 magicCode;//0xbeefbeef,0 means not configured
	  u8 toothDist;//625mm, 0 means not configured
	  u8 type;//1:male 2: female,  0 means not configured
	  u8 toothCount;//ÑÀÊý, 0 means not configured
	  u8 reserved; //0
	  u32 input[4];// 0 means not configured
	  u8 reserved2[3];
	  u8 checkSum;// byte sum of all the data above
};
static Config config;
static bool needPreset;
void Setting::initSetting(){
	
	//I2C2_Config();
	//I2C_PageWrite_AT24C01A_02(0,0,(u8*)&config,sizeof(config));
	/*u8 temp[8] = {1,2,3,4,5,6,7,8};
  u8 read[8] = {0};
	I2C1_init();
	At24c02_write(0,0,temp,8);
  delay_ms(50);
	At24c02_read(0,0,read,8);
  for(int i = 0; i < 8; ++i){
		LOG_I("%d ",read[i]);
	}*/
	bool valid = false;
	needPreset = false;
	I2C1_init();
	delay_ms(50);
	config.magicCode = 0;
	//At24c02_write(0,0,(u8*)&config,sizeof(config));
	At24c02_read(0,0,(u8*)&config,sizeof(config));
	if(config.magicCode == 0xbeefbeef){
		 u8* end = (u8*)&config + sizeof(config) - 1;
		 u8 sum = 0;
		 for(u8* p = (u8*)&config; p != end; ++ p){
			  sum += *p;
		 }
		 if(sum == config.checkSum){
			 valid = true;
		 }
	}
	if(!valid){
		 LOG_I("config is not valid");
		 u8* end = (u8*)&config + sizeof(config);
		 u8 sum = 0;
		 for(u8* p = (u8*)&config; p != end; ++ p){
			  *p = 0;
		 }
		 At24c02_write(0,0,(u8*)&config,sizeof(config));
		 config.magicCode = 0xbeefbeef;
		 needPreset = true;
	}
}
bool Setting::needPresetParameters(){
	  return needPreset;
}
void Setting::saveToFlash(){
	 u8* end = (u8*)&config + sizeof(config) - 1;
	 u8 sum = 0;
	 for(u8* p = (u8*)&config; p != end; ++ p){
			sum += *p;
	 }
	 config.checkSum = sum;
	 At24c02_write(0,0,(u8*)&config,sizeof(config));
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
void Setting::setMainAxisAngleInPulse(u32 pulse){
   g_MainAxisAngleInPulse = pulse;
}
u32 Setting::getMainAxisAngleInPulse(){ 
	return g_MainAxisAngleInPulse;
}
void Setting::setToothCount(u32 count){
	config.toothCount = count;
}
u32  Setting::getToothCount(){
	return config.toothCount;
}
static u32 g_inputs[4] = { 223,117,81,46};
u32  Setting::getInput(u32 i){
	 if(i > 3)return 0;
	 return g_inputs[i];
}
void Setting::setConfigInput(u32 i,u32 v){
	 if(i > 3)return ;
	 config.input[i] = v;
}
u32  Setting::getConfigInput(u32 i){
	 if(i > 3)return 0;
	 return config.input[i];
}
void Setting::setInput(u32 i,u32 u){
	if(i > 3)return ;
	g_inputs[i] = u;
}
u32  Setting::getPulseCountPerCircle(){
	return 1024;
}
u32 Setting::getDistMMCountPerTooth(){
	return 625;
}
void Setting::setConfigToothType(ToothType male){
	config.type = (u8)male;
}
ToothType Setting::getConfigToothType(){
	return (ToothType)config.type;
}
sint Setting::pulseToDistMM(sint pulse){
	signed long roundDist = getDistMMCountPerTooth() * Setting::getToothCount();
	signed long dist = roundDist * pulse  / (signed long)getPulseCountPerCircle();
	return dist;
}
static s32 axisDegreeQueue[3];
static u32 axisTicksQueue[3];
static u32 axisRoundTicks;
void Setting::resetRoundPerMin(){
	axisDegreeQueue[0] = 0;
	axisDegreeQueue[1] = 0;
	axisDegreeQueue[2] = 0;
	axisTicksQueue[0] = 0;
	axisTicksQueue[1] = 0;
	axisTicksQueue[2] = 0;
	axisRoundTicks = 0;
}
void Setting::incRoundPerMinTicks(){
	axisRoundTicks ++;
}
u32 Setting::getRoundPerMinTicks(){
	return axisRoundTicks;
}
s32 Setting::getRoundPerMin(){
	 return g_RoundPerMin;
}
void Setting::updateRoundPerMin(u32 ticks,u16 pulseIn4096){
	   axisDegreeQueue[0] = axisDegreeQueue[1];
		 axisDegreeQueue[1] = axisDegreeQueue[2];
		 axisDegreeQueue[2] = pulseIn4096;
		 axisTicksQueue[0] = axisTicksQueue[1];
		 axisTicksQueue[1] = axisTicksQueue[2];
		 axisTicksQueue[2] = ticks;
	   //LOG_I("%d:%d",pulseIn4096,ticks);
	   if(axisDegreeQueue[0] < axisDegreeQueue[1] &&
			  axisDegreeQueue[1] < axisDegreeQueue[2]){
					u32 dltPulse = axisDegreeQueue[2] - axisDegreeQueue[0];
					u32 dltTime = axisTicksQueue[2] - axisTicksQueue[0];
					g_RoundPerMin = dltPulse * 50 * 60 / dltTime / 4096;
			}
		 else if(
				axisDegreeQueue[0] > axisDegreeQueue[1] &&
			  axisDegreeQueue[1] > axisDegreeQueue[2]){
					u32 dltPulse = axisDegreeQueue[0] - axisDegreeQueue[2];
					u32 dltTime = axisTicksQueue[0] - axisTicksQueue[2];
					g_RoundPerMin = dltPulse * 50 * 60 / dltTime / 4096;
			}
}