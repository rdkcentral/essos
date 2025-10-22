/*
 * EGL Stub implementations
 * Provides minimal EGL function implementations for linking when EGL is not available
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// EGL types
typedef void *EGLDisplay;
typedef void *EGLConfig;
typedef void *EGLSurface;
typedef void *EGLContext;
typedef void *EGLClientBuffer;
typedef void *EGLNativeDisplayType;
typedef void *EGLNativePixmapType;
typedef void *EGLNativeWindowType;
typedef int EGLint;
typedef unsigned int EGLBoolean;
typedef unsigned int EGLenum;
typedef void (*__eglMustCastToProperFunctionPointerType)(void);

#define EGL_FALSE 0
#define EGL_TRUE  1
#define EGL_SUCCESS 0x3000
#define EGL_NOT_INITIALIZED 0x3001
#define EGL_BAD_ACCESS 0x3002
#define EGL_BAD_ALLOC 0x3003
#define EGL_BAD_ATTRIBUTE 0x3004
#define EGL_BAD_CONFIG 0x3005
#define EGL_BAD_CONTEXT 0x3006
#define EGL_BAD_CURRENT_SURFACE 0x3007
#define EGL_BAD_DISPLAY 0x3008
#define EGL_BAD_MATCH 0x3009
#define EGL_BAD_NATIVE_PIXMAP 0x300A
#define EGL_BAD_NATIVE_WINDOW 0x300B
#define EGL_BAD_PARAMETER 0x300C
#define EGL_BAD_SURFACE 0x300D
#define EGL_CONTEXT_LOST 0x300E

#define EGL_NO_DISPLAY ((EGLDisplay)0)
#define EGL_NO_SURFACE ((EGLSurface)0)
#define EGL_NO_CONTEXT ((EGLContext)0)

// Global error state
static EGLint egl_error = EGL_SUCCESS;

// Stub implementations
EGLDisplay eglGetDisplay(EGLNativeDisplayType display_id)
{
    (void)display_id;
    return (EGLDisplay)0x1;
}

EGLBoolean eglInitialize(EGLDisplay dpy, EGLint *major, EGLint *minor)
{
    (void)dpy;
    if (major) *major = 1;
    if (minor) *minor = 4;
    egl_error = EGL_SUCCESS;
    return EGL_TRUE;
}

EGLBoolean eglTerminate(EGLDisplay dpy)
{
    (void)dpy;
    egl_error = EGL_SUCCESS;
    return EGL_TRUE;
}

EGLint eglGetError(void)
{
    EGLint err = egl_error;
    egl_error = EGL_SUCCESS;
    return err;
}

EGLBoolean eglChooseConfig(EGLDisplay dpy, const EGLint *attrib_list,
                          EGLConfig *configs, EGLint config_size, EGLint *num_config)
{
    (void)dpy;
    (void)attrib_list;
    if (configs && config_size > 0) {
        configs[0] = (EGLConfig)0x2;
    }
    if (num_config) {
        *num_config = 1;
    }
    egl_error = EGL_SUCCESS;
    return EGL_TRUE;
}

EGLBoolean eglGetConfigAttrib(EGLDisplay dpy, EGLConfig config,
                             EGLint attribute, EGLint *value)
{
    (void)dpy;
    (void)config;
    (void)attribute;
    if (value) {
        *value = 0;
    }
    egl_error = EGL_SUCCESS;
    return EGL_TRUE;
}

EGLContext eglCreateContext(EGLDisplay dpy, EGLConfig config,
                           EGLContext share_context, const EGLint *attrib_list)
{
    (void)dpy;
    (void)config;
    (void)share_context;
    (void)attrib_list;
    egl_error = EGL_SUCCESS;
    return (EGLContext)0x3;
}

EGLBoolean eglDestroyContext(EGLDisplay dpy, EGLContext ctx)
{
    (void)dpy;
    (void)ctx;
    egl_error = EGL_SUCCESS;
    return EGL_TRUE;
}

EGLSurface eglCreateWindowSurface(EGLDisplay dpy, EGLConfig config,
                                 EGLNativeWindowType win, const EGLint *attrib_list)
{
    (void)dpy;
    (void)config;
    (void)win;
    (void)attrib_list;
    egl_error = EGL_SUCCESS;
    return (EGLSurface)0x4;
}

EGLBoolean eglDestroySurface(EGLDisplay dpy, EGLSurface surface)
{
    (void)dpy;
    (void)surface;
    egl_error = EGL_SUCCESS;
    return EGL_TRUE;
}

EGLBoolean eglMakeCurrent(EGLDisplay dpy, EGLSurface draw,
                         EGLSurface read, EGLContext ctx)
{
    (void)dpy;
    (void)draw;
    (void)read;
    (void)ctx;
    egl_error = EGL_SUCCESS;
    return EGL_TRUE;
}

EGLBoolean eglSwapBuffers(EGLDisplay dpy, EGLSurface surface)
{
    (void)dpy;
    (void)surface;
    egl_error = EGL_SUCCESS;
    return EGL_TRUE;
}

EGLBoolean eglSwapInterval(EGLDisplay dpy, EGLint interval)
{
    (void)dpy;
    (void)interval;
    egl_error = EGL_SUCCESS;
    return EGL_TRUE;
}

EGLBoolean eglReleaseThread(void)
{
    egl_error = EGL_SUCCESS;
    return EGL_TRUE;
}

__eglMustCastToProperFunctionPointerType eglGetProcAddress(const char *procname)
{
    (void)procname;
    return NULL;
}
