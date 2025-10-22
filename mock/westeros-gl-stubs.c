/*
 * Westeros GL stub implementations
 * Provides actual function implementations for linking when Westeros is not available
 */

#include "westeros-gl.h"
#include <stdio.h>
#include <stdlib.h>

// Stub implementations for Westeros GL functions
WstGLCtx* WstGLInit()
{
    printf("WstGLInit: Using stub implementation\n");
    // Return a dummy context pointer
    return (WstGLCtx*)malloc(sizeof(void*));
}

void WstGLTerm(WstGLCtx* ctx)
{
    printf("WstGLTerm: Using stub implementation\n");
    if (ctx) {
        free(ctx);
    }
}

bool WstGLGetDisplayType(WstGLCtx* ctx, void** displayType)
{
    printf("WstGLGetDisplayType: Using stub implementation\n");
    (void)ctx;  // Suppress unused parameter warning
    if (displayType) {
        *displayType = NULL;
    }
    return false;  // Indicate failure in stub mode
}

void* WstGLCreateNativeWindow(WstGLCtx* ctx, int x, int y, int width, int height)
{
    printf("WstGLCreateNativeWindow: Using stub implementation (x=%d, y=%d, w=%d, h=%d)\n", x, y, width, height);
    (void)ctx;  // Suppress unused parameter warning
    
    // Return a dummy window handle
    return (void*)0x12345678;  // Dummy pointer value
}

void WstGLDestroyNativeWindow(WstGLCtx* ctx, void* nativeWindow)
{
    printf("WstGLDestroyNativeWindow: Using stub implementation\n");
    (void)ctx;           // Suppress unused parameter warning
    (void)nativeWindow;  // Suppress unused parameter warning
    
    // Nothing to actually destroy in stub mode
}