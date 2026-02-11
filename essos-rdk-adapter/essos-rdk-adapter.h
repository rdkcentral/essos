#ifndef ESSOS_RDK_ADAPTER_H
#define ESSOS_RDK_ADAPTER_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * Essos to RDK Resource Type Mapping
 */
typedef enum {
    ESSOS_RES_VIDEO_DECODER = 0,
    ESSOS_RES_AUDIO_DECODER = 1,
    ESSOS_RES_FRONTEND = 2,
        ESSOS_RES_SVP_ALLOCATOR = 3
} EssosResourceType;

/**
 * Resource capability structure
 */
typedef struct {
    const char* key;
    const char* value;
} EssosCapability;

/**
 * Resource request structure
 */
typedef struct {
    EssosResourceType type;
    int priority;
    
    int usage;  // Resource usage flags
    int criteria;  // Resource criteria/constraints

    EssosCapability* capabilities;
    int capability_count;
    void* usage_info;  // Essos usage info
} EssosResourceRequest;

/**
 * Initialize adapter - creates RDK client
 * @param client_id_out Output: RDK client ID assigned
 * @return 0 on success, -1 on failure
 */
int essos_rdk_adapter_init(int* client_id_out);

/**
 * Terminate adapter - cleanup RDK client
 * @param client_id RDK client ID to terminate
 * @return 0 on success, -1 on failure
 */

int essos_rdk_adapter_term(int client_id);

/**
 * Request resource via RDK RM
 * @param client_id RDK client ID
 * @param request Resource request details
 * @param essos_resource_id Output: Essos resource ID
 * @return 0 on success, -1 on failure
 */
int essos_rdk_adapter_request_resource(
    int client_id,
    EssosResourceRequest* request,
    int* essos_resource_id
);


/**
* Check resource status
* @param client_id RDK client ID
* @param essos_resource_id Essos resource ID
* @param status Output: 1 if allocated, 0 if not
* @return 0 on success, -1 on failure
*/

int essos_rdk_adapter_get_status(int client_id, int essos_resource_id, int* status);

//NEWLY ADDED//
/**
* Check if a specific client ID is valid and connected
* @param client_id RDK client ID to check
* @return 1 if connected, 0 if not
*/
int essos_rdk_adapter_is_connected(int client_id);

/*
* Set notification callback for resource state changes
* @param client_id RDK client ID
* @param essos_resource_id Essos resource ID
* @param notify_callback Callback function to invoke on state changes
* @param user_data User data to pass to callback
* @return 0 on success, -1 on failure
*/
typedef void (*EssosRdkAdapterNotifyCB)(int resource_id, int state, void* user_data);
int essos_rdk_adapter_set_notify(int client_id, int essos_resource_id, EssosRdkAdapterNotifyCB notify_callback, void* user_data);

/**
 * Release resource via RDK RM
 * @param client_id RDK client ID
 * @param essos_resource_id Essos resource ID to release
 * @return 0 on success, -1 on failure
 */
int essos_rdk_adapter_release_resource(int client_id, int essos_resource_id);

/**
 * Check resource status
 * @param essos_resource_id Essos resource ID
 * @param status Output: 1 if allocated, 0 if not
 * @return 0 on success, -1 on failure
 */

//int essos_rdk_adapter_get_status(int essos_resource_id, int* status);

/**
 * Set notification callback for resource state changes
 * @param essos_resource_id Essos resource ID
 * @param notify_callback Callback function to invoke on state changes
 * @param user_data User data to pass to callback
 * @return 0 on success, -1 on failure
 */


//typedef void (*EssosRdkAdapterNotifyCB)(int resource_id, int state, void* user_data);
//int essos_rdk_adapter_set_notify(int essos_resource_id, EssosRdkAdapterNotifyCB notify_callback, void* user_data);
/*
 * Add resource to blacklist
 * @param type Resource type
 * @param resource_id Resource identifier
 * @return 0 on success, -1 on failure (or not supported)
 */
int essos_rdk_adapter_add_to_blacklist(
    int client_id,
    EssosResourceType type,
    int resource_id
);

/**
 * Remove resource from blacklist
 * @param type Resource type
 * @param resource_id Resource identifier
 * @return 0 on success, -1 on failure (or not supported)
 */
int essos_rdk_adapter_remove_from_blacklist(
    int client_id,
    EssosResourceType type,
    int resource_id
);

/**
 * Get blacklist state for a resource
 * @param type Resource type
 * @param resource_id Resource identifier
 * @param blacklisted Output: 1 if blacklisted, 0 if not
 * @return 0 on success, -1 on failure
 */
int essos_rdk_adapter_get_blacklist_state(
    EssosResourceType type,
    int resource_id,
    int* blacklisted
);

/**
 * Handle revoke notification from RDK RM
 * This is called when RDK RM requests resource revocation
 * @param essos_resource_id Essos resource ID being revoked
 * @param timeout_ms Timeout for graceful release (milliseconds)
 * @return 0 on success, -1 on failure
 */
int essos_rdk_adapter_handle_revoke(
    int essos_resource_id,
    int timeout_ms
);

#ifdef __cplusplus
}
#endif

#endif // ESSOS_RDK_ADAPTER_H
