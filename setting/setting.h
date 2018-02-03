#ifndef ZLG_Setting_HH
#define ZLG_Setting_HH
#include "../api/lcd.h"
#define PULSE_PER_AXIS_ROUND 100
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
   static void setRoundPerMin(s32 count);
   static s32  getRoundPerMin();
	 static void setMainAxisAngleInPulse(u32 count);
   static u32  getMainAxisAngleInPulse();
   static void setIsMale(bool male);
   static bool getIsMale();
   static void setToothCount(u32 count);
   static u32  getToothCount();
   static u32  getInput(u32 i);
   static void setInput(u32 i,u32 u);
   static sint pulseToDistMM(sint pulse);
};
#endif