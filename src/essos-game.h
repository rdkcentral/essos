/*
 * If not stated otherwise in this file or this component's Licenses.txt file the
 * following copyright and licenses apply:
 *
 * Copyright 2020 RDK Management
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

#ifndef __ESSOS_GAME__
#define __ESSOS_GAME__

#ifndef __cplusplus
#include <stdbool.h>
#endif

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct _EssGamepad EssGamepad;

/**
 * @brief Listener structure for gamepad connection events.
 *
 * This structure defines callback functions that are invoked when a gamepad is
 * connected or disconnected. Implementers can provide custom logic for handling
 * these events by assigning appropriate function pointers.
 */
typedef struct _EssGamepadConnectionListener
{
/**
 * @brief Listener for gamepad connection events.
 *
 * This structure defines callback functions that are invoked when a gamepad is connected or disconnected.
 *
 * @var connected
 * @param connected Called to notify an app that a gamepad has been connected
 *                  @param userData User-defined data pointer.
 *                  @param gp Pointer to the connected EssGamepad instance.
 * @var disconnected
 * @param disconnected Callback function called when a gamepad is disconnected.
 *                     @param userData User-defined data pointer.
 *                     @param gp Pointer to the disconnected EssGamepad instance.
 */
   void (*connected) ( void *userData, EssGamepad *gp );
   void (*disconnected) ( void *userData, EssGamepad *gp );
} EssGamepadConnectionListener;

typedef struct _EssGamepadEventListener
/**
 * @brief Struct containing callback function pointers for gamepad events.
 *
 * This struct allows the user to handle gamepad input events by providing
 * function pointers for button press, button release, and axis change events.
 * All id values used are defined in linux/input.h.
 *
 * @var buttonPressed
 *   Called when a gamepad button is pressed passing button id
 *   @param userData Pointer to user-defined data.
 *   @param buttonId Identifier of the button pressed.
 *
 * @var buttonReleased
 *   Called when a gamepad button is released passing button id
 *   @param userData Pointer to user-defined data.
 *   @param buttonId Identifier of the button released.
 *
 * @var axisChanged
 *   Called when a gamepad axis value changes passing the axis
 *   id and value.
 *   @param userData Pointer to user-defined data.
 *   @param axisId Identifier of the axis changed.
 *   @param value New value of the axis.
 */
{
   void (*buttonPressed)( void *userData, int buttonId );
   void (*buttonReleased)( void *userData, int buttonId );
   void (*axisChanged)( void *userData, int axisId, int value );
} EssGamepadEventListener;

/**
 * EssContextSetGamepadConnectionListener
 * @brief Set a gamepad connection listener (see EssGamepadConnectionListener) to receive 
 * gamepad connect/disconnect event callbacks.  The connected method will be invoked
 * immediately if any gamepads are connected at the time this listener is set.
 *
 * @param ctx Pointer to the EssCtx context.
 * @param userData Pointer to user-defined data that will be passed to the listener callback.
 * @param listener Pointer to the EssGamepadConnectionListener callback function.
 * @return true if the listener was successfully set, false otherwise.
 */
bool EssContextSetGamepadConnectionListener( EssCtx *ctx, void *userData, EssGamepadConnectionListener *listener );

/**
 * EssGamepadSetEventListener
 * @brief Sets the event listener for a specified EssGamepad instance.
 *
 * Registers a callback listener to receive gamepad events. The provided userData
 * pointer will be passed to the listener when events are triggered.
 * Gamepad definitions are Linux values defined by linux/input.h for buttons and absolute axes.
 *
 * @param gp Pointer to the EssGamepad instance for which the event listener is being set.
 * @param userData Pointer to user-defined data that will be passed to the event listener.
 * @param listener Pointer to the EssGamepadEventListener callback function to handle events.
 * @return true if the event listener was successfully set; false otherwise.
 */
bool EssGamepadSetEventListener( EssGamepad *gp, void *userData, EssGamepadEventListener *listener );

/**
 * EssGamepadGetDeviceName
 * @brief Retrieves the name of the device associated with the specified gamepad.
 *
 * @param gp Pointer to an EssGamepad structure representing the gamepad device.
 * @return A constant character pointer to the name of the gamepad device.
 */
const char *EssGamepadGetDeviceName( EssGamepad *gp );

/**
 * @brief Retrieves the driver version for the specified gamepad.
 *
 * This function returns the version number of the driver associated with the given
 * EssGamepad instance. The version is represented as an unsigned integer.
 *
 * @param gp Pointer to an EssGamepad structure representing the gamepad device.
 * @return The driver version as an unsigned integer.
 */
unsigned int EssGamepadGetDriverVersion( EssGamepad *gp );

/**
 * EssGamepadGetButtonMap
 *
 * @brief The gamepad button count and map.  The number of
 * buttons will be written to count and the map values will be
 * written to map.  Call with map set to NULL to get the count
 * of buttons, then allocate an array of int's and call again
 * to get the map values.  The map is an array of button id's
 * using values from linux/input.h.  The buttonPressed and buttonReleased
 * listener methods will be invoked passing these button id values.
 *
 * @param gp    Pointer to the EssGamepad instance.
 * @param count Pointer to an integer where the number of mapped buttons will be stored.
 * @param map   Pointer to an array where the button mapping will be written.
 * @return      true if the button mapping was successfully retrieved, false otherwise.
 */
bool EssGamepadGetButtonMap( EssGamepad *gp, int *count, int *map );

/**
 * EssGamepadGetAxisMap
 *
 * @brief Retrieve the gamepad axis count and map.  The number of axes
 * will be writtin to count and the map values will be written
 * to map.  Call with map set to NULL to get the count of axes,
 * then allocate an array of int's and call again to get the
 * the map values.  The map is an array of axis id's using values
 * from linux/input.h for absolute axes.  The axisChanged listener
 * methods will be invoked passing these axes id values.
 * 
 * @param gp    Pointer to the EssGamepad structure representing the gamepad.
 * @param count Pointer to an integer where the number of mapped axes will be stored.
 * @param map   Pointer to an array where the axis mapping will be stored.
 * @return      true if the axis mapping was successfully retrieved, false otherwise.
 */
bool EssGamepadGetAxisMap( EssGamepad *gp, int *count, int *map );

/**
 * EssGamepadGetState
 *
 * @brief Retrieve the current state of all buttons and axes.  The number
 * of buttons and axes should be retrieved via calls to  EssGamepadGetButtonMap
 * and EssGamepadGetAxisMap.
 *
 * @param gp           Pointer to the EssGamepad instance to query.
 * @param buttonState  Pointer to an integer where the button state bitmask will be stored.
 * @param axisState    Pointer to an integer where the axis state value will be stored.
 * @return true if the gamepad state was successfully retrieved, false otherwise.
 */
bool EssGamepadGetState( EssGamepad *gp, int *buttonState, int *axisState );

#if defined(__cplusplus)
} //extern "C"
#endif

#endif
