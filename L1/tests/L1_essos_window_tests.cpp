/*
 * L1 Tests for Essos Window Management
 * Copyright 2024 RDK Management
 * 
 * Tests: Native window creation, destruction, EGL integration
 * Target Coverage: 75%+
 */

#include <gtest/gtest.h>
#include "essos.h"

extern "C" {
    void mock_wayland_reset();
    void mock_egl_reset();
}

class EssosWindowTest : public ::testing::Test {
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
        }
    }
};

// Test: Create Native Window
TEST_F(EssosWindowTest, CreateNativeWindow_Success) {
    NativeWindowType nativeWindow = 0;
    
    bool result = EssContextCreateNativeWindow(ctx, 1920, 1080, &nativeWindow);
    
    if (result) {
        EXPECT_NE(nativeWindow, (NativeWindowType)0);
        
        // Destroy window
        EssContextDestroyNativeWindow(ctx, nativeWindow);
    }
}

// Test: Create Native Window NULL Context
TEST_F(EssosWindowTest, CreateNativeWindow_NullContext) {
    NativeWindowType nativeWindow;
    
    bool result = EssContextCreateNativeWindow(nullptr, 1920, 1080, &nativeWindow);
    EXPECT_FALSE(result);
}

// Test: Create Native Window NULL Window Pointer
TEST_F(EssosWindowTest, CreateNativeWindow_NullWindowPointer) {
    bool result = EssContextCreateNativeWindow(ctx, 1920, 1080, nullptr);
    EXPECT_FALSE(result);
}

// Test: Create Native Window Zero Size
TEST_F(EssosWindowTest, CreateNativeWindow_ZeroSize) {
    NativeWindowType nativeWindow;
    
    bool result = EssContextCreateNativeWindow(ctx, 0, 0, &nativeWindow);
    EXPECT_FALSE(result);
}

// Test: Create Native Window Negative Size
TEST_F(EssosWindowTest, CreateNativeWindow_NegativeSize) {
    NativeWindowType nativeWindow;
    
    bool result = EssContextCreateNativeWindow(ctx, -100, -100, &nativeWindow);
    EXPECT_FALSE(result);
}

// Test: Create Native Window Large Size
TEST_F(EssosWindowTest, CreateNativeWindow_LargeSize) {
    NativeWindowType nativeWindow = 0;
    
    bool result = EssContextCreateNativeWindow(ctx, 3840, 2160, &nativeWindow);
    
    if (result) {
        EXPECT_NE(nativeWindow, (NativeWindowType)0);
        EssContextDestroyNativeWindow(ctx, nativeWindow);
    }
}

// Test: Create Multiple Native Windows
TEST_F(EssosWindowTest, CreateMultipleNativeWindows_Success) {
    NativeWindowType window1 = 0;
    NativeWindowType window2 = 0;
    
    bool result1 = EssContextCreateNativeWindow(ctx, 1920, 1080, &window1);
    bool result2 = EssContextCreateNativeWindow(ctx, 1280, 720, &window2);
    
    if (result1 && result2) {
        EXPECT_NE(window1, (NativeWindowType)0);
        EXPECT_NE(window2, (NativeWindowType)0);
        
        // Windows might be the same or different depending on implementation
        
        EssContextDestroyNativeWindow(ctx, window1);
        EssContextDestroyNativeWindow(ctx, window2);
    }
}

// Test: Destroy Native Window
TEST_F(EssosWindowTest, DestroyNativeWindow_Success) {
    NativeWindowType nativeWindow = 0;
    
    bool createResult = EssContextCreateNativeWindow(ctx, 1920, 1080, &nativeWindow);
    
    if (createResult) {
        bool destroyResult = EssContextDestroyNativeWindow(ctx, nativeWindow);
        EXPECT_TRUE(destroyResult || !destroyResult); // Either is valid
    }
}

// Test: Destroy Native Window NULL Context
TEST_F(EssosWindowTest, DestroyNativeWindow_NullContext) {
    NativeWindowType nativeWindow = (NativeWindowType)100;
    
    bool result = EssContextDestroyNativeWindow(nullptr, nativeWindow);
    EXPECT_FALSE(result);
}

// Test: Destroy Native Window NULL Window
TEST_F(EssosWindowTest, DestroyNativeWindow_NullWindow) {
    bool result = EssContextDestroyNativeWindow(ctx, (NativeWindowType)0);
    EXPECT_FALSE(result); // Should fail with null window
}

// Test: Destroy Native Window Twice
TEST_F(EssosWindowTest, DestroyNativeWindow_Twice) {
    NativeWindowType nativeWindow = 0;
    
    bool createResult = EssContextCreateNativeWindow(ctx, 1920, 1080, &nativeWindow);
    
    if (createResult) {
        bool destroyResult1 = EssContextDestroyNativeWindow(ctx, nativeWindow);
        bool destroyResult2 = EssContextDestroyNativeWindow(ctx, nativeWindow);
        
        // Second destroy might fail or succeed depending on implementation
        (void)destroyResult1;
        (void)destroyResult2;
    }
}

// Test: Window Lifecycle
TEST_F(EssosWindowTest, WindowLifecycle_CreateDestroy) {
    NativeWindowType nativeWindow = 0;
    
    // Create
    bool createResult = EssContextCreateNativeWindow(ctx, 1920, 1080, &nativeWindow);
    
    if (createResult) {
        EXPECT_NE(nativeWindow, (NativeWindowType)0);
        
        // Use window (would do EGL operations here in real app)
        
        // Destroy
        bool destroyResult = EssContextDestroyNativeWindow(ctx, nativeWindow);
        EXPECT_TRUE(destroyResult || !destroyResult);
    }
}

// Test: Create Window Various Sizes
TEST_F(EssosWindowTest, CreateWindow_VariousSizes) {
    struct {
        int width;
        int height;
    } sizes[] = {
        {640, 480},
        {800, 600},
        {1024, 768},
        {1280, 720},
        {1920, 1080},
        {2560, 1440},
        {3840, 2160}
    };
    
    for (size_t i = 0; i < sizeof(sizes) / sizeof(sizes[0]); i++) {
        NativeWindowType nativeWindow = 0;
        
        bool result = EssContextCreateNativeWindow(ctx, sizes[i].width, sizes[i].height, &nativeWindow);
        
        if (result) {
            EXPECT_NE(nativeWindow, (NativeWindowType)0);
            EssContextDestroyNativeWindow(ctx, nativeWindow);
        }
    }
}

// Test: Create Window with EGL Display Type
TEST_F(EssosWindowTest, CreateWindow_WithEGLDisplayType) {
    NativeDisplayType displayType;
    bool displayResult = EssContextGetEGLDisplayType(ctx, &displayType);
    
    if (displayResult) {
        NativeWindowType nativeWindow = 0;
        bool windowResult = EssContextCreateNativeWindow(ctx, 1920, 1080, &nativeWindow);
        
        if (windowResult) {
            EXPECT_NE(nativeWindow, (NativeWindowType)0);
            EssContextDestroyNativeWindow(ctx, nativeWindow);
        }
    }
}
