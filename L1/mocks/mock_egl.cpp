/*
 * Mock EGL Implementation for L1 Testing
 * Copyright 2024 RDK Management
 */

#include <map>
#include <string>
#include <cstring>
#include <cstdlib>

extern "C" {
#include <EGL/egl.h>
#include <EGL/eglext.h>
}

// Mock state
static bool g_mock_egl_initialized = false;
static EGLint g_mock_error = EGL_SUCCESS;
static std::map<void*, std::string> g_mock_egl_objects;
static bool g_mock_make_current_should_fail = false;

// Mock EGL structures
struct _EGLDisplay {
    bool initialized;
    EGLint major;
    EGLint minor;
};

struct _EGLConfig {
    EGLint config_id;
    EGLint surface_type;
    EGLint renderable_type;
};

struct _EGLSurface {
    EGLDisplay display;
    EGLConfig config;
    EGLint width;
    EGLint height;
};

struct _EGLContext {
    EGLDisplay display;
    EGLConfig config;
    EGLint context_client_version;
};

// Control functions for testing
extern "C" {

void mock_egl_reset() {
    g_mock_egl_initialized = false;
    g_mock_error = EGL_SUCCESS;
    g_mock_egl_objects.clear();
    g_mock_make_current_should_fail = false;
}

void mock_egl_set_make_current_fail(bool should_fail) {
    g_mock_make_current_should_fail = should_fail;
}

void mock_egl_set_error(EGLint error) {
    g_mock_error = error;
}

// EGL Display functions
EGLDisplay eglGetDisplay(EGLNativeDisplayType display_id) {
    (void)display_id;
    _EGLDisplay *display = new _EGLDisplay();
    display->initialized = false;
    display->major = 1;
    display->minor = 4;
    g_mock_egl_objects[display] = "EGLDisplay";
    return (EGLDisplay)display;
}

EGLBoolean eglInitialize(EGLDisplay dpy, EGLint *major, EGLint *minor) {
    _EGLDisplay *display = (_EGLDisplay*)dpy;
    if (!display) {
        g_mock_error = EGL_BAD_DISPLAY;
        return EGL_FALSE;
    }
    
    display->initialized = true;
    if (major) *major = display->major;
    if (minor) *minor = display->minor;
    g_mock_error = EGL_SUCCESS;
    return EGL_TRUE;
}

EGLBoolean eglTerminate(EGLDisplay dpy) {
    _EGLDisplay *display = (_EGLDisplay*)dpy;
    if (!display) {
        g_mock_error = EGL_BAD_DISPLAY;
        return EGL_FALSE;
    }
    
    display->initialized = false;
    g_mock_error = EGL_SUCCESS;
    return EGL_TRUE;
}

// EGL Configuration functions
EGLBoolean eglChooseConfig(EGLDisplay dpy, const EGLint *attrib_list, 
                          EGLConfig *configs, EGLint config_size, EGLint *num_config) {
    _EGLDisplay *display = (_EGLDisplay*)dpy;
    if (!display || !display->initialized) {
        g_mock_error = EGL_NOT_INITIALIZED;
        return EGL_FALSE;
    }
    
    (void)attrib_list;
    
    if (num_config) {
        *num_config = 1;
    }
    
    if (configs && config_size > 0) {
        _EGLConfig *config = new _EGLConfig();
        config->config_id = 1;
        config->surface_type = EGL_WINDOW_BIT;
        config->renderable_type = EGL_OPENGL_ES2_BIT;
        g_mock_egl_objects[config] = "EGLConfig";
        configs[0] = (EGLConfig)config;
    }
    
    g_mock_error = EGL_SUCCESS;
    return EGL_TRUE;
}

EGLBoolean eglGetConfigAttrib(EGLDisplay dpy, EGLConfig config, EGLint attribute, EGLint *value) {
    _EGLDisplay *display = (_EGLDisplay*)dpy;
    _EGLConfig *cfg = (_EGLConfig*)config;
    
    if (!display || !display->initialized) {
        g_mock_error = EGL_NOT_INITIALIZED;
        return EGL_FALSE;
    }
    
    if (!cfg || !value) {
        g_mock_error = EGL_BAD_CONFIG;
        return EGL_FALSE;
    }
    
    switch (attribute) {
        case EGL_CONFIG_ID:
            *value = cfg->config_id;
            break;
        case EGL_SURFACE_TYPE:
            *value = cfg->surface_type;
            break;
        case EGL_RENDERABLE_TYPE:
            *value = cfg->renderable_type;
            break;
        case EGL_RED_SIZE:
            *value = 8;
            break;
        case EGL_GREEN_SIZE:
            *value = 8;
            break;
        case EGL_BLUE_SIZE:
            *value = 8;
            break;
        case EGL_ALPHA_SIZE:
            *value = 8;
            break;
        case EGL_DEPTH_SIZE:
            *value = 24;
            break;
        default:
            *value = 0;
            break;
    }
    
    g_mock_error = EGL_SUCCESS;
    return EGL_TRUE;
}

// EGL Surface functions
EGLSurface eglCreateWindowSurface(EGLDisplay dpy, EGLConfig config, 
                                 EGLNativeWindowType win, const EGLint *attrib_list) {
    _EGLDisplay *display = (_EGLDisplay*)dpy;
    _EGLConfig *cfg = (_EGLConfig*)config;
    
    if (!display || !display->initialized) {
        g_mock_error = EGL_NOT_INITIALIZED;
        return EGL_NO_SURFACE;
    }
    
    if (!cfg) {
        g_mock_error = EGL_BAD_CONFIG;
        return EGL_NO_SURFACE;
    }
    
    (void)win;
    (void)attrib_list;
    
    _EGLSurface *surface = new _EGLSurface();
    surface->display = dpy;
    surface->config = config;
    surface->width = 1920;
    surface->height = 1080;
    g_mock_egl_objects[surface] = "EGLSurface";
    
    g_mock_error = EGL_SUCCESS;
    return (EGLSurface)surface;
}

EGLBoolean eglDestroySurface(EGLDisplay dpy, EGLSurface surface) {
    _EGLDisplay *display = (_EGLDisplay*)dpy;
    _EGLSurface *surf = (_EGLSurface*)surface;
    
    if (!display || !display->initialized) {
        g_mock_error = EGL_NOT_INITIALIZED;
        return EGL_FALSE;
    }
    
    if (surf) {
        g_mock_egl_objects.erase(surf);
        delete surf;
    }
    
    g_mock_error = EGL_SUCCESS;
    return EGL_TRUE;
}

EGLBoolean eglQuerySurface(EGLDisplay dpy, EGLSurface surface, EGLint attribute, EGLint *value) {
    _EGLDisplay *display = (_EGLDisplay*)dpy;
    _EGLSurface *surf = (_EGLSurface*)surface;
    
    if (!display || !display->initialized) {
        g_mock_error = EGL_NOT_INITIALIZED;
        return EGL_FALSE;
    }
    
    if (!surf || !value) {
        g_mock_error = EGL_BAD_SURFACE;
        return EGL_FALSE;
    }
    
    switch (attribute) {
        case EGL_WIDTH:
            *value = surf->width;
            break;
        case EGL_HEIGHT:
            *value = surf->height;
            break;
        default:
            *value = 0;
            break;
    }
    
    g_mock_error = EGL_SUCCESS;
    return EGL_TRUE;
}

// EGL Context functions
EGLContext eglCreateContext(EGLDisplay dpy, EGLConfig config, 
                           EGLContext share_context, const EGLint *attrib_list) {
    _EGLDisplay *display = (_EGLDisplay*)dpy;
    _EGLConfig *cfg = (_EGLConfig*)config;
    
    if (!display || !display->initialized) {
        g_mock_error = EGL_NOT_INITIALIZED;
        return EGL_NO_CONTEXT;
    }
    
    if (!cfg) {
        g_mock_error = EGL_BAD_CONFIG;
        return EGL_NO_CONTEXT;
    }
    
    (void)share_context;
    
    _EGLContext *context = new _EGLContext();
    context->display = dpy;
    context->config = config;
    context->context_client_version = 2;
    
    // Parse attrib list for version
    if (attrib_list) {
        for (int i = 0; attrib_list[i] != EGL_NONE; i += 2) {
            if (attrib_list[i] == EGL_CONTEXT_CLIENT_VERSION) {
                context->context_client_version = attrib_list[i + 1];
            }
        }
    }
    
    g_mock_egl_objects[context] = "EGLContext";
    g_mock_error = EGL_SUCCESS;
    return (EGLContext)context;
}

EGLBoolean eglDestroyContext(EGLDisplay dpy, EGLContext ctx) {
    _EGLDisplay *display = (_EGLDisplay*)dpy;
    _EGLContext *context = (_EGLContext*)ctx;
    
    if (!display || !display->initialized) {
        g_mock_error = EGL_NOT_INITIALIZED;
        return EGL_FALSE;
    }
    
    if (context) {
        g_mock_egl_objects.erase(context);
        delete context;
    }
    
    g_mock_error = EGL_SUCCESS;
    return EGL_TRUE;
}

EGLBoolean eglMakeCurrent(EGLDisplay dpy, EGLSurface draw, EGLSurface read, EGLContext ctx) {
    _EGLDisplay *display = (_EGLDisplay*)dpy;
    
    if (!display || !display->initialized) {
        g_mock_error = EGL_NOT_INITIALIZED;
        return EGL_FALSE;
    }
    
    if (g_mock_make_current_should_fail) {
        g_mock_error = EGL_BAD_MATCH;
        return EGL_FALSE;
    }
    
    (void)draw;
    (void)read;
    (void)ctx;
    
    g_mock_error = EGL_SUCCESS;
    return EGL_TRUE;
}

EGLContext eglGetCurrentContext() {
    return EGL_NO_CONTEXT;
}

// EGL Buffer swap
EGLBoolean eglSwapBuffers(EGLDisplay dpy, EGLSurface surface) {
    _EGLDisplay *display = (_EGLDisplay*)dpy;
    
    if (!display || !display->initialized) {
        g_mock_error = EGL_NOT_INITIALIZED;
        return EGL_FALSE;
    }
    
    (void)surface;
    g_mock_error = EGL_SUCCESS;
    return EGL_TRUE;
}

// EGL Error
EGLint eglGetError() {
    EGLint error = g_mock_error;
    g_mock_error = EGL_SUCCESS;
    return error;
}

// EGL Binding
EGLBoolean eglBindAPI(EGLenum api) {
    if (api == EGL_OPENGL_ES_API) {
        g_mock_error = EGL_SUCCESS;
        return EGL_TRUE;
    }
    g_mock_error = EGL_BAD_PARAMETER;
    return EGL_FALSE;
}

const char* eglQueryString(EGLDisplay dpy, EGLint name) {
    (void)dpy;
    
    switch (name) {
        case EGL_VENDOR:
            return "Mock EGL Vendor";
        case EGL_VERSION:
            return "1.4";
        case EGL_EXTENSIONS:
            return "EGL_KHR_create_context";
        default:
            return "";
    }
}

void (*eglGetProcAddress(const char *procname))(void) {
    (void)procname;
    // Return a dummy function pointer
    return nullptr;
}

EGLBoolean eglSwapInterval(EGLDisplay dpy, EGLint interval) {
    (void)dpy;
    (void)interval;
    g_mock_error = EGL_SUCCESS;
    return EGL_TRUE;
}

EGLBoolean eglReleaseThread(void) {
    g_mock_error = EGL_SUCCESS;
    return EGL_TRUE;
}

} // extern "C"

