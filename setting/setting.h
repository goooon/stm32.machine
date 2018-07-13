#ifndef ZLG_Setting_HH
#define ZLG_Setting_HH
#include "../api/lcd.h"

enum ToothType{
	None,
	Male,
	Female
};
class Setting
{
public:
    static s32  getPulseCountPerCircle();
    static s32  getDistUMCountPerTooth();
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

    static void setBaseConfigInput(u32 i,u32 v,s32 curDegree);
    static void setDefaultBaseConfigInputIndex(u32 defautIndex);
    static u32  getDefaultBaseConfigInputIndex();
    static u32  getBaseConfigInput(u32 i,s32& curDegree);

    static void setConfigToothType(ToothType ypte);
    static ToothType  getConfigToothType();
    static void saveToFlash();
		static signed long distUMToPulse(signed long distUm);
    static signed long pulseToDistUM(signed long pulse);

    //static void setRoundPerMin(s32 count);
    static s32  getRoundPerMin();
    static void resetRoundPerMin();
    static void incRoundPerMinTicks();
    static u32  getRoundPerMinTicks();
    static void updateRoundPerMin(u32 ticks, u16 pulseIn4096);

    static bool needPresetParameters();
    static void setNeedPresetParameters(bool need);
    static u32  calcDegreePulse(u32 distMM);
    static s32  getPrecision();

    static s32  calculateDelayedPulse(s32 baseDegreeInPulse,s32 baseDist,s32 measureDegreeInPulses,s32 measureDist);
		
		static s32  calcDistToZero(u32 distMM,s32 currAxisPulses);
};
#endif