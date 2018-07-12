#include "Tracer.h"
extern "C"{
	#include "../sys/usart.h"
}
using namespace api;
namespace api{
	Tracer tracer;
}
Tracer& Tracer::operator << (const char* string)
		{
			UART3_SendString(const_cast<char*>(string));
			return *this;
		}
		Tracer& Tracer::operator <<(u32 _i) {
			achar str[20];
			s32 i = 18, v = _i;
			u32 r;
			str[19] = 0;
			do
			{
				r = v % 10;
				str[i--] = r + '0';
				v /= 10;
			} while (v != 0);
			return operator<<(&str[i + 1]);
		}
		Tracer& Tracer::operator <<(s32 _i) {
			achar str[20];
			s32 i = 18, v = _i;
			u32 r;
			if (v < 0) { v = -v; }

			str[19] = 0;
			do
			{
				r = v % 10;
				str[i--] = r + '0';
				v /= 10;
			} while (v != 0);

			if (_i < 0)str[i--] = '-';
			return operator<<(&str[i + 1]);
		}
// 		Tracer& operator << (hex h) {
// 			return operator<<(h.v);
// 		}
		void Tracer::printf(char* fmt, ...)
		{
			char buff[512];
			va_list ap;
			va_start(ap, fmt);
			vsprintf((char*)buff, fmt, ap);
			va_end(ap);
			UART3_SendString(buff);
		}