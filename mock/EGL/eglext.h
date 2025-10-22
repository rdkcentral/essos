/*
 * EGL Extensions Header Stub
 * Provides minimal EGL extension definitions for compilation
 * This is a fallback header - real EGL headers should be used when available
 */

#ifndef __EGL_EGLEXT_H_STUB__
#define __EGL_EGLEXT_H_STUB__

#ifdef __cplusplus
extern "C" {
#endif

#include "egl.h"

/* EGL Extension Types */
typedef void *EGLImageKHR;
typedef void *EGLSyncKHR;
typedef void *EGLStreamKHR;
typedef void *EGLSync;
typedef void *EGLImage;
typedef khronos_utime_nanoseconds_t EGLTimeKHR;
typedef khronos_utime_nanoseconds_t EGLTime;

/* API calling conventions */
#ifndef EGLAPIENTRY
#define EGLAPIENTRY
#endif

#ifndef EGLAPIENTRYP
#define EGLAPIENTRYP EGLAPIENTRY *
#endif

/* Common extension constants */
#define EGL_NO_IMAGE_KHR                    ((EGLImageKHR)0)
#define EGL_NO_SYNC_KHR                     ((EGLSyncKHR)0)
#define EGL_NO_STREAM_KHR                   ((EGLStreamKHR)0)

/* Extension function pointer types */
typedef EGLImageKHR (EGLAPIENTRY *PFNEGLCREATEIMAGEKHRPROC)(EGLDisplay dpy, EGLContext ctx, EGLenum target, EGLClientBuffer buffer, const EGLint *attrib_list);
typedef EGLBoolean (EGLAPIENTRY *PFNEGLDESTROYIMAGEKHRPROC)(EGLDisplay dpy, EGLImageKHR image);

#ifdef __cplusplus
}
#endif

#endif /* __EGL_EGLEXT_H_STUB__ */
