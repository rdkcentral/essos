#ifndef _POSIX_C_SOURCE
#define _POSIX_C_SOURCE 200809L
#endif

#include <gtest/gtest.h>

#include <cstdlib>
#include <linux/input.h>
#include <string>
#include <vector>

#include "essos.h"

extern "C" {
void EssContextTestGetDefaults(EssCtx *, int *, int *, int *, int *, int *, int *, int *, int *, int *, int *, int *);
void EssContextTestSetRunning(EssCtx *, bool);
void EssContextTestSetCustomSafeArea(EssCtx *, int, int, int, int, int, int);
void EssContextTestEmitKeyPressed(EssCtx *, int);
void EssContextTestEmitKeyReleased(EssCtx *, int);
void EssContextTestEmitKeyRepeat(EssCtx *, int);
void EssContextTestEmitPointerMotion(EssCtx *, int, int);
void EssContextTestEmitPointerPressed(EssCtx *, int);
void EssContextTestEmitPointerReleased(EssCtx *, int);
void EssContextTestEmitTouchDown(EssCtx *, int, int, int);
void EssContextTestEmitTouchUp(EssCtx *, int);
void EssContextTestEmitTouchMotion(EssCtx *, int, int, int);
void EssContextTestEmitTouchFrame(EssCtx *);
void EssContextTestEmitGamepadEvents(void *, EssGamepadEventListener *);
}

namespace {

class ScopedEnvironmentValue {
public:
   ScopedEnvironmentValue(const char *name, const char *value) : key(name) {
      const char *previous = std::getenv(name);
      hadPrevious = previous != nullptr;
      if (hadPrevious) previousValue = previous;
      if (value) setenv(name, value, 1);
      else unsetenv(name);
   }
   ~ScopedEnvironmentValue() {
      if (hadPrevious) setenv(key.c_str(), previousValue.c_str(), 1);
      else unsetenv(key.c_str());
   }
private:
   std::string key;
   std::string previousValue;
   bool hadPrevious = false;
};

class EssContextFixture : public ::testing::Test {
protected:
   void SetUp() override { ctx = EssContextCreate(); }
   void TearDown() override { EssContextDestroy(ctx); }
   EssCtx *ctx = nullptr;
};

struct EventRecord {
   std::vector<int> keysPressed;
   std::vector<int> keysReleased;
   std::vector<int> keysRepeated;
   std::vector<EssInputDeviceMetadata *> keyMetadata;
   std::vector<int> pointerMotion;
   std::vector<int> pointerPressed;
   std::vector<int> pointerReleased;
   std::vector<int> touchDown;
   std::vector<int> touchUp;
   std::vector<int> touchMotion;
   int touchFrames = 0;
   std::vector<int> gamepadPressed;
   std::vector<int> gamepadReleased;
   std::vector<int> gamepadAxes;
};

void keyPressed(void *data, unsigned int key) { static_cast<EventRecord *>(data)->keysPressed.push_back(key); }
void keyReleased(void *data, unsigned int key) { static_cast<EventRecord *>(data)->keysReleased.push_back(key); }
void keyRepeated(void *data, unsigned int key) { static_cast<EventRecord *>(data)->keysRepeated.push_back(key); }
void metadataKeyPressed(void *data, unsigned int key, EssInputDeviceMetadata *metadata) {
   auto *record = static_cast<EventRecord *>(data);
   record->keysPressed.push_back(key);
   record->keyMetadata.push_back(metadata);
}
void metadataKeyReleased(void *data, unsigned int key, EssInputDeviceMetadata *metadata) {
   auto *record = static_cast<EventRecord *>(data);
   record->keysReleased.push_back(key);
   record->keyMetadata.push_back(metadata);
}
void metadataKeyRepeated(void *data, unsigned int key, EssInputDeviceMetadata *metadata) {
   auto *record = static_cast<EventRecord *>(data);
   record->keysRepeated.push_back(key);
   record->keyMetadata.push_back(metadata);
}
void pointerMotion(void *data, int x, int y) { static_cast<EventRecord *>(data)->pointerMotion = {x, y}; }
void pointerPressed(void *data, int button, int x, int y) { static_cast<EventRecord *>(data)->pointerPressed = {button, x, y}; }
void pointerReleased(void *data, int button, int x, int y) { static_cast<EventRecord *>(data)->pointerReleased = {button, x, y}; }
void touchDown(void *data, int id, int x, int y) { static_cast<EventRecord *>(data)->touchDown = {id, x, y}; }
void touchUp(void *data, int id) { static_cast<EventRecord *>(data)->touchUp = {id}; }
void touchMotion(void *data, int id, int x, int y) { static_cast<EventRecord *>(data)->touchMotion = {id, x, y}; }
void touchFrame(void *data) { ++static_cast<EventRecord *>(data)->touchFrames; }
void gamepadPressed(void *data, int button) { static_cast<EventRecord *>(data)->gamepadPressed = {button}; }
void gamepadReleased(void *data, int button) { static_cast<EventRecord *>(data)->gamepadReleased = {button}; }
void gamepadAxis(void *data, int axis, int value) { static_cast<EventRecord *>(data)->gamepadAxes = {axis, value}; }

} // namespace

TEST_F(EssContextFixture, CreationProvidesDocumentedDefaults) {
   ASSERT_NE(ctx, nullptr);
   int width, height, safeX, safeY, safeW, safeH, delay, period, windowW, windowH, swap;
   EssContextTestGetDefaults(ctx, &width, &height, &safeX, &safeY, &safeW, &safeH,
                             &delay, &period, &windowW, &windowH, &swap);
   EXPECT_EQ(width, 1280);
   EXPECT_EQ(height, 720);
   EXPECT_EQ(safeX, 64);
   EXPECT_EQ(safeY, 36);
   EXPECT_EQ(safeW, 1152);
   EXPECT_EQ(safeH, 648);
   EXPECT_EQ(delay, 500);
   EXPECT_EQ(period, 100);
   EXPECT_EQ(windowW, 1280);
   EXPECT_EQ(windowH, 720);
   EXPECT_EQ(swap, 1);
   EXPECT_STREQ(EssContextGetLastErrorDetail(ctx), "");
}

TEST_F(EssContextFixture, NullContextIsRejectedByStatefulPublicApis) {
   EXPECT_FALSE(EssContextSetUseWayland(nullptr, true));
   EXPECT_FALSE(EssContextSetUseDirect(nullptr, true));
   EXPECT_FALSE(EssContextSetName(nullptr, "app"));
   EXPECT_FALSE(EssContextSetDisplayMode(nullptr, "1080p"));
   EXPECT_FALSE(EssContextInit(nullptr));
   EXPECT_FALSE(EssContextSetInitialWindowSize(nullptr, 800, 600));
   EXPECT_FALSE(EssContextSetSwapInterval(nullptr, 0));
   EXPECT_FALSE(EssContextSetEGLConfigAttributes(nullptr, nullptr, 0));
   EXPECT_FALSE(EssContextGetEGLConfigAttributes(nullptr, nullptr, nullptr));
   EXPECT_FALSE(EssContextSetEGLSurfaceAttributes(nullptr, nullptr, 0));
   EXPECT_FALSE(EssContextGetEGLSurfaceAttributes(nullptr, nullptr, nullptr));
   EXPECT_FALSE(EssContextSetEGLContextAttributes(nullptr, nullptr, 0));
   EXPECT_FALSE(EssContextGetEGLContextAttributes(nullptr, nullptr, nullptr));
   EXPECT_FALSE(EssContextSetKeyRepeatInitialDelay(nullptr, 1));
   EXPECT_FALSE(EssContextSetKeyRepeatPeriod(nullptr, 1));
   EXPECT_FALSE(EssContextSetDisplaySize(nullptr, 1, 1));
   EXPECT_FALSE(EssContextGetDisplaySize(nullptr, nullptr, nullptr));
   EXPECT_FALSE(EssContextGetDisplaySafeArea(nullptr, nullptr, nullptr, nullptr, nullptr));
   EXPECT_FALSE(EssContextGetEGLDisplayType(nullptr, nullptr));
   EXPECT_EQ(EssContextGetAppPlatformDisplayType(nullptr), EssAppPlatformDisplayType_direct);
   EXPECT_FALSE(EssContextSetKeyListener(nullptr, nullptr, nullptr));
   EXPECT_FALSE(EssContextSetKeyAndMetadataListener(nullptr, nullptr, nullptr, nullptr));
   EXPECT_FALSE(EssContextSetPointerListener(nullptr, nullptr, nullptr));
   EXPECT_FALSE(EssContextSetTouchListener(nullptr, nullptr, nullptr));
   EXPECT_FALSE(EssContextSetSettingsListener(nullptr, nullptr, nullptr));
   EXPECT_FALSE(EssContextSetTerminateListener(nullptr, nullptr, nullptr));
   EXPECT_FALSE(EssContextSetGamepadConnectionListener(nullptr, nullptr, nullptr));
   EXPECT_FALSE(EssContextSetWindowPosition(nullptr, 0, 0));
   EXPECT_FALSE(EssContextSetWindowFocus(nullptr));
   EssContextRunEventLoopOnce(nullptr);
   EXPECT_EQ(EssContextGetLastErrorDetail(nullptr), nullptr);
}

TEST_F(EssContextFixture, BackendSelectionAndDisplayModeValidation) {
   ASSERT_TRUE(EssContextSupportWayland(ctx));
   ASSERT_TRUE(EssContextSupportDirect(ctx));
   EXPECT_FALSE(EssContextGetUseWayland(ctx));
   EXPECT_TRUE(EssContextGetUseDirect(ctx));
   EXPECT_FALSE(EssContextSetDisplayMode(ctx, nullptr));
   EXPECT_NE(EssContextGetLastErrorDetail(ctx), nullptr);

   EXPECT_TRUE(EssContextSetUseWayland(ctx, true));
   EXPECT_TRUE(EssContextGetUseWayland(ctx));
   EXPECT_FALSE(EssContextGetUseDirect(ctx));
   EXPECT_TRUE(EssContextSetDisplayMode(ctx, "1920x1080p60"));
   EXPECT_TRUE(EssContextSetUseDirect(ctx, true));
   EXPECT_FALSE(EssContextGetUseWayland(ctx));
   EXPECT_TRUE(EssContextSetDisplayMode(ctx, "720p"));
}

TEST_F(EssContextFixture, NameAndModeCannotBeChangedAfterInitialization) {
   EXPECT_FALSE(EssContextSetName(ctx, nullptr));
   EXPECT_NE(EssContextGetLastErrorDetail(ctx), nullptr);
   ASSERT_TRUE(EssContextSetName(ctx, "essos-l1"));
   ASSERT_TRUE(EssContextSetUseDirect(ctx, true));
   ASSERT_TRUE(EssContextInit(ctx));
   EXPECT_FALSE(EssContextSetName(ctx, "late-name"));
   EXPECT_FALSE(EssContextSetUseWayland(ctx, true));
   EXPECT_NE(EssContextGetLastErrorDetail(ctx), nullptr);

   NativeDisplayType displayType = 0;
   EXPECT_TRUE(EssContextGetEGLDisplayType(ctx, &displayType));
   EXPECT_EQ(displayType, EGL_DEFAULT_DISPLAY);
   EXPECT_EQ(EssContextGetAppPlatformDisplayType(ctx), EssAppPlatformDisplayType_direct);
   EXPECT_FALSE(EssContextInit(ctx));
}

TEST_F(EssContextFixture, AutomaticModeSelectionFollowsWaylandEnvironment) {
   {
      ScopedEnvironmentValue wayland("WAYLAND_DISPLAY", "essos-test-display");
      ASSERT_TRUE(EssContextInit(ctx));
      EXPECT_TRUE(EssContextGetUseWayland(ctx));
   }

   EssContextDestroy(ctx);
   ctx = EssContextCreate();
   ASSERT_NE(ctx, nullptr);
   {
      ScopedEnvironmentValue noWayland("WAYLAND_DISPLAY", nullptr);
      ASSERT_TRUE(EssContextInit(ctx));
      EXPECT_FALSE(EssContextGetUseWayland(ctx));
   }
}

TEST_F(EssContextFixture, DisplayQueriesRequireInitializationAndSafeAreaTracksSize) {
   int width = -1, height = -1, x = -1, y = -1, safeW = -1, safeH = -1;
   EXPECT_FALSE(EssContextGetDisplaySize(ctx, &width, &height));
   EXPECT_FALSE(EssContextGetDisplaySafeArea(ctx, &x, &y, &safeW, &safeH));
   EXPECT_FALSE(EssContextSetDisplaySize(ctx, 1920, 1080));
   ASSERT_TRUE(EssContextSetUseDirect(ctx, true));
   ASSERT_TRUE(EssContextInit(ctx));

   EXPECT_TRUE(EssContextGetDisplaySize(ctx, &width, &height));
   EXPECT_EQ(width, 1280);
   EXPECT_EQ(height, 720);
   EXPECT_TRUE(EssContextGetDisplaySafeArea(ctx, &x, &y, &safeW, &safeH));
   EXPECT_EQ((std::vector<int>{x, y, safeW, safeH}), (std::vector<int>{64, 36, 1152, 648}));

   EXPECT_TRUE(EssContextSetDisplaySize(ctx, 1920, 1080));
   EXPECT_TRUE(EssContextGetDisplaySize(ctx, &width, &height));
   EXPECT_EQ((std::vector<int>{width, height}), (std::vector<int>{1920, 1080}));
   EXPECT_TRUE(EssContextGetDisplaySafeArea(ctx, &x, &y, &safeW, &safeH));
   EXPECT_EQ((std::vector<int>{x, y, safeW, safeH}), (std::vector<int>{96, 54, 1728, 972}));
   EXPECT_TRUE(EssContextGetDisplaySize(ctx, nullptr, nullptr));
   EXPECT_TRUE(EssContextGetDisplaySafeArea(ctx, nullptr, nullptr, nullptr, nullptr));
}

TEST_F(EssContextFixture, DisplaySizeIsIgnoredInWaylandMode) {
   ASSERT_TRUE(EssContextSetUseWayland(ctx, true));
   ASSERT_TRUE(EssContextInit(ctx));
   EXPECT_TRUE(EssContextSetDisplaySize(ctx, 1920, 1080));
   int width = 0, height = 0;
   ASSERT_TRUE(EssContextGetDisplaySize(ctx, &width, &height));
   EXPECT_EQ((std::vector<int>{width, height}), (std::vector<int>{1280, 720}));
}

TEST_F(EssContextFixture, CustomSafeAreaValuesArePreserved) {
   ASSERT_TRUE(EssContextSetUseDirect(ctx, true));
   ASSERT_TRUE(EssContextInit(ctx));
   EssContextTestSetCustomSafeArea(ctx, 1000, 700, 7, 11, 900, 600);
   int x = 0, y = 0, width = 0, height = 0;
   ASSERT_TRUE(EssContextGetDisplaySafeArea(ctx, &x, &y, &width, &height));
   EXPECT_EQ((std::vector<int>{x, y, width, height}), (std::vector<int>{7, 11, 900, 600}));
}

TEST_F(EssContextFixture, EglAttributeSettersRoundTripAndResetToDefaults) {
   EGLint config[] = {EGL_RED_SIZE, 5, EGL_NONE};
   EGLint surface[] = {EGL_NONE};
   EGLint context[] = {EGL_CONTEXT_CLIENT_VERSION, 3, EGL_NONE};
   EGLint *actual = nullptr;
   EGLint size = 0;

   EXPECT_TRUE(EssContextSetEGLConfigAttributes(ctx, config, 3));
   ASSERT_TRUE(EssContextGetEGLConfigAttributes(ctx, &actual, &size));
   EXPECT_EQ(actual, config);
   EXPECT_EQ(size, 3);
   EXPECT_FALSE(EssContextGetEGLConfigAttributes(ctx, nullptr, &size));
   EXPECT_FALSE(EssContextGetEGLConfigAttributes(ctx, &actual, nullptr));
   EXPECT_TRUE(EssContextSetEGLConfigAttributes(ctx, nullptr, 0));
   ASSERT_TRUE(EssContextGetEGLConfigAttributes(ctx, &actual, &size));
   EXPECT_EQ(size, 17);

   EXPECT_TRUE(EssContextSetEGLSurfaceAttributes(ctx, surface, 1));
   ASSERT_TRUE(EssContextGetEGLSurfaceAttributes(ctx, &actual, &size));
   EXPECT_EQ(actual, surface);
   EXPECT_EQ(size, 1);
   EXPECT_TRUE(EssContextSetEGLSurfaceAttributes(ctx, nullptr, 0));
   ASSERT_TRUE(EssContextGetEGLSurfaceAttributes(ctx, &actual, &size));
   EXPECT_EQ(size, 1);

   EXPECT_TRUE(EssContextSetEGLContextAttributes(ctx, context, 3));
   ASSERT_TRUE(EssContextGetEGLContextAttributes(ctx, &actual, &size));
   EXPECT_EQ(actual, context);
   EXPECT_EQ(size, 3);
   EXPECT_TRUE(EssContextSetEGLContextAttributes(ctx, nullptr, 0));
   ASSERT_TRUE(EssContextGetEGLContextAttributes(ctx, &actual, &size));
   EXPECT_EQ(size, 3);
}

TEST_F(EssContextFixture, WindowAndRepeatConfigurationCanBeUpdated) {
   int width, height, delay, period, swap;
   ASSERT_TRUE(EssContextSetInitialWindowSize(ctx, 900, 500));
   ASSERT_TRUE(EssContextSetSwapInterval(ctx, 0));
   ASSERT_TRUE(EssContextSetKeyRepeatInitialDelay(ctx, 250));
   ASSERT_TRUE(EssContextSetKeyRepeatPeriod(ctx, 30));
   EssContextTestGetDefaults(ctx, nullptr, nullptr, nullptr, nullptr, nullptr, nullptr,
                             &delay, &period, &width, &height, &swap);
   EXPECT_EQ((std::vector<int>{width, height, delay, period, swap}), (std::vector<int>{900, 500, 250, 30, 0}));

   EssContextTestSetRunning(ctx, true);
   EXPECT_TRUE(EssContextSetInitialWindowSize(ctx, 800, 450));
   EXPECT_FALSE(EssContextSetSwapInterval(ctx, 2));
   EXPECT_NE(EssContextGetLastErrorDetail(ctx), nullptr);
   EssContextTestSetRunning(ctx, false);
   EXPECT_TRUE(EssContextSetSwapInterval(ctx, 2));
}

TEST_F(EssContextFixture, WaylandDisplayTypeAndWindowConfigurationAreSafeWithoutCompositor) {
   ASSERT_TRUE(EssContextSetUseWayland(ctx, true));
   EXPECT_EQ(EssContextGetAppPlatformDisplayType(ctx), EssAppPlatformDisplayType_wayland);
   EXPECT_TRUE(EssContextSetWindowPosition(ctx, 13, 27));
   EXPECT_TRUE(EssContextSetWindowFocus(ctx));
   EXPECT_TRUE(EssContextSetInitialWindowSize(ctx, 640, 360));
}

TEST_F(EssContextFixture, SettingsTerminateAndGamepadConnectionListenersCanBeRegistered) {
   EssSettingsListener settings = {};
   EssTerminateListener terminate = {};
   EssGamepadConnectionListener gamepadConnection = {};
   EXPECT_TRUE(EssContextSetSettingsListener(ctx, nullptr, &settings));
   EXPECT_TRUE(EssContextSetTerminateListener(ctx, nullptr, &terminate));
   EXPECT_TRUE(EssContextSetGamepadConnectionListener(ctx, nullptr, &gamepadConnection));
   EXPECT_TRUE(EssContextSetSettingsListener(ctx, nullptr, nullptr));
   EXPECT_TRUE(EssContextSetTerminateListener(ctx, nullptr, nullptr));
   EXPECT_TRUE(EssContextSetGamepadConnectionListener(ctx, nullptr, nullptr));
}

TEST_F(EssContextFixture, KeyAndMetadataCallbacksReceiveEventsAndRepeatFallbackWorks) {
   EventRecord record;
   EssInputDeviceMetadata metadata = {};
   EssKeyListener listener = {keyPressed, keyReleased, keyRepeated};
   EssKeyAndMetadataListener metadataListener = {metadataKeyPressed, metadataKeyReleased, metadataKeyRepeated};
   ASSERT_TRUE(EssContextSetKeyListener(ctx, &record, &listener));
   ASSERT_TRUE(EssContextSetKeyAndMetadataListener(ctx, &record, &metadataListener, &metadata));

   EssContextTestEmitKeyPressed(ctx, KEY_ENTER);
   EssContextTestEmitKeyReleased(ctx, KEY_ENTER);
   EssContextTestEmitKeyRepeat(ctx, KEY_ENTER);
   EXPECT_EQ(record.keysPressed.size(), 2u);
   EXPECT_EQ(record.keysReleased.size(), 2u);
   EXPECT_EQ(record.keysRepeated.size(), 2u);
   EXPECT_EQ(record.keyMetadata.size(), 3u);
   EXPECT_EQ(record.keyMetadata.front(), &metadata);

   EssKeyListener fallbackListener = {keyPressed, keyReleased, nullptr};
   ASSERT_TRUE(EssContextSetKeyListener(ctx, &record, &fallbackListener));
   EssContextTestEmitKeyRepeat(ctx, KEY_SPACE);
   EXPECT_EQ(record.keysPressed.back(), KEY_SPACE);
   EssContextTestEmitKeyPressed(nullptr, KEY_ENTER);
}

TEST_F(EssContextFixture, PointerCallbacksUseLatestCoordinatesForButtonEvents) {
   EventRecord record;
   EssPointerListener listener = {pointerMotion, pointerPressed, pointerReleased};
   ASSERT_TRUE(EssContextSetPointerListener(ctx, &record, &listener));
   EssContextTestEmitPointerMotion(ctx, 101, 202);
   EssContextTestEmitPointerPressed(ctx, BTN_LEFT);
   EssContextTestEmitPointerReleased(ctx, BTN_RIGHT);
   EXPECT_EQ((std::vector<int>{record.pointerMotion[0], record.pointerMotion[1]}), (std::vector<int>{101, 202}));
   EXPECT_EQ((std::vector<int>{record.pointerPressed[0], record.pointerPressed[1], record.pointerPressed[2]}), (std::vector<int>{BTN_LEFT, 101, 202}));
   EXPECT_EQ((std::vector<int>{record.pointerReleased[0], record.pointerReleased[1], record.pointerReleased[2]}), (std::vector<int>{BTN_RIGHT, 101, 202}));
}

TEST_F(EssContextFixture, TouchCallbacksForwardDownUpMotionAndFrame) {
   EventRecord record;
   EssTouchListener listener = {touchDown, touchUp, touchMotion, touchFrame};
   ASSERT_TRUE(EssContextSetTouchListener(ctx, &record, &listener));
   EssContextTestEmitTouchDown(ctx, 4, 20, 30);
   EssContextTestEmitTouchUp(ctx, 4);
   EssContextTestEmitTouchMotion(ctx, 4, 21, 31);
   EssContextTestEmitTouchFrame(ctx);
   EXPECT_EQ((std::vector<int>{record.touchDown[0], record.touchDown[1], record.touchDown[2]}), (std::vector<int>{4, 20, 30}));
   EXPECT_EQ(record.touchUp, (std::vector<int>{4}));
   EXPECT_EQ((std::vector<int>{record.touchMotion[0], record.touchMotion[1], record.touchMotion[2]}), (std::vector<int>{4, 21, 31}));
   EXPECT_EQ(record.touchFrames, 1);
}

TEST_F(EssContextFixture, EventLoopCanBePumpedWithoutAPlatformBackend) {
   EssContextRunEventLoopOnce(ctx);
   EssContextRunEventLoopOnce(ctx);
   EssContextRunEventLoopOnce(nullptr);
}

TEST(EssosGamepadDispatchTest, ForwardsButtonAndAxisEventsToRegisteredListener) {
   EventRecord record;
   EssGamepadEventListener listener = {gamepadPressed, gamepadReleased, gamepadAxis};
   EssContextTestEmitGamepadEvents(&record, &listener);
   EXPECT_EQ(record.gamepadPressed, (std::vector<int>{BTN_A}));
   EXPECT_EQ(record.gamepadReleased, (std::vector<int>{BTN_B}));
   EXPECT_EQ(record.gamepadAxes, (std::vector<int>{ABS_X, -1234}));
}
