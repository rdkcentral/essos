/*
 * Simple Shell Client Protocol Header
 * Copyright 2024 RDK Management
 */

#ifndef SIMPLESHELL_CLIENT_PROTOCOL_H
#define SIMPLESHELL_CLIENT_PROTOCOL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <wayland-client.h>

struct wl_simple_shell;

/**
 * Simple shell listener interface
 */
struct wl_simple_shell_listener {
    /**
     * surface_id - surface id assignment
     * @surface: the surface
     * @surfaceId: the surface id
     * @name: the surface name
     */
    void (*surface_id)(void *data,
                      struct wl_simple_shell *wl_simple_shell,
                      struct wl_surface *surface,
                      uint32_t surfaceId,
                      const char *name);
    
    /**
     * surface_created - surface creation notification
     * @surfaceId: the surface id
     * @surfaceName: name of the surface
     * @type: surface type
     */
    void (*surface_created)(void *data,
                           struct wl_simple_shell *wl_simple_shell,
                           uint32_t surfaceId,
                           const char *surfaceName,
                           uint32_t type);
    
    /**
     * surface_destroyed - surface destruction notification
     * @surfaceId: the surface id
     * @surfaceName: name of the surface
     */
    void (*surface_destroyed)(void *data,
                             struct wl_simple_shell *wl_simple_shell,
                             uint32_t surfaceId,
                             const char *surfaceName);
    
    /**
     * surface_status - surface status notification
     * @surfaceId: the surface id
     * @surfaceName: name of the surface
     * @visible: visibility status
     * @x: x position
     * @y: y position
     * @width: width
     * @height: height
     * @opacity: opacity value
     * @zorder: z-order value
     */
    void (*surface_status)(void *data,
                          struct wl_simple_shell *wl_simple_shell,
                          uint32_t surfaceId,
                          const char *surfaceName,
                          uint32_t visible,
                          uint32_t x,
                          uint32_t y,
                          uint32_t width,
                          uint32_t height,
                          wl_fixed_t opacity,
                          wl_fixed_t zorder);
    
    /**
     * get_surfaces_done - completion of surface list
     */
    void (*get_surfaces_done)(void *data,
                             struct wl_simple_shell *wl_simple_shell);
};

/**
 * Add a listener for simple shell events
 */
static inline int
wl_simple_shell_add_listener(struct wl_simple_shell *wl_simple_shell,
                             const struct wl_simple_shell_listener *listener,
                             void *data)
{
    return wl_proxy_add_listener((struct wl_proxy *) wl_simple_shell,
                                 (void (**)(void)) listener, data);
}

/**
 * Set the name of a surface
 */
static inline void
wl_simple_shell_set_name(struct wl_simple_shell *wl_simple_shell,
                        uint32_t surfaceId,
                        const char *name)
{
    wl_proxy_marshal_flags((struct wl_proxy *) wl_simple_shell,
                          0, NULL, wl_proxy_get_version((struct wl_proxy *) wl_simple_shell), 0,
                          surfaceId, name);
}

/**
 * Set the geometry of a surface
 */
static inline void
wl_simple_shell_set_geometry(struct wl_simple_shell *wl_simple_shell,
                            uint32_t surfaceId,
                            uint32_t x,
                            uint32_t y,
                            uint32_t width,
                            uint32_t height)
{
    wl_proxy_marshal_flags((struct wl_proxy *) wl_simple_shell,
                          1, NULL, wl_proxy_get_version((struct wl_proxy *) wl_simple_shell), 0,
                          surfaceId, x, y, width, height);
}

/**
 * Get list of surfaces
 */
static inline void
wl_simple_shell_get_surfaces(struct wl_simple_shell *wl_simple_shell)
{
    wl_proxy_marshal_flags((struct wl_proxy *) wl_simple_shell,
                          2, NULL, wl_proxy_get_version((struct wl_proxy *) wl_simple_shell), 0);
}

/**
 * Get status of a surface
 */
static inline void
wl_simple_shell_get_status(struct wl_simple_shell *wl_simple_shell,
                          uint32_t surfaceId)
{
    wl_proxy_marshal_flags((struct wl_proxy *) wl_simple_shell,
                          3, NULL, wl_proxy_get_version((struct wl_proxy *) wl_simple_shell), 0,
                          surfaceId);
}

/**
 * Destroy simple shell
 */
static inline void
wl_simple_shell_destroy(struct wl_simple_shell *wl_simple_shell)
{
    wl_proxy_destroy((struct wl_proxy *) wl_simple_shell);
}

/**
 * Simple shell interface
 */
extern const struct wl_interface wl_simple_shell_interface;

#ifdef __cplusplus
}
#endif

#endif // SIMPLESHELL_CLIENT_PROTOCOL_H
