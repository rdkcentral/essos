#include <EGL/egl.h>

extern "C" {
EGLint eglGetError(void) { return EGL_SUCCESS; }
EGLDisplay eglGetDisplay(EGLNativeDisplayType) { return EGL_NO_DISPLAY; }
EGLBoolean eglInitialize(EGLDisplay, EGLint *, EGLint *) { return EGL_FALSE; }
EGLBoolean eglChooseConfig(EGLDisplay, const EGLint *, EGLConfig *, EGLint, EGLint *) { return EGL_FALSE; }
EGLBoolean eglGetConfigAttrib(EGLDisplay, EGLConfig, EGLint, EGLint *) { return EGL_FALSE; }
EGLContext eglCreateContext(EGLDisplay, EGLConfig, EGLContext, const EGLint *) { return EGL_NO_CONTEXT; }
EGLSurface eglCreateWindowSurface(EGLDisplay, EGLConfig, EGLNativeWindowType, const EGLint *) { return EGL_NO_SURFACE; }
EGLBoolean eglMakeCurrent(EGLDisplay, EGLSurface, EGLSurface, EGLContext) { return EGL_FALSE; }
EGLBoolean eglSwapInterval(EGLDisplay, EGLint) { return EGL_FALSE; }
EGLBoolean eglDestroySurface(EGLDisplay, EGLSurface) { return EGL_TRUE; }
EGLBoolean eglDestroyContext(EGLDisplay, EGLContext) { return EGL_TRUE; }
EGLBoolean eglTerminate(EGLDisplay) { return EGL_TRUE; }
EGLBoolean eglReleaseThread(void) { return EGL_TRUE; }
EGLBoolean eglSwapBuffers(EGLDisplay, EGLSurface) { return EGL_FALSE; }
__eglMustCastToProperFunctionPointerType eglGetProcAddress(const char *) { return 0; }
}
