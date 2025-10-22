#ifndef SIMPLESHELL_CLIENT_PROTOCOL_H
#define SIMPLESHELL_CLIENT_PROTOCOL_H

/*
 * Stub header for simpleshell-client-protocol.h
 * Used when compiling without full Westeros compositor support
 * 
 * This is a Wayland protocol extension for simple shell functionality
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>

// Wayland protocol stubs
struct wl_client;
struct wl_resource;
struct wl_interface;

// Simple shell interface stubs
struct wl_simple_shell;
struct wl_simple_shell_listener {
    void (*name)(void *data, struct wl_simple_shell *wl_simple_shell, const char *name);
    void (*get_position)(void *data, struct wl_simple_shell *wl_simple_shell, uint32_t x, uint32_t y);
    void (*get_opacity)(void *data, struct wl_simple_shell *wl_simple_shell, uint32_t opacity);
    void (*get_zorder)(void *data, struct wl_simple_shell *wl_simple_shell, uint32_t z);
    void (*get_focus)(void *data, struct wl_simple_shell *wl_simple_shell);
    void (*surface_created)(void *data, struct wl_simple_shell *wl_simple_shell, struct wl_resource *surface, const char *name);
    void (*surface_destroyed)(void *data, struct wl_simple_shell *wl_simple_shell, struct wl_resource *surface, const char *name);
    void (*surface_status)(void *data, struct wl_simple_shell *wl_simple_shell, struct wl_resource *surface, const char *name, uint32_t visible, int32_t x, int32_t y, int32_t width, int32_t height, uint32_t opacity, uint32_t zorder);
};

// Simple shell surface interface
struct wl_simple_shell_surface;
struct wl_simple_shell_surface_listener {
    void (*get_position)(void *data, struct wl_simple_shell_surface *surface, uint32_t x, uint32_t y);
    void (*get_opacity)(void *data, struct wl_simple_shell_surface *surface, uint32_t opacity);
    void (*get_zorder)(void *data, struct wl_simple_shell_surface *surface, uint32_t z);
};

// Protocol version
#define WL_SIMPLE_SHELL_VERSION 1
#define WL_SIMPLE_SHELL_SURFACE_VERSION 1

// Interface declarations (external)
extern const struct wl_interface wl_simple_shell_interface;
extern const struct wl_interface wl_simple_shell_surface_interface;

// Stub function declarations (not implemented)
static inline struct wl_simple_shell *
wl_simple_shell_create(struct wl_client *client, uint32_t version, uint32_t id) {
    return NULL;
}

static inline void
wl_simple_shell_destroy(struct wl_simple_shell *shell) {
    // Stub implementation
}

static inline int
wl_simple_shell_add_listener(struct wl_simple_shell *shell,
                             const struct wl_simple_shell_listener *listener,
                             void *data) {
    return 0; // Success stub
}

static inline struct wl_simple_shell_surface *
wl_simple_shell_get_surface(struct wl_simple_shell *shell, struct wl_resource *surface) {
    return NULL;
}

static inline void
wl_simple_shell_surface_destroy(struct wl_simple_shell_surface *surface) {
    // Stub implementation  
}

static inline int
wl_simple_shell_surface_add_listener(struct wl_simple_shell_surface *surface,
                                     const struct wl_simple_shell_surface_listener *listener,
                                     void *data) {
    return 0; // Success stub
}

// Request method stubs
static inline void
wl_simple_shell_set_name(struct wl_simple_shell *shell, const char *name) {
    // Stub implementation
}

static inline void
wl_simple_shell_set_position(struct wl_simple_shell *shell, uint32_t x, uint32_t y) {
    // Stub implementation
}

static inline void
wl_simple_shell_set_opacity(struct wl_simple_shell *shell, uint32_t opacity) {
    // Stub implementation
}

static inline void
wl_simple_shell_set_zorder(struct wl_simple_shell *shell, uint32_t z) {
    // Stub implementation
}

static inline void
wl_simple_shell_surface_set_position(struct wl_simple_shell_surface *surface, uint32_t x, uint32_t y) {
    // Stub implementation
}

static inline void
wl_simple_shell_surface_set_opacity(struct wl_simple_shell_surface *surface, uint32_t opacity) {
    // Stub implementation
}

static inline void
wl_simple_shell_surface_set_zorder(struct wl_simple_shell_surface *surface, uint32_t z) {
    // Stub implementation
}

#ifdef __cplusplus
}
#endif

#endif /* SIMPLESHELL_CLIENT_PROTOCOL_H */