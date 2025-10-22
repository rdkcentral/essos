#ifndef WAYLAND_STUBS_H
#define WAYLAND_STUBS_H

/*
 * Wayland function stubs for compilation without full Wayland support
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

// Forward declarations for Wayland structures
struct wl_shell_surface;
struct wl_egl_window;

// Dummy structures to ensure compilation
typedef struct wl_shell_surface wl_shell_surface;
typedef struct wl_egl_window wl_egl_window;

// Wayland shell surface stubs
static inline void wl_shell_surface_pong(struct wl_shell_surface *shell_surface, uint32_t serial)
{
    // Stub implementation - no-op
    (void)shell_surface;
    (void)serial;
}

// Wayland EGL window stubs  
static inline void wl_egl_window_resize(struct wl_egl_window *egl_window, int width, int height, int dx, int dy)
{
    // Stub implementation - no-op
    (void)egl_window;
    (void)width;
    (void)height;
    (void)dx;
    (void)dy;
}

// Shell surface listener structure (need complete type)
struct wl_shell_surface_listener {
    void (*ping)(void *data, struct wl_shell_surface *wl_shell_surface, uint32_t serial);
    void (*configure)(void *data, struct wl_shell_surface *wl_shell_surface, uint32_t edges, int32_t width, int32_t height);
    void (*popup_done)(void *data, struct wl_shell_surface *wl_shell_surface);
};

#ifdef __cplusplus
}
#endif

#endif // WAYLAND_STUBS_H