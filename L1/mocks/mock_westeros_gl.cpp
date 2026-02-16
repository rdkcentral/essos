/*
 * Mock Westeros GL Implementation for L1 Testing
 * Copyright 2024 RDK Management
 */

#include "westeros-gl.h"
#include <cstdlib>
#include <cstring>

// Mock GL context structure
struct _WstGLCtx {
    int width;
    int height;
    bool initialized;
};

extern "C" {

WstGLCtx* WstGLInit(void) {
    WstGLCtx *ctx = new WstGLCtx();
    ctx->width = 1920;
    ctx->height = 1080;
    ctx->initialized = true;
    return ctx;
}

void WstGLTerm(WstGLCtx *ctx) {
    if (ctx) {
        delete ctx;
    }
}

void* WstGLCreateNativeWindow(WstGLCtx *ctx, int x, int y, int width, int height) {
    (void)ctx;
    (void)x;
    (void)y;
    (void)width;
    (void)height;
    
    // Return a mock native window handle
    return (void*)0x12345678;
}

void WstGLDestroyNativeWindow(WstGLCtx *ctx, void *nativeWindow) {
    (void)ctx;
    (void)nativeWindow;
    // Mock cleanup - nothing to do
}

bool WstGLGetDisplayInfo(WstGLCtx *ctx, int *width, int *height) {
    if (!ctx || !width || !height) {
        return false;
    }
    
    *width = ctx->width;
    *height = ctx->height;
    return true;
}

bool WstGLGetDisplaySafeArea(WstGLCtx *ctx, int *x, int *y, int *w, int *h) {
    if (!ctx || !x || !y || !w || !h) {
        return false;
    }
    
    // Return 5% safe border
    *x = ctx->width * 5 / 100;
    *y = ctx->height * 5 / 100;
    *w = ctx->width * 90 / 100;
    *h = ctx->height * 90 / 100;
    return true;
}

bool WstGLSetDisplayMode(WstGLCtx *ctx, const char *mode) {
    (void)ctx;
    (void)mode;
    // Mock - always succeed
    return true;
}

bool WstGLGetDisplayCaps(WstGLCtx *ctx, unsigned int *caps) {
    if (!ctx || !caps) {
        return false;
    }
    
    *caps = 0; // No special capabilities in mock
    return true;
}

bool WstGLAddDisplaySizeListener(WstGLCtx *ctx, void *userData, DisplaySizeCallback listener) {
    (void)ctx;
    (void)userData;
    (void)listener;
    // Mock - always succeed
    return true;
}

// Internal function pointers (used via dlsym)
bool _WstGLAddDisplaySizeListener(WstGLCtx *ctx, void *userData, DisplaySizeCallback listener) {
    return WstGLAddDisplaySizeListener(ctx, userData, listener);
}

bool _WstGLGetDisplaySafeArea(WstGLCtx *ctx, int *x, int *y, int *w, int *h) {
    return WstGLGetDisplaySafeArea(ctx, x, y, w, h);
}

bool _WstGLGetDisplayCaps(WstGLCtx *ctx, unsigned int *caps) {
    return WstGLGetDisplayCaps(ctx, caps);
}

bool _WstGLSetDisplayMode(WstGLCtx *ctx, const char *mode) {
    return WstGLSetDisplayMode(ctx, mode);
}

} // extern "C"
