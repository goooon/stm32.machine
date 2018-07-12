#include "./Setting.h"
#include "../api/Tracer.h"
extern "C"
{
	#include "../api/i2c.h"
}
#define PULSE_COUNT_PER_ROUND_FROM_FPGA 4096

static u32 g_MeasureFixPulse;
static u32 g_MeasureFixDist;
static s32 g_whellFixPulse;
static s32 g_whellFixDist;
static s32 g_RoundPerMinute;
static u32 g_MainAxisAngleInPulse;
static u32 g_MainAxisRotateCount;
struct Config{
	  u32 magicCode;//0xbeefbeef,0 means not configured
	  u8 toothDist;//625mm, 0 means not configured
	  u8 type;//1:male 2: female,  0 means not configured
	  u8 toothCount;//0 means not configured
	  u8 reserved; //0
	  u32 input[4];// 0 means not configured
	  s32 currMainAxisInPulse[4];
	  u32 defaultBaseInput;//index for input[], which is currently selected
	  u8 reserved2[3];
	  u8 checkSum;// byte sum of all the data above
};
static Config config;
static bool needPreset;
void Setting::initSetting(){
	bool valid = false;
	needPreset = false;
	I2C1_init();
	delay_ms(50);
	config.magicCode = 0;
	config.defaultBaseInput = -1;
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
	//needPreset = true;
}
bool Setting::needPresetParameters(){
	  return needPreset;
}
void Setting::setNeedPresetParameters(bool need){
	  needPreset = need;
}
void Setting::saveToFlash(){
	 u8* end = (u8*)&config + sizeof(config) - 1;
	 u8 sum = 0;
	 for(u8* p = (u8*)&config; p != end; ++ p){
			sum += *p;
	 }
	 config.checkSum = sum;
	 At24c02_write(0,0,(u8*)&config,sizeof(config));
	 needPreset = false;
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
	 if(i > 3){
		 LOG_E("wrong getInput(%d)\r\n",i);
		 return 0;
	 }
	 return g_inputs[i];
}
void Setting::setBaseConfigInput(u32 i,u32 v,s32 curDegree){
	 if(i > 3){
		  LOG_E("wrong setBaseConfigInput(%d)\r\n",i);
		 return ;
	 }
	 config.input[i] = v;
	 config.currMainAxisInPulse[i] = curDegree;
}
void Setting::setDefaultBaseConfigInputIndex(u32 i)
{
	 config.defaultBaseInput = i > 3 ? 0 : i;
}
u32  Setting::getDefaultBaseConfigInputIndex(){
	return config.defaultBaseInput;
}
u32  Setting::getBaseConfigInput(u32 i,s32& curDegree){
	 curDegree = config.currMainAxisInPulse[0];
	 if(i > 3){
		  LOG_E("wrong getBaseConfigInput(%d)\r\n",i);
		  return 0;
	 }
	 curDegree = config.currMainAxisInPulse[i];
	 return config.input[i];
}
void Setting::setInput(u32 i,u32 u){
	if(i > 3){
		return ;
		LOG_E("wrong setInput(%d)\r\n",i);
	}
	g_inputs[i] = u;
}
s32  Setting::getPulseCountPerCircle(){
	return 1024;
}
s32 Setting::getDistUMCountPerTooth(){
	return 6350;
}
s32  Setting::getPrecision(){
	return 1000;
}
void Setting::setConfigToothType(ToothType male){
	config.type = (u8)male;
}
ToothType Setting::getConfigToothType(){
	return (ToothType)config.type;
}
signed long Setting::pulseToDistUM(signed long pulse){
	//signed long roundDist = getDistUMCountPerTooth() * Setting::getToothCount();
	signed long dist = getDistUMCountPerTooth() * pulse  / (signed long)getPulseCountPerCircle();
	return dist;
}
signed long Setting::distUMToPulse(signed long distUm){
	signed long ret = (signed long)getPulseCountPerCircle() * (signed long)distUm/getDistUMCountPerTooth();
	return ret;
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
	 return g_RoundPerMinute;
}
void Setting::updateRoundPerMin(u32 ticks,u16 pulseIn4096){
	    #define COUNT_PER_SECOUND  50  //0.02s per call for Timer7
		#define SECOUND_PER_MINUTE 60
		#define PULSE_COUNT_PER_MINUTE(dltPulse) (dltPulse * COUNT_PER_SECOUND * SECOUND_PER_MINUTE)
		axisDegreeQueue[0] = axisDegreeQueue[1];
		axisDegreeQueue[1] = axisDegreeQueue[2];
		axisDegreeQueue[2] = pulseIn4096;
		axisTicksQueue[0] = axisTicksQueue[1];
		axisTicksQueue[1] = axisTicksQueue[2];
		axisTicksQueue[2] = ticks;
	   //LOG_I("%d:%d",pulseIn4096,ticks);
	   if(axisDegreeQueue[0] <= axisDegreeQueue[1] &&
			axisDegreeQueue[1] <= axisDegreeQueue[2]){//check direction
			u32 dltPulse = axisDegreeQueue[2] - axisDegreeQueue[0];
			u32 dltTime = axisTicksQueue[2] - axisTicksQueue[0];
			g_RoundPerMinute = PULSE_COUNT_PER_MINUTE(dltPulse)/ dltTime / PULSE_COUNT_PER_ROUND_FROM_FPGA;
		}
		else if(axisDegreeQueue[0] >= axisDegreeQueue[1] &&
			axisDegreeQueue[1] >= axisDegreeQueue[2]){//check direction
			u32 dltPulse = axisDegreeQueue[0] - axisDegreeQueue[2];
			u32 dltTime = axisTicksQueue[2] - axisTicksQueue[0];
			g_RoundPerMinute = PULSE_COUNT_PER_MINUTE(dltPulse)/ dltTime / PULSE_COUNT_PER_ROUND_FROM_FPGA;
		}
}
u32 Setting::calcDegreePulse(u32 distMM){
	u32 roundPulse = Setting::getPulseCountPerCircle();
	u32 distPerTooth = getDistUMCountPerTooth();
	u32 pulseDegree = Setting::getMainAxisAngleInPulse();
	
	int result;
	int fixedForToothPulse = 0;
	int x = distPerTooth * pulseDegree / roundPulse;
	int releativeDist = distMM + x;
	//while(releativeDist < 0) releativeDist += distPerTooth;

	fixedForToothPulse = roundPulse * releativeDist / distPerTooth;
	result = fixedForToothPulse % roundPulse;
	LOG_I("dist:%d,axis:%d, result: %d",distMM,pulseDegree,result);
	return result;
}