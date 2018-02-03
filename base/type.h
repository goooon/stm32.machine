#ifndef API_TYPE_H
#define API_TYPE_H
#include "stm32f10x.h"
typedef signed int      sint;
typedef unsigned int    uint;

typedef signed char       s8;
typedef unsigned char     u8;
typedef short           s16;
typedef unsigned short  u16;
//typedef signed long     s32;
//typedef unsigned long   u32;

#if ME_COMPILER == ME_COMPILER_GNU
typedef long long          s64;
typedef unsigned long long u64;
#else
typedef __int64          s64;
typedef unsigned __int64 u64;
#endif
typedef u64             hash64;

typedef double          f64;
typedef float           f32;

//typedef u32          b32;
//xStaticAssert(sizeof(bool) == 4, should_be_same_size);
//typedef u8              b8;
//typedef bool            b8;

typedef s8              sbyte;
typedef u8              ubyte;
typedef s16             sword;
typedef u16             uword;
typedef s32             snumb;
typedef u32             unumb;
typedef long            slong;
typedef unsigned long   ulong;
typedef f32             realf;
typedef f64             reald;
#ifndef size_t
//typedef u32         size_t;
#endif

//typedef wchar_t         wchar;
typedef char						b32;
typedef char            achar;
typedef u32             result;
typedef const achar     cachar;
//typedef const wchar     cwchar;
typedef const achar*    castr;
//typedef const wchar*    cwstr;
typedef u32             color;
#define vTrue true
#define vFalse false
// class hex {
// 	u32 v;
// 	hex(u32 u):v(u){}
// 	hex(s32 s):v(s){}
// 	operator u32() { return v; }
// 	operator u32&() { return v; }
// 	
// };

#define ARRAY_SIZE(a) sizeof(a)/sizeof(a[0])
#endif
