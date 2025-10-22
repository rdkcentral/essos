/*
 * If not stated otherwise in this file or this component's Licenses.txt file the
 * following copyright and licenses apply:
 *
 * Copyright 2017 RDK Management
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef __ESSOS_APP__
#define __ESSOS_APP__

/* ----------------------------------------------------------------------------
 * Essos apps are single windowed portable applications that render graphics
 * using OpenGLES2.
 *
 * The API flow for a minimal app is:
 * EssContextCreate()
 * EssContextInit()
 * EssContextSetKeyListener()
 * EssContextSetPointerListener()
 * EssContextSetSettingsListener()
 * EssContextSetTerminateListener()
 * doEGLSetup() including:
 *   EssContextGetEGLDisplayType()
 *   EssContextCreateNativeWindow()
 * EssContextStart()
 *
 * then execute a main loop that includes
 *
 *   EssContextRunEventLoopOnce()
 *   doOpenGLES2Rendering()
 *   EssContextUpdateDisplay();
 *---------------------------------------------------------------------------- */
#include <EGL/egl.h>
#include <EGL/eglext.h>

#ifndef __cplusplus
#include <stdbool.h>
#endif

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct _EssCtx EssCtx;

/**
 * @brief Structure representing a key event listener.
 *
 * EssKeyListener provides a set of callback functions to handle key events.
 * Each function pointer can be set to handle the corresponding key event:
 * - keyPressed: Called when a key is pressed.
 * - keyReleased: Called when a key is released.
 * - keyRepeat: Called when a key is held down and repeats.
 *
 * @param userData Pointer to user-defined data passed to each callback.
 * @param key The key code associated with the event.
 */
typedef struct _EssKeyListener
{
   void (*keyPressed)( void *userData, unsigned int key ); 
   void (*keyReleased)( void *userData, unsigned int key ); 
   void (*keyRepeat)( void *userData, unsigned int key ); 
} EssKeyListener;

/**
 * @brief Structure for handling pointer (mouse/touch) events.
 *
 * EssPointerListener provides a set of callback functions to handle
 * pointer motion and button events. Each function pointer should be
 * assigned to handle the corresponding event.
 *
 * Members:
 * - pointerMotion: Called when the pointer moves.
 *      @param userData User-defined data pointer.
 *      @param x        X coordinate of the pointer.
 *      @param y        Y coordinate of the pointer.
 *
 * - pointerButtonPressed: Called when a pointer button is pressed.
 *      @param userData User-defined data pointer.
 *      @param button   Identifier of the button pressed.
 *      @param x        X coordinate of the pointer.
 *      @param y        Y coordinate of the pointer.
 *
 * - pointerButtonReleased: Called when a pointer button is released.
 *      @param userData User-defined data pointer.
 *      @param button   Identifier of the button released.
 *      @param x        X coordinate of the pointer.
 *      @param y        Y coordinate of the pointer.
 */
typedef struct _EssPointerListener
{
   void (*pointerMotion)( void *userData, int x, int y );
   void (*pointerButtonPressed)( void *userData, int button, int x, int y );
   void (*pointerButtonReleased)( void *userData, int button, int x, int y );
} EssPointerListener;

/**
 * @brief Struct for handling touch events in the Essos application.
 *
 * EssTouchListener provides a set of function pointers for responding to
 * various touch events such as touch down, touch up, touch motion, and touch frame.
 *
 * Members:
 * - touchDown:    Callback invoked when a touch is pressed down.
 *                 @param userData User-defined data pointer.
 *                 @param id       Identifier for the touch point.
 *                 @param x        X coordinate of the touch.
 *                 @param y        Y coordinate of the touch.
 *
 * - touchUp:      Callback invoked when a touch is released.
 *                 @param userData User-defined data pointer.
 *                 @param id       Identifier for the touch point.
 *
 * - touchMotion:  Callback invoked when a touch moves.
 *                 @param userData User-defined data pointer.
 *                 @param id       Identifier for the touch point.
 *                 @param x        New X coordinate of the touch.
 *                 @param y        New Y coordinate of the touch.
 *
 * - touchFrame:   Callback invoked at the end of a frame of touch events.
 *                 @param userData User-defined data pointer.
 */
typedef struct _EssTouchListener
{
   void (*touchDown)( void *userData, int id, int x, int y );
   void (*touchUp)( void *userData, int id );
   void (*touchMotion)( void *userData, int id, int x, int y );
   void (*touchFrame)( void *userData );
} EssTouchListener;

typedef struct _EssSettingsListener
{
   /**
    * @brief Callback function pointer for handling display size changes.
    *
    * This function is called when the display size changes, providing the new width and height.
    *
    * @param userData Pointer to user-defined data passed to the callback.
    * @param width The new width of the display.
    * @param height The new height of the display.
    */
   void (*displaySize)( void *userData, int width, int height );
   /*
    * If content rendered to the edges of the display are not guaranteed to be
    * visible, this callback will provide information on the region of the
    * application display surface that are guaranteed to be visible.  An application
    * can optionally use this information to, for example, draw a background image
    * that goes to the display edges but position important information within
    * the specified safe area. */

   /**
    * @brief Callback function pointer to handle display safe area updates.
    *
    * This function is called to notify about changes to the display's safe area.
    *
    * @param userData Pointer to user-defined data passed to the callback.
    * @param x The x-coordinate of the safe area's top-left corner.
    * @param y The y-coordinate of the safe area's top-left corner.
    * @param width The width of the safe area.
    * @param height The height of the safe area.
    */
   void (*displaySafeArea)( void *userData, int x, int y, int width, int height );
} EssSettingsListener;

typedef struct _EssTerminateListener
{
   /*
    * Called to notify the application that it should shutdown. If this
    * callback is invoked it means some abnormal condition has occurred and
    * the application should shutdown.
    */
   void (*terminated)( void *userData );
} EssTerminateListener;


/**
 * @enum EssAppPlatformDisplayType
 * @brief Enumerates the types of platform display backends supported by the application.
 *
 * This enum is used to specify the display platform type that the application
 * can interact with. It helps in selecting the appropriate display handling
 * mechanism based on the environment.
 *
 * @var EssAppPlatformDisplayType_direct
 *   Direct display type, typically used for direct rendering without a windowing system.
 *
 * @var EssAppPlatformDisplayType_wayland
 *   Wayland display type, used when running under a Wayland compositor.
 *
 * @var EssAppPlatformDisplayType_waylandExtension
 *   Wayland extension display type, used for extended Wayland features or protocols.
 */
typedef enum _EssAppPlatformDisplayType
{
   EssAppPlatformDisplayType_direct,
   EssAppPlatformDisplayType_wayland,
   EssAppPlatformDisplayType_waylandExtension
} EssAppPlatformDisplayType;

/**
 * @brief Creates and initializes a new EssCtx context.
 *
 * This function allocates and sets up a new EssCtx context, which is required
 * for using the Essos API. The caller is responsible for releasing the context
 * when it is no longer needed.
 *
 * @return Pointer to the newly created EssCtx context, or nullptr on failure.
 */
EssCtx* EssContextCreate();

/**
 * @brief Destroys the specified Essos context and releases all associated resources.
 *
 * This function should be called when the EssCtx context is no longer needed.
 * After calling this function, the context pointer should not be used.
 *
 * @param ctx Pointer to the EssCtx context to be destroyed.
 */
void EssContextDestroy( EssCtx *ctx );

/**
 * @brief Retrieves the detailed description of the last error that occurred in the given EssCtx context.
 *
 * This function returns a pointer to a string containing additional information about
 * the most recent error encountered in the specified EssCtx context. The returned string
 * is managed by the context and should not be freed or modified by the caller.
 *
 * @param ctx Pointer to the EssCtx context from which to retrieve the last error detail.
 * @return A constant character pointer to the error detail string, or NULL if no error has occurred.
 */
const char *EssContextGetLastErrorDetail( EssCtx *ctx );

/**
 * @brief Initializes the Essos context.
 *
 * This function sets up the provided EssCtx structure for use with the Essos library.
 * It must be called before any other operations are performed on the context.
 * Initialize an application context.  Inititialization will be performed
 * by EssContextStart but for use cases where it is not desired to start
 * an application context, EssContextInit must be called before methods
 * such as EssContextGetEGLDisplayType or ESSContextCreateNativeWindow
 * can be called.
 *
 * @param ctx Pointer to an EssCtx structure to initialize.
 * @return true if initialization was successful, false otherwise.
 */
bool EssContextInit( EssCtx *ctx );

/**
 * @brief Retrieves the EGL display type associated with the given EssCtx context.
 *
 * This function obtains the native display type (EGLDisplay) for the specified EssCtx context.
 * API is for applications that wish to create their EGL environment rather than allowing Essos
 * to do so automatically.
 *
 * @param[in]  ctx         Pointer to the EssCtx context from which to retrieve the display type.
 * @param[out] displayType Pointer to a NativeDisplayType variable that will receive the display type.
 * @return true if the display type was successfully retrieved; false otherwise.
 */
bool EssContextGetEGLDisplayType( EssCtx *ctx, NativeDisplayType *displayType );

/**
 * @brief Retrieves the platform display type associated with the given Essos context.
 *
 * This function returns the display type that the application is currently using,
 * as represented by the specified EssCtx context.
 *
 * @param ctx Pointer to the EssCtx context for which the platform display type is requested.
 * @return EssAppPlatformDisplayType The platform display type of the application.
 */
EssAppPlatformDisplayType EssContextGetAppPlatformDisplayType( EssCtx *ctx );

/**
 * @brief Creates a native window for the given Essos context.
 *
 * This function initializes and creates a native window associated with the specified
 * Essos context. The created window will have the specified width and height.
 *
 * @param ctx           Pointer to the Essos context.
 * @param width         Desired width of the native window.
 * @param h             Desired height of the native window.
 * @param nativeWindow  Pointer to a variable that will receive the created native window handle.
 *
 * @return true if the native window was successfully created, false otherwise.
 */
bool EssContextCreateNativeWindow( EssCtx *ctx, int width, int h, NativeWindowType *nativeWindow );

/**
 * @brief Destroys the specified native window associated with the given Ess context.
 *
 * This function releases any resources and performs necessary cleanup for the
 * native window identified by the provided handle within the specified Ess context.
 *
 * @param ctx Pointer to the EssCtx structure representing the application context.
 * @param nativeWindow Handle to the native window to be destroyed.
 * @return true if the native window was successfully destroyed; false otherwise.
 */
bool EssContextDestroyNativeWindow( EssCtx *ctx, NativeWindowType nativeWindow );

/**
 * @brief Sets a key event listener for the specified EssCtx context.
 *
 * Registers a key listener callback that will be invoked when key events occur
 * within the given context. The userData pointer will be passed to the listener
 * when it is called.
 * Key codes are Linux codes defined by linux/input.h
 *
 * @param ctx Pointer to the EssCtx context for which the key listener is to be set.
 * @param userData Pointer to user-defined data that will be passed to the listener callback.
 * @param listener Pointer to an EssKeyListener structure containing the callback functions.
 * @return true if the key listener was successfully set, false otherwise.
 */
bool EssContextSetKeyListener( EssCtx *ctx, void *userData, EssKeyListener *listener );

/**
 * @brief Sets the pointer event listener for the given EssCtx context.
 *
 * Registers a pointer event listener that will receive pointer events (such as mouse or touch)
 * for the specified context. The userData parameter will be passed to the listener callbacks.
 *
 * @param ctx        Pointer to the EssCtx context to associate the listener with.
 * @param userData   Pointer to user-defined data that will be passed to the listener callbacks.
 * @param listener   Pointer to an EssPointerListener structure containing callback functions for pointer events.
 * @return true if the listener was successfully set; false otherwise.
 */
bool EssContextSetPointerListener( EssCtx *ctx, void *userData, EssPointerListener *listener );

/**
 * @brief Sets a touch event listener for the specified Ess context.
 *
 * Registers a callback listener to receive touch events from the given Ess context.
 *
 * @param ctx Pointer to the Ess context.
 * @param userData Pointer to user-defined data that will be passed to the listener callbacks.
 * @param listener Pointer to an EssTouchListener structure containing the callback functions.
 * @return true if the listener was successfully set, false otherwise.
 */
bool EssContextSetTouchListener( EssCtx *ctx, void *userData, EssTouchListener *listener );


/**
 * @brief Sets the settings listener for the given EssCtx context.
 *
 * Registers a callback listener that will be notified of settings changes or events
 * related to the specified EssCtx context.
 *
 * @param ctx Pointer to the EssCtx context for which the listener is being set.
 * @param userData Pointer to user-defined data that will be passed to the listener callbacks.
 * @param listener Pointer to an EssSettingsListener structure containing the callback functions.
 * @return true if the listener was successfully set, false otherwise.
 */
bool EssContextSetSettingsListener( EssCtx *ctx, void *userData, EssSettingsListener *listener );

/**
 * @brief Sets a terminate listener for the given EssCtx context.
 *
 * Registers a callback function that will be invoked when the context is about to terminate.
 *
 * @param ctx Pointer to the EssCtx context for which the terminate listener is to be set.
 * @param userData Pointer to user-defined data that will be passed to the listener callback.
 * @param listener Pointer to the EssTerminateListener callback function to be invoked on termination.
 * @return true if the listener was successfully set, false otherwise.
 */
bool EssContextSetTerminateListener( EssCtx *ctx, void *userData, EssTerminateListener *listener );


/**
 * @brief Sets the name for the given EssCtx context.
 *
 * This function assigns the specified name to the provided EssCtx context.
 *
 * @param ctx  Pointer to the EssCtx context to be named.
 * @param name Null-terminated string representing the name to set.
 * @return true if the name was successfully set, false otherwise.
 */
bool EssContextSetName( EssCtx *ctx, const char *name );


/**
 * @brief Retrieves the current display size associated with the given EssCtx context.
 *
 * This function obtains the width and height of the display for the specified context.
 *
 * @param ctx    Pointer to the EssCtx context.
 * @param width  Pointer to an integer where the display width will be stored.
 * @param height Pointer to an integer where the display height will be stored.
 * @return true if the display size was successfully retrieved, false otherwise.
 */
bool EssContextGetDisplaySize( EssCtx *ctx, int *width, int *height );

/**
 * @brief Retrieves the display safe area for the given Essos context.
 *
 * This function obtains the coordinates and dimensions of the safe area on the display,
 * which is the region where content can be displayed without being clipped or obscured
 * by overscan or other display artifacts.
 *
 * @param ctx    Pointer to the Essos context.
 * @param x      Pointer to an integer to receive the x-coordinate of the safe area.
 * @param y      Pointer to an integer to receive the y-coordinate of the safe area.
 * @param width  Pointer to an integer to receive the width of the safe area.
 * @param height Pointer to an integer to receive the height of the safe area.
 * @return       true if the safe area was successfully retrieved, false otherwise.
 */
bool EssContextGetDisplaySafeArea( EssCtx *ctx, int *x, int *y, int *width, int *height );

/**
 * EssContextStart
 *
 * Start an application context running.  Context initialization will be performed by this call
 * if it has not already been done with EssContextInit. For applications that allow Essos to perform EGL
 * setup, the EGL environment will be active after calling this method. GLES2 rendering can then
 * be performed on this thread with buffer flips triggered by calls to EssContextUpdateDisplay. For
 * aoolications that manually perform EGL creation the EGL creation must be done between calling
 * EssContextInit and EssContextStart.  When manual EGL setup is done (EssContextCreateNativeWindow has
 * been called), EssContextStart will skip automatic EGL setup.  EssContextStart will also perform
 * setup required for user input and any other required setup.
 *
 * While running the EssContextRunEventLoop method must be regularly called.  
 * @param ctx Pointer to an EssCtx structure to be started.
 * @return true if the context was started successfully, false otherwise.
 */

bool EssContextStart( EssCtx *ctx );


/**
 * @brief Stops the given Essos context.
 *
 * This function halts all ongoing operations and releases any resources
 * associated with the specified EssCtx context.
 *
 * @param ctx Pointer to the EssCtx structure representing the context to stop.
 */
void EssContextStop( EssCtx *ctx );

/**
 * @brief Resizes the window associated with the given EssCtx context.
 *
 * Set a new window size.  This API may be called in response to a display size notification received
 * via an Essos settings listener.
 *
 * @param ctx    Pointer to the EssCtx context whose window will be resized.
 * @param width  The new width of the window in pixels.
 * @param height The new height of the window in pixels.
 * @return true if the window was successfully resized, false otherwise.
 */
bool EssContextResizeWindow( EssCtx *ctx, int width, int height );

/**
 * EssContextRunEventLoopOnce
 *
 * @brief Perform event processing.  This API will not block if no events are pending.
 * It must be called regularly while the aoplication is running.
 * @param ctx Pointer to the EssCtx context whose event loop should be processed.
 */

void EssContextRunEventLoopOnce( EssCtx *ctx);


/**
 * @brief Updates the display associated with the given EssCtx context.
 *
 * This function refreshes or redraws the display for the specified EssCtx context.
 * It should be called whenever the display needs to be updated to reflect changes
 * in the context's state.
 *
 * @param ctx Pointer to the EssCtx structure representing the current context.
 */
void EssContextUpdateDisplay( EssCtx *ctx );


#if defined(__cplusplus)
} //extern "C"
#endif
#endif

