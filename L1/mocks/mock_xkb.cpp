/*
 * Mock XKB (X Keyboard) Implementation for L1 Testing
 * Copyright 2024 RDK Management
 */

#include <map>
#include <string>
#include <cstring>
#include <cstdlib>

extern "C" {
#include <xkbcommon/xkbcommon.h>
}

// Mock structures
struct xkb_context {
    int ref_count;
};

struct xkb_keymap {
    xkb_context *context;
    int ref_count;
};

struct xkb_state {
    xkb_keymap *keymap;
    int ref_count;
};

// Mock state
static std::map<void*, std::string> g_mock_xkb_objects;

extern "C" {

// Context functions
xkb_context* xkb_context_new(enum xkb_context_flags flags) {
    (void)flags;
    xkb_context *ctx = new xkb_context();
    ctx->ref_count = 1;
    g_mock_xkb_objects[ctx] = "xkb_context";
    return ctx;
}

xkb_context* xkb_context_ref(xkb_context *context) {
    if (context) {
        context->ref_count++;
    }
    return context;
}

void xkb_context_unref(xkb_context *context) {
    if (context) {
        context->ref_count--;
        if (context->ref_count <= 0) {
            g_mock_xkb_objects.erase(context);
            delete context;
        }
    }
}

// Keymap functions
xkb_keymap* xkb_keymap_new_from_string(xkb_context *context, const char *string,
                                       enum xkb_keymap_format format,
                                       enum xkb_keymap_compile_flags flags) {
    (void)string;
    (void)format;
    (void)flags;
    
    if (!context) {
        return nullptr;
    }
    
    xkb_keymap *keymap = new xkb_keymap();
    keymap->context = context;
    keymap->ref_count = 1;
    g_mock_xkb_objects[keymap] = "xkb_keymap";
    return keymap;
}

xkb_keymap* xkb_keymap_ref(xkb_keymap *keymap) {
    if (keymap) {
        keymap->ref_count++;
    }
    return keymap;
}

void xkb_keymap_unref(xkb_keymap *keymap) {
    if (keymap) {
        keymap->ref_count--;
        if (keymap->ref_count <= 0) {
            g_mock_xkb_objects.erase(keymap);
            delete keymap;
        }
    }
}

// State functions
xkb_state* xkb_state_new(xkb_keymap *keymap) {
    if (!keymap) {
        return nullptr;
    }
    
    xkb_state *state = new xkb_state();
    state->keymap = keymap;
    state->ref_count = 1;
    g_mock_xkb_objects[state] = "xkb_state";
    return state;
}

xkb_state* xkb_state_ref(xkb_state *state) {
    if (state) {
        state->ref_count++;
    }
    return state;
}

void xkb_state_unref(xkb_state *state) {
    if (state) {
        state->ref_count--;
        if (state->ref_count <= 0) {
            g_mock_xkb_objects.erase(state);
            delete state;
        }
    }
}

enum xkb_state_component xkb_state_update_mask(xkb_state *state,
                                               xkb_mod_mask_t depressed_mods,
                                               xkb_mod_mask_t latched_mods,
                                               xkb_mod_mask_t locked_mods,
                                               xkb_layout_index_t depressed_layout,
                                               xkb_layout_index_t latched_layout,
                                               xkb_layout_index_t locked_layout) {
    (void)state;
    (void)depressed_mods;
    (void)latched_mods;
    (void)locked_mods;
    (void)depressed_layout;
    (void)latched_layout;
    (void)locked_layout;
    return (xkb_state_component)0;
}

xkb_keysym_t xkb_state_key_get_one_sym(xkb_state *state, xkb_keycode_t key) {
    (void)state;
    // Return mock keysym based on keycode
    return key + 1000;
}

uint32_t xkb_keysym_to_utf32(xkb_keysym_t keysym) {
    // Simple mock mapping
    return keysym;
}

xkb_mod_index_t xkb_keymap_mod_get_index(struct xkb_keymap *keymap, const char *name) {
    (void)keymap;
    
    if (!name) {
        return XKB_MOD_INVALID;
    }
    
    // Return mock modifier indices for common modifiers
    if (strcmp(name, "Shift") == 0) return 0;
    if (strcmp(name, "Control") == 0) return 1;
    if (strcmp(name, "Alt") == 0) return 2;
    if (strcmp(name, "Mod1") == 0) return 2;
    if (strcmp(name, "Mod4") == 0) return 3;
    
    return XKB_MOD_INVALID;
}

} // extern "C"

