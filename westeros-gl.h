/*
 * Westeros GL Header
 * Copyright 2024 RDK Management
 */

#ifndef WESTEROS_GL_H
#define WESTEROS_GL_H

#ifdef __cplusplus
extern "C" {
#endif

// Forward declarations
typedef struct _WstGLCtx WstGLCtx;

// Display capability flags
typedef enum _WstGLDisplayCap
{
   WstGLDisplayCap_modeset = 0x01
} WstGLDisplayCap;

// Callback types
typedef void (*DisplaySizeCallback)(void *userData, int width, int height);

// API Functions
WstGLCtx* WstGLInit(void);
void WstGLTerm(WstGLCtx *ctx);

void* WstGLCreateNativeWindow(WstGLCtx *ctx, int x, int y, int width, int height);
void WstGLDestroyNativeWindow(WstGLCtx *ctx, void *nativeWindow);

bool WstGLGetDisplayInfo(WstGLCtx *ctx, int *width, int *height);
bool WstGLGetDisplaySafeArea(WstGLCtx *ctx, int *x, int *y, int *w, int *h);
bool WstGLSetDisplayMode(WstGLCtx *ctx, const char *mode);
bool WstGLGetDisplayCaps(WstGLCtx *ctx, unsigned int *caps);

bool WstGLAddDisplaySizeListener(WstGLCtx *ctx, void *userData, DisplaySizeCallback listener);

// Internal function pointers (used via dlsym)
bool _WstGLAddDisplaySizeListener(WstGLCtx *ctx, void *userData, DisplaySizeCallback listener);
bool _WstGLGetDisplaySafeArea(WstGLCtx *ctx, int *x, int *y, int *w, int *h);
bool _WstGLGetDisplayCaps(WstGLCtx *ctx, unsigned int *caps);
bool _WstGLSetDisplayMode(WstGLCtx *ctx, const char *mode);

#ifdef __cplusplus
}
#endif

#endif // WESTEROS_GL_H
