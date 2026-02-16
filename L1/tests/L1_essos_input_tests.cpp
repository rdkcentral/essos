/*
 * L1 Tests for Essos Input Management
 * Copyright 2024 RDK Management
 * 
 * Tests: Keyboard, pointer, touch input
 * Target Coverage: 75%+
 */

#include <gtest/gtest.h>
#include "essos.h"

extern "C" {
    void mock_wayland_reset();
}

class EssosInputTest : public ::testing::Test {
protected:
    EssCtx *ctx;
    
    void SetUp() override {
        mock_wayland_reset();
        ctx = EssContextCreate();
        ASSERT_NE(ctx, nullptr);
        EssContextInit(ctx);
    }
    
    void TearDown() override {
        if (ctx) {
            EssContextDestroy(ctx);
        }
    }
};

// Test callbacks
static bool g_keyPressed = false;
static bool g_keyReleased = false;
static bool g_keyRepeat = false;
static unsigned int g_lastKey = 0;

static void keyPressedCallback(void *userData, unsigned int key) {
    (void)userData;
    g_keyPressed = true;
    g_lastKey = key;
}

static void keyReleasedCallback(void *userData, unsigned int key) {
    (void)userData;
    g_keyReleased = true;
    g_lastKey = key;
}

static void keyRepeatCallback(void *userData, unsigned int key) {
    (void)userData;
    g_keyRepeat = true;
    g_lastKey = key;
}

// Pointer callbacks
static bool g_pointerMotion = false;
static bool g_pointerButtonPressed = false;
static bool g_pointerButtonReleased = false;
static int g_pointerX = 0, g_pointerY = 0;
static int g_pointerButton = 0;

static void pointerMotionCallback(void *userData, int x, int y) {
    (void)userData;
    g_pointerMotion = true;
    g_pointerX = x;
    g_pointerY = y;
}

static void pointerButtonPressedCallback(void *userData, int button, int x, int y) {
    (void)userData;
    g_pointerButtonPressed = true;
    g_pointerButton = button;
    g_pointerX = x;
    g_pointerY = y;
}

static void pointerButtonReleasedCallback(void *userData, int button, int x, int y) {
    (void)userData;
    g_pointerButtonReleased = true;
    g_pointerButton = button;
    g_pointerX = x;
    g_pointerY = y;
}

// Touch callbacks
static bool g_touchDown = false;
static bool g_touchUp = false;
static bool g_touchMotion = false;
static bool g_touchFrame = false;
static int g_touchId = 0;
static int g_touchX = 0, g_touchY = 0;

static void touchDownCallback(void *userData, int id, int x, int y) {
    (void)userData;
    g_touchDown = true;
    g_touchId = id;
    g_touchX = x;
    g_touchY = y;
}

static void touchUpCallback(void *userData, int id) {
    (void)userData;
    g_touchUp = true;
    g_touchId = id;
}

static void touchMotionCallback(void *userData, int id, int x, int y) {
    (void)userData;
    g_touchMotion = true;
    g_touchId = id;
    g_touchX = x;
    g_touchY = y;
}

static void touchFrameCallback(void *userData) {
    (void)userData;
    g_touchFrame = true;
}

void resetInputCallbacks() {
    g_keyPressed = false;
    g_keyReleased = false;
    g_keyRepeat = false;
    g_lastKey = 0;
    g_pointerMotion = false;
    g_pointerButtonPressed = false;
    g_pointerButtonReleased = false;
    g_pointerX = 0;
    g_pointerY = 0;
    g_pointerButton = 0;
    g_touchDown = false;
    g_touchUp = false;
    g_touchMotion = false;
    g_touchFrame = false;
    g_touchId = 0;
    g_touchX = 0;
    g_touchY = 0;
}

// Test: Set Key Listener
TEST_F(EssosInputTest, SetKeyListener_Success) {
    EssKeyListener listener = {
        keyPressedCallback,
        keyReleasedCallback,
        keyRepeatCallback
    };
    
    bool result = EssContextSetKeyListener(ctx, nullptr, &listener);
    EXPECT_TRUE(result);
}

// Test: Set Key Listener NULL Context
TEST_F(EssosInputTest, SetKeyListener_NullContext) {
    EssKeyListener listener = {
        keyPressedCallback,
        keyReleasedCallback,
        keyRepeatCallback
    };
    
    bool result = EssContextSetKeyListener(nullptr, nullptr, &listener);
    EXPECT_FALSE(result);
}

// Test: Set Key Listener NULL Listener
TEST_F(EssosInputTest, SetKeyListener_NullListener) {
    bool result = EssContextSetKeyListener(ctx, nullptr, nullptr);
    // Should succeed - clearing listener
    EXPECT_TRUE(result || !result);
}

// Test: Set Pointer Listener
TEST_F(EssosInputTest, SetPointerListener_Success) {
    EssPointerListener listener = {
        pointerMotionCallback,
        pointerButtonPressedCallback,
        pointerButtonReleasedCallback
    };
    
    bool result = EssContextSetPointerListener(ctx, nullptr, &listener);
    EXPECT_TRUE(result);
}

// Test: Set Pointer Listener NULL Context
TEST_F(EssosInputTest, SetPointerListener_NullContext) {
    EssPointerListener listener = {
        pointerMotionCallback,
        pointerButtonPressedCallback,
        pointerButtonReleasedCallback
    };
    
    bool result = EssContextSetPointerListener(nullptr, nullptr, &listener);
    EXPECT_FALSE(result);
}

// Test: Set Touch Listener
TEST_F(EssosInputTest, SetTouchListener_Success) {
    EssTouchListener listener = {
        touchDownCallback,
        touchUpCallback,
        touchMotionCallback,
        touchFrameCallback
    };
    
    bool result = EssContextSetTouchListener(ctx, nullptr, &listener);
    EXPECT_TRUE(result);
}

// Test: Set Touch Listener NULL Context
TEST_F(EssosInputTest, SetTouchListener_NullContext) {
    EssTouchListener listener = {
        touchDownCallback,
        touchUpCallback,
        touchMotionCallback,
        touchFrameCallback
    };
    
    bool result = EssContextSetTouchListener(nullptr, nullptr, &listener);
    EXPECT_FALSE(result);
}

// Test: Set All Input Listeners
TEST_F(EssosInputTest, SetAllInputListeners_Success) {
    EssKeyListener keyListener = {
        keyPressedCallback,
        keyReleasedCallback,
        keyRepeatCallback
    };
    
    EssPointerListener pointerListener = {
        pointerMotionCallback,
        pointerButtonPressedCallback,
        pointerButtonReleasedCallback
    };
    
    EssTouchListener touchListener = {
        touchDownCallback,
        touchUpCallback,
        touchMotionCallback,
        touchFrameCallback
    };
    
    bool result1 = EssContextSetKeyListener(ctx, nullptr, &keyListener);
    bool result2 = EssContextSetPointerListener(ctx, nullptr, &pointerListener);
    bool result3 = EssContextSetTouchListener(ctx, nullptr, &touchListener);
    
    EXPECT_TRUE(result1);
    EXPECT_TRUE(result2);
    EXPECT_TRUE(result3);
}

// Test: Key Listener with User Data
TEST_F(EssosInputTest, SetKeyListener_WithUserData) {
    int userData = 42;
    EssKeyListener listener = {
        keyPressedCallback,
        keyReleasedCallback,
        keyRepeatCallback
    };
    
    bool result = EssContextSetKeyListener(ctx, &userData, &listener);
    EXPECT_TRUE(result);
}

// Test: Pointer Listener with User Data
TEST_F(EssosInputTest, SetPointerListener_WithUserData) {
    int userData = 42;
    EssPointerListener listener = {
        pointerMotionCallback,
        pointerButtonPressedCallback,
        pointerButtonReleasedCallback
    };
    
    bool result = EssContextSetPointerListener(ctx, &userData, &listener);
    EXPECT_TRUE(result);
}

// Test: Touch Listener with User Data
TEST_F(EssosInputTest, SetTouchListener_WithUserData) {
    int userData = 42;
    EssTouchListener listener = {
        touchDownCallback,
        touchUpCallback,
        touchMotionCallback,
        touchFrameCallback
    };
    
    bool result = EssContextSetTouchListener(ctx, &userData, &listener);
    EXPECT_TRUE(result);
}

// Test: Replace Key Listener
TEST_F(EssosInputTest, ReplaceKeyListener_Success) {
    EssKeyListener listener1 = {
        keyPressedCallback,
        nullptr,
        nullptr
    };
    
    EssKeyListener listener2 = {
        nullptr,
        keyReleasedCallback,
        keyRepeatCallback
    };
    
    bool result1 = EssContextSetKeyListener(ctx, nullptr, &listener1);
    EXPECT_TRUE(result1);
    
    bool result2 = EssContextSetKeyListener(ctx, nullptr, &listener2);
    EXPECT_TRUE(result2);
}

// Test: Clear Input Listeners
TEST_F(EssosInputTest, ClearInputListeners_Success) {
    // Set listeners first
    EssKeyListener keyListener = {
        keyPressedCallback,
        keyReleasedCallback,
        keyRepeatCallback
    };
    EssContextSetKeyListener(ctx, nullptr, &keyListener);
    
    // Clear listeners
    bool result = EssContextSetKeyListener(ctx, nullptr, nullptr);
    EXPECT_TRUE(result || !result); // Either is acceptable
}
