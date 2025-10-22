#ifndef __ESSOS_MOCK__
#define __ESSOS_MOCK__

// Mock definitions for Windows compilation testing
// This allows syntax checking and basic compilation without actual Essos library

#ifdef _WIN32

// Mock EGL types
typedef void* EGLDisplay;
typedef void* EGLConfig;
typedef int EGLint;
typedef void* NativeDisplayType;
typedef void* NativeWindowType;

// Mock constants
#define EGL_NONE 0x3038
#define EGL_RED_SIZE 0x3024
#define EGL_GREEN_SIZE 0x3023
#define EGL_BLUE_SIZE 0x3022
#define EGL_ALPHA_SIZE 0x3021
#define EGL_DEPTH_SIZE 0x3025

// Mock Essos types and enums
typedef struct EssCtx EssCtx;

typedef enum _EssAppPlatformDisplayType
{
   EssAppPlatformDisplayType_direct,
   EssAppPlatformDisplayType_wayland,
   EssAppPlatformDisplayType_waylandExtension
} EssAppPlatformDisplayType;

// Mock function declarations (implementation not provided)
EssCtx* EssContextCreate(void);
void EssContextDestroy(EssCtx* ctx);
bool EssContextInit(EssCtx* ctx);
const char* EssContextGetLastErrorDetail(EssCtx* ctx);
bool EssContextSetUseWayland(EssCtx* ctx, bool useWayland);
bool EssContextGetUseWayland(EssCtx* ctx);
bool EssContextSetUseDirect(EssCtx* ctx, bool useDirect);
bool EssContextGetUseDirect(EssCtx* ctx);
bool EssContextSupportWayland(EssCtx* ctx);
bool EssContextSupportDirect(EssCtx* ctx);
EssAppPlatformDisplayType EssContextGetAppPlatformDisplayType(EssCtx* ctx);
bool EssContextSetDisplaySize(EssCtx* ctx, int width, int height);
bool EssContextSetInitialWindowSize(EssCtx* ctx, int width, int height);

#else
// On Linux, include the real headers
#include "essos.h"
#endif

#endif // __ESSOS_MOCK__