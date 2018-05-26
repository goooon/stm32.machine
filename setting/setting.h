#ifndef ZLG_Setting_HH
#define ZLG_Setting_HH
#include "../api/lcd.h"
#define PULSE_PER_AXIS_ROUND 100
enum ToothType{
	None,
	Male,
	Female
};
class Setting
{
public:
	 static u32  getPulseCountPerCircle();
   static u32  getDistMMCountPerTooth();
	 static void initSetting();
	 static void setMeasureFixPulse(u32 dist,u32 count);
   static void getMeasureFixPulse(u32& dist,u32& count);
	 static void setWhellFixPulse(s32 dist,s32 count);
   static void getWhellFixPulse(s32& dist,s32& count);
   
	 static void setMainAxisAngleInPulse(u32 count);
   static u32  getMainAxisAngleInPulse();
   static void setToothCount(u32 count);
   static u32  getToothCount();
   static u32  getInput(u32 i);
   static void setInput(u32 i,u32 u);
   static void setConfigInput(u32 i,u32 v);
   static u32  getConfigInput(u32 i);
   static void setConfigToothType(ToothType ypte);
   static ToothType  getConfigToothType();
   static void saveToFlash();
   static sint pulseToDistMM(sint pulse);
	 
	 //static void setRoundPerMin(s32 count);
   static s32  getRoundPerMin();
	 static void resetRoundPerMin();
	 static void incRoundPerMinTicks();
	 static void updateRoundPerMin(u16 pulseIn4096);
	 
	 static bool needPresetParameters();
};
#endif