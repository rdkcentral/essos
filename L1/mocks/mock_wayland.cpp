/*
 * Mock Wayland Implementation for L1 Testing
 * Copyright 2024 RDK Management
 */

#include <map>
#include <string>
#include <vector>
#include <cstring>
#include <cstdlib>

extern "C" {
#include <wayland-client.h>
#include <wayland-egl.h>
}

// Mock state
static bool g_mock_wayland_initialized = false;
static std::map<void*, std::string> g_mock_objects;
static int g_mock_display_fd = 100;
static bool g_mock_dispatch_should_fail = false;
static int g_mock_dispatch_count = 0;

// Mock Wayland Display
struct wl_display {
    int fd;
    bool valid;
};

// Mock Wayland Registry
struct wl_registry {
    wl_display *display;
};

// Mock Wayland Compositor
struct wl_compositor {
    int version;
};

// Mock Wayland Surface
struct wl_surface {
    wl_compositor *compositor;
    int x, y;
    int width, height;
};

// Mock Wayland Seat
struct wl_seat {
    uint32_t capabilities;
};

// Mock Wayland Pointer
struct wl_pointer {
    wl_seat *seat;
};

// Mock Wayland Keyboard
struct wl_keyboard {
    wl_seat *seat;
};

// Mock Wayland Touch
struct wl_touch {
    wl_seat *seat;
};

// Mock Wayland Shell Surface
struct wl_shell_surface {
    wl_surface *surface;
};

// Mock Wayland EGL Window
struct wl_egl_window {
    wl_surface *surface;
    int width;
    int height;
};

// Mock Wayland Proxy
struct wl_proxy {
    void *user_data;
    const wl_interface *interface;
    uint32_t version;
};

// Mock interface definitions
static const struct wl_interface mock_wl_compositor_interface = { "wl_compositor", 4, 0, nullptr, 0, nullptr };
static const struct wl_interface mock_wl_seat_interface = { "wl_seat", 7, 0, nullptr, 0, nullptr };
static const struct wl_interface mock_wl_shell_interface = { "wl_shell", 1, 0, nullptr, 0, nullptr };
static const struct wl_interface mock_wl_output_interface = { "wl_output", 3, 0, nullptr, 0, nullptr };
static const struct wl_interface mock_wl_registry_interface = { "wl_registry", 1, 0, nullptr, 0, nullptr };
static const struct wl_interface mock_wl_surface_interface = { "wl_surface", 4, 0, nullptr, 0, nullptr };
static const struct wl_interface mock_wl_shell_surface_interface = { "wl_shell_surface", 1, 0, nullptr, 0, nullptr };
static const struct wl_interface mock_wl_pointer_interface = { "wl_pointer", 7, 0, nullptr, 0, nullptr };
static const struct wl_interface mock_wl_keyboard_interface = { "wl_keyboard", 7, 0, nullptr, 0, nullptr };
static const struct wl_interface mock_wl_touch_interface = { "wl_touch", 7, 0, nullptr, 0, nullptr };
static const struct wl_interface mock_wl_simple_shell_interface = { "wl_simple_shell", 1, 0, nullptr, 0, nullptr };

// Export interface pointers
extern "C" {
const struct wl_interface wl_compositor_interface = mock_wl_compositor_interface;
const struct wl_interface wl_seat_interface = mock_wl_seat_interface;
const struct wl_interface wl_shell_interface = mock_wl_shell_interface;
const struct wl_interface wl_output_interface = mock_wl_output_interface;
const struct wl_interface wl_registry_interface = mock_wl_registry_interface;
const struct wl_interface wl_surface_interface = mock_wl_surface_interface;
const struct wl_interface wl_shell_surface_interface = mock_wl_shell_surface_interface;
const struct wl_interface wl_pointer_interface = mock_wl_pointer_interface;
const struct wl_interface wl_keyboard_interface = mock_wl_keyboard_interface;
const struct wl_interface wl_touch_interface = mock_wl_touch_interface;
const struct wl_interface wl_simple_shell_interface = mock_wl_simple_shell_interface;
}

// Control functions for testing
extern "C" {

void mock_wayland_reset() {
    g_mock_wayland_initialized = false;
    g_mock_objects.clear();
    g_mock_display_fd = 100;
    g_mock_dispatch_should_fail = false;
    g_mock_dispatch_count = 0;
}

void mock_wayland_set_dispatch_fail(bool should_fail) {
    g_mock_dispatch_should_fail = should_fail;
}

int mock_wayland_get_dispatch_count() {
    return g_mock_dispatch_count;
}

// Wayland Display functions
wl_display* wl_display_connect(const char *name) {
    (void)name;
    wl_display *display = new wl_display();
    display->fd = g_mock_display_fd;
    display->valid = true;
    g_mock_objects[display] = "wl_display";
    return display;
}

void wl_display_disconnect(wl_display *display) {
    if (display) {
        g_mock_objects.erase(display);
        delete display;
    }
}

int wl_display_get_fd(wl_display *display) {
    return display ? display->fd : -1;
}

int wl_display_dispatch(wl_display *display) {
    if (!display || g_mock_dispatch_should_fail) {
        return -1;
    }
    g_mock_dispatch_count++;
    return 0;
}

int wl_display_dispatch_pending(wl_display *display) {
    if (!display) {
        return -1;
    }
    return 0;
}

int wl_display_flush(wl_display *display) {
    return display ? 0 : -1;
}

int wl_display_roundtrip(wl_display *display) {
    return display ? 0 : -1;
}

// Note: The following functions are inline functions in wayland-client-protocol.h
// They internally call wl_proxy_marshal_flags and wl_proxy_add_listener which we mock below
// So they will work correctly with our mocks without needing redefinition

// EGL Window functions
wl_egl_window* wl_egl_window_create(wl_surface *surface, int width, int height) {
    if (!surface) {
        return nullptr;
    }
    wl_egl_window *window = new wl_egl_window();
    window->surface = surface;
    window->width = width;
    window->height = height;
    g_mock_objects[window] = "wl_egl_window";
    return window;
}

void wl_egl_window_destroy(wl_egl_window *window) {
    if (window) {
        g_mock_objects.erase(window);
        delete window;
    }
}

void wl_egl_window_resize(wl_egl_window *window, int width, int height, int dx, int dy) {
    if (window) {
        window->width = width;
        window->height = height;
        (void)dx;
        (void)dy;
    }
}

void wl_egl_window_get_attached_size(wl_egl_window *window, int *width, int *height) {
    if (window && width && height) {
        *width = window->width;
        *height = window->height;
    }
}

// Proxy functions
void wl_proxy_destroy(wl_proxy *proxy) {
    if (proxy) {
        g_mock_objects.erase(proxy);
        delete proxy;
    }
}

void wl_proxy_set_user_data(wl_proxy *proxy, void *user_data) {
    if (proxy) {
        proxy->user_data = user_data;
    }
}

void* wl_proxy_get_user_data(wl_proxy *proxy) {
    return proxy ? proxy->user_data : nullptr;
}

uint32_t wl_proxy_get_version(struct wl_proxy *proxy) {
    return proxy ? proxy->version : 1;
}

int wl_proxy_add_listener(struct wl_proxy *proxy, void (**implementation)(void), void *data) {
    (void)proxy;
    (void)implementation;
    (void)data;
    return 0;
}

struct wl_proxy* wl_proxy_marshal_flags(struct wl_proxy *proxy, uint32_t opcode,
                                        const struct wl_interface *interface, uint32_t version,
                                        uint32_t flags, ...) {
    (void)proxy;
    (void)opcode;
    (void)flags;
    
    if (!interface) {
        return nullptr;
    }
    
    wl_proxy *new_proxy = new wl_proxy();
    new_proxy->interface = interface;
    new_proxy->version = version;
    new_proxy->user_data = nullptr;
    return new_proxy;
}

int wl_display_prepare_read(struct wl_display *display) {
    return display ? 0 : -1;
}

int wl_display_read_events(struct wl_display *display) {
    return display ? 0 : -1;
}

} // extern "C"

