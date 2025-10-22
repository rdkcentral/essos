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

#ifndef __ESSOS_SYSTEM__
#define __ESSOS_SYSTEM__

#ifndef __cplusplus
#include <stdbool.h>
#endif

#if defined(__cplusplus)
extern "C" {
#endif

#include <linux/input.h>

/**
 * EssContextSupportWayland
 * @brief Checks if the given EssCtx context supports the Wayland display protocol.
 * To configure the application to run as a Wayland application call EssContextSetUseWayland.
 *
 * @param ctx Pointer to the EssCtx context to be checked.
 * @return true if Wayland is supported by the context, false otherwise.
 */
bool EssContextSupportWayland( EssCtx *ctx );

/**
 * @brief Checks if the given EssCtx context supports direct mode.
 *
 * This function determines whether the specified EssCtx context has support
 * for direct operations or features. The exact meaning of "direct" depends
 * on the implementation details of EssCtx. By default a newly
 * created context will be configurd to run as a direct
 * EGL application.
 *
 * @param ctx Pointer to an EssCtx context structure.
 * @return Returns true if the context supports running as a
 * normal native EGL applicaiton, false otherwise.
 */
bool EssContextSupportDirect( EssCtx *ctx );

/**
 * EssContextSetUseWayland
 * @brief Sets whether the EssCtx should use Wayland.
 *
 * This function configures the given EssCtx context to use or not use the Wayland display server protocol.
 * Configure an application context to run as a Wayland
 * application.  This must be called before initializing or 
 * starting the application.
 *
 * @param ctx Pointer to the EssCtx context to configure.
 * @param useWayland Set to true to enable Wayland usage, or false to disable it.
 * @return true if the operation was successful, false otherwise.
 */
bool EssContextSetUseWayland( EssCtx *ctx, bool useWayland );

/**
 * EssContextGetUseWayland
 * @brief Checks if the Essos context is using the Wayland display system.
 *
 * @param ctx Pointer to the EssCtx context structure.
 * @return true if the application context is configured to run as a Wayland application, false otherwise.
 */
bool EssContextGetUseWayland( EssCtx *ctx );

/**
 * EssContextSetUseDirect
 * @brief Sets whether the Ess context should use direct mode.
 *
 * This function configures the specified Ess context to enable or disable
 * the use of direct mode, based on the value of the useDirect parameter.
 * This must be called before initializing or starting the application.
 *
 * @param ctx Pointer to the EssCtx context to be configured.
 * @param useDirect Boolean flag indicating whether to enable (true) or disable (false) direct mode.
 * @return true if the operation was successful, false otherwise.
 */
bool EssContextSetUseDirect( EssCtx *ctx, bool useDirect );

/**
 * EssContextGetUseDirect
 * 
 * @brief Checks if the Ess context is configured to use direct mode.
 *
 * This function queries the specified EssCtx context to determine whether
 * the "direct" mode is enabled. Direct mode may alter how the context
 * interacts with underlying system resources or rendering paths.
 *
 * @param ctx Pointer to the EssCtx context to query.
 * @return true if direct mode is enabled, false otherwise.
 */
bool EssContextGetUseDirect( EssCtx *ctx );

/**
 * EssContextSetDisplayMode
 *
 * @brief Available only on devices where westeros-gl supports mode setting.
 *
 * Set the active display mode. The mode is specified by a string
 * with the format "[wx]h[p|i][[x]r]".  For example:
 * 1920x1080i60
 * 720p
 * 1080i
 * 3840x2160
 * 1920x1080x24
 *
 *
 * @param ctx  Pointer to the EssCtx context to configure.
 * @param mode Null-terminated string specifying the desired display mode.
 * @return true if the display mode was set successfully, false otherwise.
 */
bool EssContextSetDisplayMode( EssCtx *ctx, const char *mode );

/**
 * EssContextGetWaylandDisplay
 *
 * @brief Retrieves the Wayland display associated with the given EssCtx context.
 *
 * This function returns a pointer to the Wayland display object for the specified
 * EssCtx context. The returned pointer should be cast to the appropriate Wayland
 * display type (e.g., struct wl_display *) by the caller.
 *
 * @param ctx Pointer to the EssCtx context from which to obtain the Wayland display.
 * @return A pointer to the Wayland display, or nullptr if unavailable.
 */
void* EssContextGetWaylandDisplay( EssCtx *ctx );

/**
 * EssContextSetDisplaySize
 * @brief Sets the display size for the given EssCtx context.
 *
 * This function updates the width and height of the display associated with the specified context.
 * Used to inform Essos of the display size for a direct EGL application,  For a Wayland application
 * the source of the display size is the compositor and this call will be ignored,
 *
 * @param ctx    Pointer to the EssCtx context whose display size is to be set.
 * @param width  The desired width of the display in pixels.
 * @param height The desired height of the display in pixels.
 * @return true if the display size was successfully set; false otherwise.
 */
bool EssContextSetDisplaySize( EssCtx *ctx, int width, int height );

/**
 * EssContextSetInitialWindowSize
 *
 * @brief Sets the initial window size for the given Essos context.
 *
 * This function configures the initial width and height of the window
 * associated with the specified EssCtx context. It should be called before
 * the window is created or shown to ensure the desired size is applied.
 *
 * @param ctx    Pointer to the EssCtx context.
 * @param width  Desired initial width of the window, in pixels.
 * @param height Desired initial height of the window, in pixels.
 * @return true if the window size was set successfully, false otherwise.
 */
bool EssContextSetInitialWindowSize( EssCtx *ctx, int width, int height );

/**
 * EssContextSetWindowPosition
 *
 * @brief Sets the window position.  For a Wayland application, this will
 * set the window position if the application is not a fullscreen
 * application (ie. connected to a Westeros embedded compositor).  For a
 * direct Linux application this call will be ignored.
 *
 * @param ctx Pointer to the EssCtx context whose window position is to be set.
 * @param x The new horizontal position (in pixels) of the window.
 * @param y The new vertical position (in pixels) of the window.
 * @return true if the window position was successfully updated, false otherwise.
 */
bool EssContextSetWindowPosition( EssCtx *ctx, int x, int y );

/**
 * EssContextSetKeyRepeatInitialDelay
 * 
 * @brief Sets the initial delay for key repeat events in the given Essos context.
 *
 * This function configures the amount of time (in milliseconds) that must elapse
 * after a key is pressed and held before key repeat events begin to be generated.
 *
 * @param ctx   Pointer to the Essos context.
 * @param delay Initial delay in milliseconds before key repeat starts.
 * @return true if the delay was set successfully, false otherwise.
 */
 bool EssContextSetKeyRepeatInitialDelay( EssCtx *ctx, int delay );

/**
 * EssContextSetKeyRepeatPeriod
 * 
 * @brief Sets the key repeat period for the given Essos context.
 *
 * This function configures the interval, in milliseconds, between repeated key events
 * when a key is held down within the specified Essos context.
 *
 * @param ctx    Pointer to the Essos context.
 * @param period The repeat period in milliseconds.
 * @return true if the period was set successfully, false otherwise.
 */
 bool EssContextSetKeyRepeatPeriod( EssCtx *ctx, int period );

/**
 * EssContextSetSwapInterval
 * @brief Sets the swap interval for the given Essos rendering context.
 *
 * The swap interval determines the minimum number of vertical retraces
 * between buffer swaps. A value of 1 synchronizes buffer swaps to the
 * vertical refresh (vsync), while 0 disables synchronization.
 * The default interval is 1.
 *
 * @param ctx Pointer to the Essos rendering context.
 * @param swapInterval The desired swap interval (typically 0 or 1).
 * @return true if the swap interval was set successfully, false otherwise.
 */
bool EssContextSetSwapInterval( EssCtx *ctx, EGLint swapInterval );

/**
 * EssContextSetEGLConfigAttributes
 * @brief Sets the EGL configuration attributes for the given EssCtx context.
 *
 * Specifies a set of EGL surface attributes to be used as constraints
 * when choosing an EGL config. The attributes are provided as an array of EGLint values.
 * This call can be made to replace the default attributes used by Essos.
 *
 * @param ctx   Pointer to the EssCtx context to configure.
 * @param attrs Pointer to an array of EGLint attributes to set.
 * @param size  The number of elements in the attrs array.
 * @return true if the attributes were successfully set; false otherwise.
 */
bool EssContextSetEGLConfigAttributes( EssCtx *ctx, EGLint *attrs, EGLint size );

/**
 * EssContextGetEGLConfigAttributes
 * 
 * @brief Retrieves the EGL configuration attributes for the given EssCtx context.
 *
 * This function obtains the list of EGL configuration attributes associated with the specified
 * EssCtx context. The attributes are returned as an array of EGLint values, and the number of
 * attributes is provided via the size parameter.
 *
 * @param ctx   Pointer to the EssCtx context from which to retrieve the EGL configuration attributes.
 * @param attrs Output pointer to an array of EGLint attributes. The function will set this pointer
 *              to reference the attribute array.
 * @param size  Output pointer to an EGLint variable where the function will store the number of
 *              attributes in the array.
 * @return      true if the attributes were successfully retrieved; false otherwise.
 */
bool EssContextGetEGLConfigAttributes( EssCtx *ctx, EGLint **attrs, EGLint *size );

/**
 * EssContextSetEGLSurfaceAttributes
 * 
 * @brief This function applies the specified set of EGL surface attributes to be used when creating
 * an EGL surface.  This call can be made to replace the default
 * attributes used by Essos.
 *
 * @param ctx   Pointer to the EssCtx context to configure.
 * @param attrs Pointer to an array of EGLint attributes to set.
 * @param size  Number of elements in the attrs array.
 * @return true if the attributes were successfully set; false otherwise.
 */
bool EssContextSetEGLSurfaceAttributes( EssCtx *ctx, EGLint *attrs, EGLint size );

/**
 * EssContextGetEGLSurfaceAttributes
 *
 * @brief Retrieves the EGL surface attributes for the given EssCtx context.
 *
 * This function obtains the EGL surface attributes associated with the specified
 * EssCtx context. It populates the provided pointer with an array of EGLint attributes
 * and sets the size to the number of attributes in the array.
 *
 * @param ctx   Pointer to the EssCtx context from which to retrieve EGL surface attributes.
 * @param attrs Output pointer to an array of EGLint attributes. The caller should not modify
 *              or free this array.
 * @param size  Output pointer to an EGLint variable that will be set to the number of attributes.
 *
 * @return true if the attributes were successfully retrieved, false otherwise.
 */
bool EssContextGetEGLSurfaceAttributes( EssCtx *ctx, EGLint **attrs, EGLint *size );

/**
 * EssContextSetEGLContextAttributes
 *
 * @brief Sets the EGL context attributes for the given EssCtx context.
 *
 * This function configures the EGL context attributes for the specified EssCtx context
 * using the provided array of attributes.
 * This call can be made to replace the default attributes used by Essos.
 *
 * @param ctx   Pointer to the EssCtx context to configure.
 * @param attrs Pointer to an array of EGLint attributes to set.
 * @param size  Number of elements in the attrs array.
 * @return true if the attributes were successfully set, false otherwise.
 */
bool EssContextSetEGLContextAttributes( EssCtx *ctx, EGLint *attrs, EGLint size );

/**
 * EssContextSetEGLContextAttributes
 *
 * @brief Retrieves the EGL context attributes for the given EssCtx context.
 *
 * This function obtains the list of EGL attributes associated with the specified EssCtx context.
 * The attributes are returned as an array of EGLint values, and the number of attributes is provided.
 *
 * @param ctx   Pointer to the EssCtx context from which to retrieve EGL attributes.
 * @param attrs Output pointer to an array of EGLint attributes. The caller should not free this array.
 * @param size  Output pointer to the number of attributes in the array.
 * @return true if the attributes were successfully retrieved, false otherwise.
 */
bool EssContextGetEGLContextAttributes( EssCtx *ctx, EGLint **attrs, EGLint *size );

/**
 * @struct EssInputDeviceMetadata
 * @brief Metadata structure for an input device.
 *
 * This structure holds information about an input device, including its device number,
 * physical address, identification, and filter code.
 *
 * @var EssInputDeviceMetadata::deviceNumber
 * Device number (typically a unique identifier for the device).
 *
 * @var EssInputDeviceMetadata::devicePhysicalAddress
 * Pointer to a string containing the physical address of the device.
 *
 * @var EssInputDeviceMetadata::id
 * Input device identification information.
 *
 * @var EssInputDeviceMetadata::filterCode
 * Filter code associated with the device (purpose may vary by context).
 */
typedef struct _EssInputDeviceMetadata
{
    dev_t deviceNumber;
    char * devicePhysicalAddress;
    input_id id;
    uint8_t filterCode;
} EssInputDeviceMetadata;

/**
 * @brief Listener structure for key events with associated metadata.
 *
 * This structure defines callback functions to handle key press, release, and repeat events,
 * each providing additional input device metadata.
 *
 * @member keyPressed  Callback invoked when a key is pressed.
 *                     @param userData   Pointer to user-defined data.
 *                     @param key        The key code of the pressed key.
 *                     @param metadata   Pointer to input device metadata.
 *
 * @member keyReleased  Callback invoked when a key is released.
 *                     @param userData   Pointer to user-defined data.
 *                     @param key        The key code of the released key.
 *                     @param metadata   Pointer to input device metadata.
 *
 * @member keyRepeat    Callback invoked when a key is held down and repeats.
 *                     @param userData   Pointer to user-defined data.
 *                     @param key        The key code of the repeated key.
 *                     @param metadata   Pointer to input device metadata.
 */
typedef struct _EssKeyAndMetadataListener
{
   void (*keyPressed)( void *userData, unsigned int key, EssInputDeviceMetadata *metadata );
   void (*keyReleased)( void *userData, unsigned int key, EssInputDeviceMetadata *metadata );
   void (*keyRepeat)( void *userData, unsigned int key, EssInputDeviceMetadata *metadata );
} EssKeyAndMetadataListener;

/**
 * EssContextSetKeyAndMetadataListener
 * 
 * @brief Sets the key and metadata listener for the given EssCtx context.
 *
 * Key codes are Linux codes defined by linux/input.h.
 * Registers a listener to receive key events and associated metadata from the specified context.
 *
 * @param ctx Pointer to the EssCtx context for which the listener is being set.
 * @param userData Pointer to user-defined data that will be passed to the listener callbacks.
 * @param listener Pointer to an EssKeyAndMetadataListener structure containing callback functions.
 * @param metadata Pointer to an EssInputDeviceMetadata structure providing additional device metadata.
 * @return true if the listener was successfully set; false otherwise.
 */
bool EssContextSetKeyAndMetadataListener( EssCtx *ctx, void *userData, EssKeyAndMetadataListener *listener, EssInputDeviceMetadata *metadata );


#if defined(__cplusplus)
} //extern "C"
#endif
#endif

