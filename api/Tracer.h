#ifndef __STM32F10x_PRINT_H
#define __STM32F10x_PRINT_H
#include <stdio.h>
#include <string.h>
#include <stddef.h>
#include <stdarg.h>
#include "../base/type.h"


template<size_t N>
const char* __file_name__(const char(&file)[N]) {
    const char* p = file + N;
    while (p != file) {
        p--;
        if ((*p) == '\\') {
            p++;
            break;
        }
    }
    return p;
}

#define LOG_V(fmt,arg...) do{api::tracer.printf("V:" fmt " in %s(%d)",##arg,__file_name__(__FILE__),__LINE__);api::tracer.printf("\n");}while(0)
#define LOG_I(fmt,arg...) do{api::tracer.printf("I:" fmt " in %s(%d)",##arg,__file_name__(__FILE__),__LINE__);api::tracer.printf("\n");}while(0)
#define LOG_W(fmt,arg...) do{api::tracer.printf("W:" fmt " in %s(%d)",##arg,__file_name__(__FILE__),__LINE__);api::tracer.printf("\n");}while(0)
#define LOG_E(fmt,arg...) do{api::tracer.printf("E:" fmt " in %s(%d)",##arg,__file_name__(__FILE__),__LINE__);api::tracer.printf("\n");}while(0)
#define LOG_F(fmt,arg...) do{api::tracer.printf("F:" fmt " in %s(%d)",##arg,__file_name__(__FILE__),__LINE__);api::tracer.printf("\n");}while(0)
#define LOG_A(expr,arg...) do{if(!expr){api::tracer.printf("A:"#expr" in %s(%d)",__file_name__(__FILE__),__LINE__);api::tracer.printf("\n");}}while(0)
#define LOG_P(...) api::tracer.printf(__VA_ARGS__)
#define LOG_BUFF(fmt,buff,len) LOG_P(fmt);\
for(int i = 0;i < len; ++i){ \
					 LOG_P("0x%x ",((char*)buff)[i]); \
				 } \
				 LOG_I(""); \
 
namespace api
{
class Tracer
{
public:
    Tracer& operator << (const char* string);
    Tracer& operator <<(u32 _i);
    Tracer& operator <<(s32 _i);
    void printf(char* fmt, ...);
};
extern Tracer tracer;
};
#endif //__STM32F10x_PRINT_H
