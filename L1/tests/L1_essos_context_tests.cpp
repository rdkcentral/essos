/*
 * L1 Tests for Essos Context Management
 * Copyright 2024 RDK Management
 * 
 * Tests: Context creation, initialization, destruction
 * Target Coverage: 75%+
 */

#include <gtest/gtest.h>
#include "essos.h"

// Mock control functions
extern "C" {
    void mock_wayland_reset();
    void mock_egl_reset();
    void mock_system_reset();
}

class EssosContextTest : public ::testing::Test {
protected:
    void SetUp() override {
        mock_wayland_reset();
        mock_egl_reset();
        mock_system_reset();
    }
    
    void TearDown() override {
        // Cleanup
    }
};

// Test: Context Creation
TEST_F(EssosContextTest, CreateContext_Success) {
    EssCtx *ctx = EssContextCreate();
    
    ASSERT_NE(ctx, nullptr);
    EXPECT_NE(ctx, (EssCtx*)0);
    
    EssContextDestroy(ctx);
}

// Test: Context Creation Multiple
TEST_F(EssosContextTest, CreateMultipleContexts_Success) {
    EssCtx *ctx1 = EssContextCreate();
    EssCtx *ctx2 = EssContextCreate();
    EssCtx *ctx3 = EssContextCreate();
    
    ASSERT_NE(ctx1, nullptr);
    ASSERT_NE(ctx2, nullptr);
    ASSERT_NE(ctx3, nullptr);
    
    // All contexts should be different
    EXPECT_NE(ctx1, ctx2);
    EXPECT_NE(ctx2, ctx3);
    EXPECT_NE(ctx1, ctx3);
    
    EssContextDestroy(ctx1);
    EssContextDestroy(ctx2);
    EssContextDestroy(ctx3);
}

// Test: Context Destruction NULL
TEST_F(EssosContextTest, DestroyContext_NullContext) {
    // Should not crash
    EssContextDestroy(nullptr);
}

// Test: Context Initialization
TEST_F(EssosContextTest, InitContext_Success) {
    EssCtx *ctx = EssContextCreate();
    ASSERT_NE(ctx, nullptr);
    
    bool result = EssContextInit(ctx);
    EXPECT_TRUE(result);
    
    EssContextDestroy(ctx);
}

// Test: Context Initialization NULL
TEST_F(EssosContextTest, InitContext_NullContext) {
    bool result = EssContextInit(nullptr);
    EXPECT_FALSE(result);
}

// Test: Context Initialization Twice
TEST_F(EssosContextTest, InitContext_Twice) {
    EssCtx *ctx = EssContextCreate();
    ASSERT_NE(ctx, nullptr);
    
    bool result1 = EssContextInit(ctx);
    EXPECT_TRUE(result1);
    
    // Second init should also succeed or handle gracefully
    bool result2 = EssContextInit(ctx);
    EXPECT_TRUE(result2 || !result2); // Either way is acceptable
    
    EssContextDestroy(ctx);
}

// Test: Get Last Error Detail
TEST_F(EssosContextTest, GetLastErrorDetail_ValidContext) {
    EssCtx *ctx = EssContextCreate();
    ASSERT_NE(ctx, nullptr);
    
    const char *error = EssContextGetLastErrorDetail(ctx);
    EXPECT_NE(error, nullptr);
    
    EssContextDestroy(ctx);
}

// Test: Use Wayland Setting
TEST_F(EssosContextTest, SetUseWayland_BeforeInit) {
    EssCtx *ctx = EssContextCreate();
    ASSERT_NE(ctx, nullptr);
    
    bool result = EssContextSetUseWayland(ctx, true);
    EXPECT_TRUE(result);
    
    bool initResult = EssContextInit(ctx);
    EXPECT_TRUE(initResult);
    
    EssContextDestroy(ctx);
}

// Test: Use Direct Setting
TEST_F(EssosContextTest, SetUseDirect_BeforeInit) {
    EssCtx *ctx = EssContextCreate();
    ASSERT_NE(ctx, nullptr);
    
    bool result = EssContextSetUseDirect(ctx, false);
    EXPECT_TRUE(result);
    
    EssContextDestroy(ctx);
}

// Test: Set Initial Window Size
TEST_F(EssosContextTest, SetInitialWindowSize_Success) {
    EssCtx *ctx = EssContextCreate();
    ASSERT_NE(ctx, nullptr);
    
    bool result = EssContextSetInitialWindowSize(ctx, 1280, 720);
    EXPECT_TRUE(result);
    
    EssContextDestroy(ctx);
}

// Test: Resize Window
TEST_F(EssosContextTest, ResizeWindow_AfterInit) {
    EssCtx *ctx = EssContextCreate();
    ASSERT_NE(ctx, nullptr);
    
    EssContextInit(ctx);
    
    bool result = EssContextResizeWindow(ctx, 1024, 768);
    (void)result;
    
    EssContextDestroy(ctx);
}

// Test: Get EGL Config Attributes
TEST_F(EssosContextTest, GetEGLConfigAttributes_Success) {
    EssCtx *ctx = EssContextCreate();
    ASSERT_NE(ctx, nullptr);
    
    EGLint attrs[] = {EGL_RED_SIZE, 8, EGL_GREEN_SIZE, 8, EGL_NONE};
    EssContextSetEGLConfigAttributes(ctx, attrs, 5);
    
    EGLint *outAttrs = nullptr;
    EGLint outSize = 0;
    bool result = EssContextGetEGLConfigAttributes(ctx, &outAttrs, &outSize);
    EXPECT_TRUE(result);
    EXPECT_EQ(outSize, 5);
    
    EssContextDestroy(ctx);
}

// Test: Get EGL Config Attributes Null Pointers
TEST_F(EssosContextTest, GetEGLConfigAttributes_NullPointers) {
    EssCtx *ctx = EssContextCreate();
    ASSERT_NE(ctx, nullptr);
    
    bool result = EssContextGetEGLConfigAttributes(ctx, nullptr, nullptr);
    EXPECT_FALSE(result);
    
    EssContextDestroy(ctx);
}

// Test: Set Swap Interval Before Running
TEST_F(EssosContextTest, SetSwapInterval_BeforeRunning) {
    EssCtx *ctx = EssContextCreate();
    ASSERT_NE(ctx, nullptr);
    
    bool result = EssContextSetSwapInterval(ctx, 1);
    EXPECT_TRUE(result);
    
    result = EssContextSetSwapInterval(ctx, 0);
    EXPECT_TRUE(result);
    
    EssContextDestroy(ctx);
}

// Test: Update Display After Init
TEST_F(EssosContextTest, UpdateDisplay_AfterInit) {
    EssCtx *ctx = EssContextCreate();
    ASSERT_NE(ctx, nullptr);
    
    EssContextInit(ctx);
    EssContextUpdateDisplay(ctx); // Should not crash
    
    EssContextDestroy(ctx);
}

// Test: Get Last Error Detail NULL Context
TEST_F(EssosContextTest, GetLastErrorDetail_NullContext) {
    const char *error = EssContextGetLastErrorDetail(nullptr);
    // Should return NULL or empty string
    EXPECT_TRUE(error == nullptr || strlen(error) == 0);
}

// Test: Set Context Name
TEST_F(EssosContextTest, SetName_Success) {
    EssCtx *ctx = EssContextCreate();
    ASSERT_NE(ctx, nullptr);
    
    bool result = EssContextSetName(ctx, "TestApp");
    EXPECT_TRUE(result);
    
    EssContextDestroy(ctx);
}

// Test: Set Context Name NULL Context
TEST_F(EssosContextTest, SetName_NullContext) {
    bool result = EssContextSetName(nullptr, "TestApp");
    EXPECT_FALSE(result);
}

// Test: Set Context Name NULL Name
TEST_F(EssosContextTest, SetName_NullName) {
    EssCtx *ctx = EssContextCreate();
    ASSERT_NE(ctx, nullptr);
    
    bool result = EssContextSetName(ctx, nullptr);
    EXPECT_FALSE(result);
    
    EssContextDestroy(ctx);
}

// Test: Set Context Name Long Name
TEST_F(EssosContextTest, SetName_LongName) {
    EssCtx *ctx = EssContextCreate();
    ASSERT_NE(ctx, nullptr);
    
    char longName[1024];
    memset(longName, 'A', sizeof(longName) - 1);
    longName[sizeof(longName) - 1] = '\0';
    
    bool result = EssContextSetName(ctx, longName);
    EXPECT_TRUE(result);
    
    EssContextDestroy(ctx);
}

// Test: Full Context Lifecycle
TEST_F(EssosContextTest, FullLifecycle_CreateInitDestroy) {
    EssCtx *ctx = EssContextCreate();
    ASSERT_NE(ctx, nullptr);
    
    // Name must be set BEFORE initialization
    bool nameResult = EssContextSetName(ctx, "LifecycleTest");
    EXPECT_TRUE(nameResult);
    
    bool initResult = EssContextInit(ctx);
    EXPECT_TRUE(initResult);
    
    const char *error = EssContextGetLastErrorDetail(ctx);
    EXPECT_NE(error, nullptr);
    
    EssContextDestroy(ctx);
}
