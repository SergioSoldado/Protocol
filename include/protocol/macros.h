#pragma once
/**
 * @file   protocol/macros.h
 * @brief  Compiler macros
 */

/**
 * @def PROTOCOL_EXTERN_C_BEGIN
 * Begins a C region when included in a C++ file.
 *
 * @def PROTOCOL_EXTERN_C_END
 * Ends a C region in C++ files.
 */
#ifdef __cplusplus
#define PROTOCOL_EXTERN_C_BEGIN extern "C" {
#define PROTOCOL_EXTERN_C_END }
#else
#define PROTOCOL_EXTERN_C_BEGIN
#define PROTOCOL_EXTERN_C_END
#endif

/**
 * @def PROTOCOL_DLL_PUBLIC
 * Enables public export of function in shared library
 *
 * @def PROTOCOL_DLL_LOCAL
 * Disables public export of function in shared library
 */
#if defined _WIN32 || defined __CYGWIN__
#ifdef BUILDING_DLL
#ifdef __GNUC__
#define PROTOCOL_DLL_PUBLIC __attribute__((dllexport))
#else
#define PROTOCOL_DLL_PUBLIC __declspec(dllexport)
#endif
#else
#ifdef __GNUC__
#define PROTOCOL_DLL_PUBLIC __attribute__((dllimport))
#else
#define PROTOCOL_DLL_PUBLIC __declspec(dllimport)
#endif
#endif
#define PROTOCOL_DLL_LOCAL
#elif __GNUC__ >= 4
#define PROTOCOL_DLL_PUBLIC __attribute__((visibility("default")))
#define PROTOCOL_DLL_LOCAL __attribute__((visibility("hidden")))
#else
#define PROTOCOL_DLL_PUBLIC
#define PROTOCOL_DLL_LOCAL
#endif
