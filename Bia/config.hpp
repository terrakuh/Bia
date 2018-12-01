#pragma once

#include "config_cmake.hpp"

// Compiler macro
#if defined(_MSC_VER)
#define BIA_COMPILER_MSVC 1
#elif defined(__GNUC__) || defined(__GNUG__)
#define BIA_COMPILER_GNU 1
#elif defined(__clang__)
#define BIA_COMPILER_CLANG 1
//#elif defined(__ICC) || defined(__INTEL_COMPILER)
//#elif defined(__IBMC__) || defined(__IBMCPP__)
//#elif defined(__SUNPRO_C)  || defined(__SUNPRO_CC)
#else
#error "Unsupported compiler."
#endif

//Operating System
#if defined(_WIN32) || defined(_WIN64) || defined(__CYGWIN__)
#define BIA_OS_WINDOWS 1
#elif defined(__linux__)
#define BIA_OS_LINUX 1
#else
#error "Unsupported operating system."
#endif

// Architecture
#if defined(__i386) || defined(_M_IX86)
#define BIA_ARCHITECTURE_X86_32 1
#elif defined(__x86_64__) || defined(_M_X64)
#define BIA_ARCHITECTURE_X86_64 1
#else
#error "Unsupported architecture."
#endif

// Microsoft
#if defined(BIA_COMPILER_MSVC)

// Calling conventions
#if defined(BIA_ARCHITECTURE_X86_32)
#define BIA_STATIC_CALLING_CONVETION __fastcall
#define BIA_MEMBER_CALLING_CONVENTION __fastcall
#define BIA_VARG_MEMBER_CALLING_CONVENTION __cdecl
#elif defined(BIA_ARCHITECTURE_X86_64)
#define BIA_STATIC_CALLING_CONVETION
#define BIA_MEMBER_CALLING_CONVENTION
#define BIA_VARG_MEMBER_CALLING_CONVENTION
#endif

// Export
#if defined(BIA_BUILD_SHARED) && defined(BIA_BUILDING)
#define BIA_EXPORT __declspec(dllexport)
#elif defined(BIA_BUILD_SHARED)
#define BIA_EXPORT __declspec(dllimport)
#else
#define BIA_EXPORT
#endif

// GNU and Clang
#elif defined(BIA_COMPILER_GNU) || defined(BIA_COMPILER_CLANG)

// Calling conventions
#if defined(BIA_ARCHITECTURE_X86_32)
#define BIA_STATIC_CALLING_CONVETION __attribute__((__fastcall__))
#define BIA_MEMBER_CALLING_CONVENTION __attribute__((__fastcall__))
#define BIA_VARG_MEMBER_CALLING_CONVENTION __attribute__((__cdecl__))
#elif defined(BIA_ARCHITECTURE_X86_64)
#define BIA_STATIC_CALLING_CONVETION
#define BIA_MEMBER_CALLING_CONVENTION
#define BIA_VARG_MEMBER_CALLING_CONVENTION
#endif

// Export
#define BIA_EXPORT

#endif

// Function signatures
template<typename _Return, typename... _Args>
using static_function_signature = _Return(BIA_STATIC_CALLING_CONVETION *)(_Args...);

template<typename _Class, typename _Return, typename... _Args>
using member_function_signature = _Return(BIA_MEMBER_CALLING_CONVENTION _Class::*)(_Args...);

template<typename _Class, typename _Return, typename... _Args>
using const_member_function_signature = _Return(BIA_MEMBER_CALLING_CONVENTION _Class::*)(_Args...) const;

template<typename _Class, typename _Return, typename... _Args>
using varg_member_function_signature = _Return(BIA_VARG_MEMBER_CALLING_CONVENTION _Class::*)(_Args..., ...);

// Universal macros
#define BIA_MIN_INT_SIZE 64
#define BIA_MAX_KEYWORD_LENGTH 16
#define BIA_MAX_OPERATOR_LENGTH 4
/** The minimum buffer size of stream::input_stream::get_buffer(). */
#define BIA_MIN_INPUT_BUFFER_SIZE 1024
/** The maximum identifier length. Can be at most as high as @ref BIA_MIN_INPUT_BUFFER / 4. */
#define BIA_MAX_IDENTIFIER_LENGTH (BIA_MIN_INPUT_BUFFER_SIZE / 4)
