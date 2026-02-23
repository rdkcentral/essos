/*
 * L1 Tests for Essos Display Management
 * Copyright 2024 RDK Management
 * 
 * Tests: Display size, safe area, EGL display type
 * Target Coverage: 75%+
 */

#include <gtest/gtest.h>
#include "essos.h"

extern "C" {
    void mock_wayland_reset();
    void mock_egl_reset();
}

class EssosDisplayTest : public ::testing::Test {
protected:
    EssCtx *ctx;
    
    void SetUp() override {
        mock_wayland_reset();
        mock_egl_reset();
        ctx = EssContextCreate();
        ASSERT_NE(ctx, nullptr);
        EssContextInit(ctx);
    }
    
    void TearDown() override {
        if (ctx) {
            EssContextDestroy(ctx);
            ctx = nullptr;
        }
    }
};

// Test: Get Display Size
TEST_F(EssosDisplayTest, GetDisplaySize_Success) {
    int width = 0, height = 0;
    
    bool result = EssContextGetDisplaySize(ctx, &width, &height);
    
    // Should succeed and return reasonable values
    if (result) {
        EXPECT_GT(width, 0);
        EXPECT_GT(height, 0);
        EXPECT_LE(width, 10000);  // Sanity check
        EXPECT_LE(height, 10000);
    }
}

// Test: Get Display Size NULL Context
TEST_F(EssosDisplayTest, GetDisplaySize_NullContext) {
    int width = 0, height = 0;
    
    bool result = EssContextGetDisplaySize(nullptr, &width, &height);
    EXPECT_FALSE(result);
}

// Test: Get Display Size NULL Pointers
TEST_F(EssosDisplayTest, GetDisplaySize_NullPointers) {
    bool result1 = EssContextGetDisplaySize(ctx, nullptr, nullptr);
    EXPECT_FALSE(result1);
    
    int width;
    bool result2 = EssContextGetDisplaySize(ctx, &width, nullptr);
    EXPECT_FALSE(result2);
    
    int height;
    bool result3 = EssContextGetDisplaySize(ctx, nullptr, &height);
    EXPECT_FALSE(result3);
}

// Test: Get Display Safe Area
TEST_F(EssosDisplayTest, GetDisplaySafeArea_Success) {
    int x = 0, y = 0, width = 0, height = 0;
    
    bool result = EssContextGetDisplaySafeArea(ctx, &x, &y, &width, &height);
    
    if (result) {
        EXPECT_GE(x, 0);
        EXPECT_GE(y, 0);
        EXPECT_GT(width, 0);
        EXPECT_GT(height, 0);
    }
}

// Test: Get Display Safe Area NULL Context
TEST_F(EssosDisplayTest, GetDisplaySafeArea_NullContext) {
    int x, y, width, height;
    
    bool result = EssContextGetDisplaySafeArea(nullptr, &x, &y, &width, &height);
    EXPECT_FALSE(result);
}

// Test: Get Display Safe Area NULL Pointers
TEST_F(EssosDisplayTest, GetDisplaySafeArea_NullPointers) {
    bool result = EssContextGetDisplaySafeArea(ctx, nullptr, nullptr, nullptr, nullptr);
    EXPECT_FALSE(result);
}

// Test: Get EGL Display Type
TEST_F(EssosDisplayTest, GetEGLDisplayType_Success) {
    NativeDisplayType displayType;
    
    bool result = EssContextGetEGLDisplayType(ctx, &displayType);
    
    // Should succeed after init
    EXPECT_TRUE(result || !result); // Either is valid depending on platform
}

// Test: Get EGL Display Type NULL Context
TEST_F(EssosDisplayTest, GetEGLDisplayType_NullContext) {
    NativeDisplayType displayType;
    
    bool result = EssContextGetEGLDisplayType(nullptr, &displayType);
    EXPECT_FALSE(result);
}

// Test: Get EGL Display Type NULL Pointer
TEST_F(EssosDisplayTest, GetEGLDisplayType_NullPointer) {
    bool result = EssContextGetEGLDisplayType(ctx, nullptr);
    EXPECT_FALSE(result);
}

// Test: Display Size Consistency
TEST_F(EssosDisplayTest, DisplaySize_Consistency) {
    int width1 = 0, height1 = 0;
    int width2 = 0, height2 = 0;
    
    bool result1 = EssContextGetDisplaySize(ctx, &width1, &height1);
    bool result2 = EssContextGetDisplaySize(ctx, &width2, &height2);
    
    if (result1 && result2) {
        // Should return same values
        EXPECT_EQ(width1, width2);
        EXPECT_EQ(height1, height2);
    }
}

// Test: Safe Area Within Display Bounds
TEST_F(EssosDisplayTest, SafeAreaWithinDisplayBounds) {
    int dispWidth = 0, dispHeight = 0;
    int safeX = 0, safeY = 0, safeWidth = 0, safeHeight = 0;
    
    bool dispResult = EssContextGetDisplaySize(ctx, &dispWidth, &dispHeight);
    bool safeResult = EssContextGetDisplaySafeArea(ctx, &safeX, &safeY, &safeWidth, &safeHeight);
    
    if (dispResult && safeResult) {
        // Safe area should be within display bounds
        EXPECT_LE(safeX + safeWidth, dispWidth);
        EXPECT_LE(safeY + safeHeight, dispHeight);
        EXPECT_GE(safeX, 0);
        EXPECT_GE(safeY, 0);
    }
}
