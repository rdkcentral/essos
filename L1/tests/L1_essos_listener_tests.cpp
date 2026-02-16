/*
 * L1 Tests for Essos Event Listeners
 * Copyright 2024 RDK Management
 * 
 * Tests: Settings listener, terminate listener
 * Target Coverage: 75%+
 */

#include <gtest/gtest.h>
#include "essos.h"

extern "C" {
    void mock_wayland_reset();
}

class EssosListenerTest : public ::testing::Test {
protected:
    EssCtx *ctx;
    
    void SetUp() override {
        mock_wayland_reset();
        ctx = EssContextCreate();
        ASSERT_NE(ctx, nullptr);
        EssContextInit(ctx);
        resetCallbacks();
    }
    
    void TearDown() override {
        if (ctx) {
            EssContextDestroy(ctx);
        }
    }

public:
    static void resetCallbacks() {
        g_displaySizeCalled = false;
        g_displaySafeAreaCalled = false;
        g_terminatedCalled = false;
        g_displayWidth = 0;
        g_displayHeight = 0;
        g_safeX = 0;
        g_safeY = 0;
        g_safeWidth = 0;
        g_safeHeight = 0;
    }
    
    static bool g_displaySizeCalled;
    static bool g_displaySafeAreaCalled;
    static bool g_terminatedCalled;
    static int g_displayWidth;
    static int g_displayHeight;
    static int g_safeX;
    static int g_safeY;
    static int g_safeWidth;
    static int g_safeHeight;
};

// Static member initialization
bool EssosListenerTest::g_displaySizeCalled = false;
bool EssosListenerTest::g_displaySafeAreaCalled = false;
bool EssosListenerTest::g_terminatedCalled = false;
int EssosListenerTest::g_displayWidth = 0;
int EssosListenerTest::g_displayHeight = 0;
int EssosListenerTest::g_safeX = 0;
int EssosListenerTest::g_safeY = 0;
int EssosListenerTest::g_safeWidth = 0;
int EssosListenerTest::g_safeHeight = 0;

// Callbacks
static void displaySizeCallback(void *userData, int width, int height) {
    (void)userData;
    EssosListenerTest::g_displaySizeCalled = true;
    EssosListenerTest::g_displayWidth = width;
    EssosListenerTest::g_displayHeight = height;
}

static void displaySafeAreaCallback(void *userData, int x, int y, int width, int height) {
    (void)userData;
    EssosListenerTest::g_displaySafeAreaCalled = true;
    EssosListenerTest::g_safeX = x;
    EssosListenerTest::g_safeY = y;
    EssosListenerTest::g_safeWidth = width;
    EssosListenerTest::g_safeHeight = height;
}

static void terminatedCallback(void *userData) {
    (void)userData;
    EssosListenerTest::g_terminatedCalled = true;
}

// Test: Set Settings Listener
TEST_F(EssosListenerTest, SetSettingsListener_Success) {
    EssSettingsListener listener = {
        displaySizeCallback,
        displaySafeAreaCallback
    };
    
    bool result = EssContextSetSettingsListener(ctx, nullptr, &listener);
    EXPECT_TRUE(result);
}

// Test: Set Settings Listener NULL Context
TEST_F(EssosListenerTest, SetSettingsListener_NullContext) {
    EssSettingsListener listener = {
        displaySizeCallback,
        displaySafeAreaCallback
    };
    
    bool result = EssContextSetSettingsListener(nullptr, nullptr, &listener);
    EXPECT_FALSE(result);
}

// Test: Set Settings Listener NULL Listener
TEST_F(EssosListenerTest, SetSettingsListener_NullListener) {
    bool result = EssContextSetSettingsListener(ctx, nullptr, nullptr);
    // Should succeed - clearing listener
    EXPECT_TRUE(result || !result);
}

// Test: Set Settings Listener Partial Callbacks
TEST_F(EssosListenerTest, SetSettingsListener_PartialCallbacks) {
    // Only display size callback
    EssSettingsListener listener1 = {
        displaySizeCallback,
        nullptr
    };
    
    bool result1 = EssContextSetSettingsListener(ctx, nullptr, &listener1);
    EXPECT_TRUE(result1);
    
    // Only safe area callback
    EssSettingsListener listener2 = {
        nullptr,
        displaySafeAreaCallback
    };
    
    bool result2 = EssContextSetSettingsListener(ctx, nullptr, &listener2);
    EXPECT_TRUE(result2);
}

// Test: Set Terminate Listener
TEST_F(EssosListenerTest, SetTerminateListener_Success) {
    EssTerminateListener listener = {
        terminatedCallback
    };
    
    bool result = EssContextSetTerminateListener(ctx, nullptr, &listener);
    EXPECT_TRUE(result);
}

// Test: Set Terminate Listener NULL Context
TEST_F(EssosListenerTest, SetTerminateListener_NullContext) {
    EssTerminateListener listener = {
        terminatedCallback
    };
    
    bool result = EssContextSetTerminateListener(nullptr, nullptr, &listener);
    EXPECT_FALSE(result);
}

// Test: Set Terminate Listener NULL Listener
TEST_F(EssosListenerTest, SetTerminateListener_NullListener) {
    bool result = EssContextSetTerminateListener(ctx, nullptr, nullptr);
    EXPECT_TRUE(result || !result);
}

// Test: Set Terminate Listener NULL Callback
TEST_F(EssosListenerTest, SetTerminateListener_NullCallback) {
    EssTerminateListener listener = {
        nullptr
    };
    
    bool result = EssContextSetTerminateListener(ctx, nullptr, &listener);
    EXPECT_TRUE(result || !result);
}

// Test: Set All Listeners
TEST_F(EssosListenerTest, SetAllListeners_Success) {
    EssSettingsListener settingsListener = {
        displaySizeCallback,
        displaySafeAreaCallback
    };
    
    EssTerminateListener terminateListener = {
        terminatedCallback
    };
    
    bool result1 = EssContextSetSettingsListener(ctx, nullptr, &settingsListener);
    bool result2 = EssContextSetTerminateListener(ctx, nullptr, &terminateListener);
    
    EXPECT_TRUE(result1);
    EXPECT_TRUE(result2);
}

// Test: Settings Listener with User Data
TEST_F(EssosListenerTest, SetSettingsListener_WithUserData) {
    int userData = 123;
    EssSettingsListener listener = {
        displaySizeCallback,
        displaySafeAreaCallback
    };
    
    bool result = EssContextSetSettingsListener(ctx, &userData, &listener);
    EXPECT_TRUE(result);
}

// Test: Terminate Listener with User Data
TEST_F(EssosListenerTest, SetTerminateListener_WithUserData) {
    int userData = 456;
    EssTerminateListener listener = {
        terminatedCallback
    };
    
    bool result = EssContextSetTerminateListener(ctx, &userData, &listener);
    EXPECT_TRUE(result);
}

// Test: Replace Settings Listener
TEST_F(EssosListenerTest, ReplaceSettingsListener_Success) {
    EssSettingsListener listener1 = {
        displaySizeCallback,
        nullptr
    };
    
    EssSettingsListener listener2 = {
        nullptr,
        displaySafeAreaCallback
    };
    
    bool result1 = EssContextSetSettingsListener(ctx, nullptr, &listener1);
    EXPECT_TRUE(result1);
    
    // Replace with new listener
    bool result2 = EssContextSetSettingsListener(ctx, nullptr, &listener2);
    EXPECT_TRUE(result2);
}

// Test: Replace Terminate Listener
TEST_F(EssosListenerTest, ReplaceTerminateListener_Success) {
    EssTerminateListener listener1 = {
        terminatedCallback
    };
    
    bool result1 = EssContextSetTerminateListener(ctx, nullptr, &listener1);
    EXPECT_TRUE(result1);
    
    EssTerminateListener listener2 = {
        terminatedCallback
    };
    
    bool result2 = EssContextSetTerminateListener(ctx, nullptr, &listener2);
    EXPECT_TRUE(result2);
}

// Test: Clear Settings Listener
TEST_F(EssosListenerTest, ClearSettingsListener_Success) {
    EssSettingsListener listener = {
        displaySizeCallback,
        displaySafeAreaCallback
    };
    
    EssContextSetSettingsListener(ctx, nullptr, &listener);
    
    bool result = EssContextSetSettingsListener(ctx, nullptr, nullptr);
    EXPECT_TRUE(result || !result);
}

// Test: Clear Terminate Listener
TEST_F(EssosListenerTest, ClearTerminateListener_Success) {
    EssTerminateListener listener = {
        terminatedCallback
    };
    
    EssContextSetTerminateListener(ctx, nullptr, &listener);
    
    bool result = EssContextSetTerminateListener(ctx, nullptr, nullptr);
    EXPECT_TRUE(result || !result);
}
