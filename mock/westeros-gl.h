#ifndef __WESTEROS_GL_H__
#define __WESTEROS_GL_H__

/*
 * Stub header for westeros-gl.h
 * Used when compiling without full Westeros compositor support
 */

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Westeros GL stub definitions
typedef void* WstGLCtx;

// Stub function declarations (not implemented)
WstGLCtx* WstGLInit();
void WstGLTerm(WstGLCtx* ctx);
bool WstGLGetDisplayType(WstGLCtx* ctx, void** displayType);
void* WstGLCreateNativeWindow(WstGLCtx* ctx, int x, int y, int width, int height);
void WstGLDestroyNativeWindow(WstGLCtx* ctx, void* nativeWindow);

// Westeros specific constants
#define WST_GL_SUCCESS 1
#define WST_GL_FAILURE 0

#ifdef __cplusplus
}
#endif

#endif // __WESTEROS_GL_H__