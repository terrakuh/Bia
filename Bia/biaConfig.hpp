#pragma once

#ifdef _WIN32

#else
#define BIA_INSTRUCTION_CALLING_CONVETION __attribute__ ((cdecl))
#endif

#if defined(_MSC_VER)

#define BIA_INSTRUCTION_CALLING_CONVETION(x, y) x __cdecl y
#define BIA_MEMBER_CALLING_CONVENTION __thiscall
#define BIA_STATIC_CALLING_CONEVENTION __cdecl
#define BIA_COMPILER_MSCV 1

#if defined(_M_IX86)
#define BIA_ARCHITECTURE_MG32 1
#elif defined(_M_X64)

#endif

#endif


#define BIA_MAX_TEMP_ADDRESSES 2147483648