#include "./Tool.h"
#include "./Tracer.h"
int tool::convertFixed(s32 numb,s32 digit,short unicode[],uint size,bool showPlus,short ending)
{
	  int i;
	  if(numb < 0){unicode[0] = 0x2d; i = 1; numb = -numb;}
		else if(showPlus && numb > 0){unicode[0] = 0x2b; i = 1;}
		else i = 0;
     unsigned long times = 1000000000;
		 if(numb != 0){
			 while(numb / times == 0){
				  if(times == digit){
						 unicode[i] = '0';
						 i++;
						 unicode[i] = '.';
						 i++;
						 times /= 10;
						 break;
					}
					times /= 10;
			 }
			 while(times != 0){
				 unicode[i] = numb / times + '0';
				 i++;
				 numb %= times;
				 if(times == digit){
					 unicode[i] = '.';
					 i++;
				 }
				 times /= 10;
			 }
		 }
		 else{
			 unicode[i++] = '0';
			 if(digit){
				 unicode[i++] = '.';
			 }
			 while(digit /= 10)
			 {
				 unicode[i++] = '0';
			 }
		 }
		 unicode[i] = ending;
		 i++; 
		 return i;
}
//p/c = o/360; o = 360 * p / c;
int tool::convertPulseToAngle(u32 pulse,u32 circulePulses,short unicode[],uint size){
	  pulse %= circulePulses;
	  u32 c = pulse * 360 * 60 * 60;
	  u32 out = c / circulePulses;
	  //LOG_I("total degree %d out %d",c,out);
	  u32 s = out % 60;
	  u32 m = (out / 60 ) % 60;
	  u32 d = out / 3600;
	  int total = 0;
	  int r = convertFixed(d,0,unicode,size,false,0x00B0);
	  total += r;
	  unicode += r;
	  size -= r;
		r = convertFixed(m,0,unicode,size,false,0x2032);
	  total += r;
	  unicode += r;
	  size -= r;
	  r = convertFixed(s,0,unicode,size,false,0x2033);
	  unicode[r] = 0xffff;
	  r++;
	  //LOG_I("return total %d",total + r);
	  return total + r;
}

