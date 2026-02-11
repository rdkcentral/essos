/*
 * If not stated otherwise in this file or this component's Licenses.txt file the
 * following copyright and licenses apply:
 *
 * Copyright 2025 RDK Management
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

/*
 * essos-resmgr-rdk.cpp
 *
 * RDK Resource Manager adapter implementation for Essos
 * This file integrates the Essos Resource Manager with the RDK Resource Manager
 * by routing all resource requests through the essos-rdk-adapter.
 */
 
#include "essos-rdk-adapter/essos-rdk-adapter.h"
#include "essos-resmgr.h"
#include <map>
#include <vector>
#include <algorithm>
#include <string.h>
#include <string>
#include <iostream>

#include <pthread.h>

//EssRMgr structure definition for RDK implementation
// Note: This is a minimal structure for RDK. The full structure is defined
// in the UDS/SHM implementations, but we only need these fields for RDK.
struct _EssRMgr
{
    pthread_mutex_t mutex;
    void *conn;  // Not used in RDK, but kept for compatibility
    int nextRequestId;
    int rdk_client_id;  // RDK client ID for this Essos session
};

// Global state tracking
static bool g_rdk_rm_initialized = false;
static std::map<int, EssRMgrRequest*> g_active_requests;  // resource_id -> request


// Force a log message at initialization to verify RDK code is compiled in
// This will appear even if log level is low
static void __attribute__((constructor)) essos_rdk_verify_compiled(void) {
    fprintf(stderr, "[Essos-RDK] RDK Resource Manager implementation is COMPILED and LOADED\n");
    fflush(stderr);
}


// Initialize RDK adapter
static bool essRMInit( EssRMgr *rm )
{
	INFO("Chirag essRMInit +");
#if 0
	if (g_rdk_rm_initialized) 
    {
        INFO("RDK RM already initialized");
        return true;
    }
#endif	
    INFO("Calling chirag essos_rdk_adapter_init() from essRMinit()");    
    int client_id = -1;
    int ret = essos_rdk_adapter_init(&client_id);     
    INFO("Calling chirag essos_rdk_adapter_init() from essRMinit() ret is %d, client_id=%d" , ret, client_id);
    if (ret != 0) {
        ERROR("Failed to initialize RDK adapter");
        return false;
    }
   
    g_rdk_rm_initialized = true;
    INFO("RDK RM adapter initialized successfully");
    return true;
}
 
// Terminate RDK adapter
static void essRMTerm( EssRMgr *rm )
{
#if 0
	if (!g_rdk_rm_initialized) {
        return;
    
	}
#endif	
   
    // Clean up active requests
    for (auto& pair : g_active_requests) {
        if (pair.second) {
            free(pair.second);
        }
    }
    g_active_requests.clear();
   
    //essos_rdk_adapter_term();
     // Note: essRMTerm is legacy and doesn't have client_id context
    // Use EssRMgrDestroy instead which properly terminates the client
    // essos_rdk_adapter_term() requires client_id parameter
    g_rdk_rm_initialized = false;
   
    INFO("Chirag RDK RM adapter terminated (legacy function - use EssRMgrDestroy)");
}
 
// Notification callback from adapter
static void essRMNotifyCallback(int resource_id, int state, void* user_data)
{
	INFO("Notify CB ess RMN Chirag");
    auto it = g_active_requests.find(resource_id);
    if (it != g_active_requests.end() && it->second) {
        EssRMgrRequest* req = it->second;
       
        // Update request state based on RDK notification
        //if (state == 1) {  // Resource granted
        //    req->assignedId = resource_id;
        //    req->assignedCaps = req->info.capabilities;
	
	// Update request state based on RDK notification
        if (state == 1) {  // Resource granted
            req->assignedId = resource_id;
            // assignedCaps will be set based on resource type capabilities
            // For now, set to 0 - can be enhanced later based on RDK response
            req->assignedCaps = 0;
           
        //    if (req->notify) {
        //        req->notify(req->notifyUserData);
        //    }
        //} else if (state == 0) {  // Resource revoked
        //    req->assignedId = -1;
        //    req->assignedCaps = 0;
	//
	if (req->notifyCB) {
                req->notifyCB(NULL, EssRMgrEvent_granted, req->type, resource_id, req->notifyUserData);
            }
        } else if (state == 0) {  // Resource revoked
            req->assignedId = -1;
            req->assignedCaps = 0;
           
       //     if (req->notify) {
       //         req->notify(req->notifyUserData);
       //     }
       if (req->notifyCB) {
                req->notifyCB(NULL, EssRMgrEvent_revoked, req->type, resource_id, req->notifyUserData);
            }
        }
    }
}
 
// Request resource via RDK adapter
static bool essRMRequestResource( EssRMgr *rm, int type, EssRMgrRequest *req )
{
    if (!rm || !req) {
        ERROR("Invalid parameters");
        return false;
    }
   INFO("EssRMReqRes Chirag Entry");
#if 0 
    if (!g_rdk_rm_initialized) {
        ERROR("RDK RM not initialized");
        return false;
    }
#endif
   
    // Prepare adapter request
    EssosResourceRequest adapter_req;
    memset(&adapter_req, 0, sizeof(adapter_req));
   /*
    adapter_req.type = (EssosResourceType)type;
    adapter_req.priority = req->priority;
    adapter_req.usage = req->info.usage;
    adapter_req.criteria = req->criteria;
    adapter_req.capability_count = 0;
    adapter_req.capabilities = NULL;
    adapter_req.usage_info = &req->info;
   */
    adapter_req.type = (EssosResourceType)type;
    adapter_req.priority = req->priority;
    adapter_req.usage = req->usage;  // usage is a direct member of EssRMgrRequest
    adapter_req.criteria = 0;  // criteria not available in EssRMgrRequest, set to 0
    //adapter_req.capability_count = 0;
    //adapter_req.capabilities = NULL;
    adapter_req.usage_info = &req->info;

    // Convert Essos capabilities to adapter format if needed
    // --- Capability extraction and mapping ---
    // Capability extraction for all resource types
    EssosCapability caps[5];
    int cap_count = 0;
    memset(caps, 0, sizeof(caps));
 
    switch (type) {
        case EssRMgrResType_videoDecoder:
            caps[cap_count].key = "maxWidth";
            caps[cap_count].value = "0";
            cap_count++;

            caps[cap_count].key = "maxHeight";
            caps[cap_count].value = "0";
            cap_count++;
            
            ERROR("chirag EssRMgrResType_videoDecoder %d ", cap_count);
            break;
        case EssRMgrResType_audioDecoder:
            caps[cap_count].key = "none";
            caps[cap_count].value = "none";
            cap_count++;
            ERROR("chirag EssRMgrResType_audioDecoder %d ", cap_count);
            break;
        case EssRMgrResType_frontEnd:
            // Example: map frontend capabilities (bitmask) to key/value
            //if (req->info.frontEnd.capabilities & EssRMgrFECap_none) {
                caps[cap_count].key = "none";
                caps[cap_count].value = "1";
                cap_count++;
            //}
            break;
        case EssRMgrResType_svpAllocator:
            // Example: map svpAllocator capabilities (bitmask) to key/value
            //if (req->info.svpAllocator.capabilities & EssRMgrSVPACap_none) {
                caps[cap_count].key = "none";
                caps[cap_count].value = "1";
                cap_count++;
            //}
            break;
        default:
            break;
    }
    // TODO: Add similar mapping for audio/frontEnd/svpAllocator if needed
 
    INFO("chirag Cap Count  %d ", cap_count);
    if (cap_count > 0) {
        adapter_req.capabilities = caps;
        adapter_req.capability_count = cap_count;
    } else {
        adapter_req.capabilities = NULL;
        adapter_req.capability_count = 0;
    }

    // For now, we'll handle basic resource types
   
    int essos_resource_id = 0;
    int ret = essos_rdk_adapter_request_resource(rm->rdk_client_id, &adapter_req, &essos_resource_id);
   
    if (ret != 0) {
        ERROR("Chirag Failed to request resource via RDK adapter for client %d", rm->rdk_client_id);
        return false;
    }
    else
    {
        ERROR("Chirag Success to request resource via RDK adapter");
    }
    
    // Update request with assigned ID
    req->assignedId = essos_resource_id;
    req->requestId = essos_resource_id;
   
    // Store request for tracking
    EssRMgrRequest* stored_req = (EssRMgrRequest*)malloc(sizeof(EssRMgrRequest));
    if (stored_req) {
        ERROR("Chirag Stored Req Allocated");
        memcpy(stored_req, req, sizeof(EssRMgrRequest));
        g_active_requests[essos_resource_id] = stored_req;
    }
   
    // Set up notification callback if needed
    if (req->notifyCB) {
        ERROR("Chirag Notifiy Callback for rdk adapter set");
        essos_rdk_adapter_set_notify(rm->rdk_client_id, essos_resource_id, essRMNotifyCallback, req->notifyUserData);
    }
   
    INFO("Chirag Resource requested: client=%d, type=%d, id=%d, priority=%d", 
         rm->rdk_client_id, type, essos_resource_id, req->priority);
    return true;
}
 
// Release resource via RDK adapter
static void essRMReleaseResource( EssRMgr *rm, int type, int id )
{
	

    INFO("essRMReleaseResource Entry Chirag");
    	if (!rm) {
        ERROR("%s: RDK RM instance is null", __FUNCTION__);
        return;
    }

#if 0 
    if (!g_rdk_rm_initialized) {
        ERROR("RDK RM not initialized");
        return;
    }
#endif
    int ret = essos_rdk_adapter_release_resource(rm->rdk_client_id, id);
    if (ret == 0) {
        INFO("Resource released: type=%d, id=%d", type, id);
       
        // Remove from tracking
        auto it = g_active_requests.find(id);
        if (it != g_active_requests.end()) {
            if (it->second) {
                free(it->second);
            }
            g_active_requests.erase(it);
        }
    } else {
        ERROR("Failed to release resource: client=%d, type=%d, id=%d", rm->rdk_client_id, type, id);
    }
}
 
// Get resource state via RDK adapter
static bool essRMGetResourceState( EssRMgr *rm, int type, int id, int *state )
{

    INFO("essRMReleaseResource Entry Chirag");
    	if (!rm || !state) {
        ERROR("Either RM or STATE is NULL");
        return false;
    }
#if 0
    if (!g_rdk_rm_initialized) {
        ERROR("RDK RM not initialized");
        *state = EssRMgrRes_idle;
        return false;
    }
#endif
    int status = 0;
    int ret = essos_rdk_adapter_get_status(rm->rdk_client_id, id, &status);
   
    if (ret == 0 && status == 1) {
        *state = EssRMgrRes_active;
        INFO("essRMGetResourceState: client=%d, resource %d active", rm->rdk_client_id, id);
    } else {
        *state = EssRMgrRes_idle;
        INFO("essRMGetResourceState: client=%d, resource %d idle", rm->rdk_client_id, id);
    }
    return (ret == 0 && status == 1);
}
 
// Dump state for debugging
static void essRMDumpState( EssRMgr *rm )
{
    INFO("=== RDK Resource Manager State ===");
    INFO("Initialized: %s", g_rdk_rm_initialized ? "YES" : "NO");
    INFO("Active requests: %zu", g_active_requests.size());
   
    for (auto& pair : g_active_requests) {
        INFO("  Resource ID: %d", pair.first);
        if (pair.second) {
            INFO("    Priority: %d", pair.second->priority);
            INFO("    Assigned ID: %d", pair.second->assignedId);
        }
    }
    INFO("==================================");
}

// Add application/resource to blacklist
static bool essRMAddToBlackList( EssRMgr *rm, const char *appId )
{
    TRACE1("essRMAddToBlackList: enter appId=%s", appId ? appId : "NULL");

    if (!rm || !appId) {
        ERROR("Invalid parameters");
        return false;
    }

#if 0
    if (!g_rdk_rm_initialized) {
        ERROR("RDK RM not initialized");
        return false;
    }
#endif 
    // For RDK adapter, we need to convert appId to resource type and ID
    // The appId format is typically "resourceType:resourceId" or just a numeric ID
    // For now, we'll use a simple numeric conversion
    // This may need to be enhanced based on your appId format

    int resource_id = 0;
    EssosResourceType resource_type = ESSOS_RES_VIDEO_DECODER; // Default type

    // Try to parse appId as "type:id" or just "id"
    const char* colon = strchr(appId, ':');
    if (colon) {
        // Format: "type:id"
        std::string type_str(appId, colon - appId);
        resource_id = atoi(colon + 1);

        // Map type string to EssosResourceType
        if (type_str == "video_decoder") {
            resource_type = ESSOS_RES_VIDEO_DECODER;
        } else if (type_str == "audio_decoder") {
            resource_type = ESSOS_RES_AUDIO_DECODER;
        } else if (type_str == "frontend") {
            resource_type = ESSOS_RES_FRONTEND;
        } else if (type_str == "svp_allocator") {
            resource_type = ESSOS_RES_SVP_ALLOCATOR;
        }
    } else {
        // Format: just numeric ID
        resource_id = atoi(appId);
    }

    INFO("Adding to blacklist: appId=%s -> type=%d, id=%d", appId, resource_type, resource_id);

    int ret = essos_rdk_adapter_add_to_blacklist(rm->rdk_client_id, resource_type, resource_id);
    bool result = (ret == 0);

    if (!result) {
        WARNING("Failed to add to blacklist: %s", appId);
    }

    TRACE1("essRMAddToBlackList: exit (result=%d)", result);
    return result;
}

// Remove application/resource from blacklist
static bool essRMRemoveFromBlackList( EssRMgr *rm, const char *appId )
{
    TRACE1("essRMRemoveFromBlackList: enter appId=%s", appId ? appId : "NULL");

    if (!rm || !appId) {
        ERROR("Invalid parameters");
        return false;
    }

#if 0
    if (!g_rdk_rm_initialized) {
        ERROR("RDK RM not initialized");
        return false;
    }
#endif

    // Parse appId same way as in add function
    int resource_id = 0;
    EssosResourceType resource_type = ESSOS_RES_VIDEO_DECODER;

    const char* colon = strchr(appId, ':');
    if (colon) {
        std::string type_str(appId, colon - appId);
        resource_id = atoi(colon + 1);

        if (type_str == "video_decoder") {
            resource_type = ESSOS_RES_VIDEO_DECODER;
        } else if (type_str == "audio_decoder") {
            resource_type = ESSOS_RES_AUDIO_DECODER;
        } else if (type_str == "frontend") {
            resource_type = ESSOS_RES_FRONTEND;
        } else if (type_str == "svp_allocator") {
            resource_type = ESSOS_RES_SVP_ALLOCATOR;
        }
    } else {
        resource_id = atoi(appId);
    }
    INFO("Removing from blacklist: appId=%s -> type=%d, id=%d", appId, resource_type, resource_id);

    int ret = essos_rdk_adapter_remove_from_blacklist(rm->rdk_client_id, resource_type, resource_id);
    bool result = (ret == 0);

    if (!result) {
        WARNING("Failed to remove from blacklist: %s", appId);
    }

    TRACE1("essRMRemoveFromBlackList: exit (result=%d)", result);
    return result;
}

// Get blacklist state for application/resource
static bool essRMGetBlackListState( EssRMgr *rm )
{
    TRACE1("essRMGetBlackListState: enter");

    // This function signature doesn't take appId parameter,
    // so we can't query specific resource blacklist state
    // Return false to indicate no general blacklist state

    TRACE1("essRMGetBlackListState: exit (false - not supported for general query)");
    return false;
}

// ========================================================================
// PUBLIC API IMPLEMENTATIONS
// ========================================================================

// Global EssRMgr instance for RDK implementation
static EssRMgr* g_rdk_rm = NULL;


// This is the main Init called at the beginning and we get these in wpeframework logs and from here the initializations and socket connection succeeds and no errors from here  
/*
bool EssRMgrInit()
{
    INFO(" RDK EssRMgrInit: enteriiiingg");
    INFO(" chirag new logs EssRMgrInit");
    extern int gLogLevel;

    char *env = getenv("ESSRMGR_DEBUG");
    if (env) {
        gLogLevel = atoi(env);
    }
    INFO(" chirag new logs EssRMgrInit %d gLogLevel", gLogLevel);

    // Initialize the RDK adapter
    int ret = essos_rdk_adapter_init();
    if (ret != 0) {
        ERROR("Failed to initialize RDK adapter");
        return false;
    }

    g_rdk_rm_initialized = true;
    INFO("************** RDK RM initialized successfully ******");

    INFO("RDK EssRMgrInit: exitinnngggg");
    return true;
}
*/

bool EssRMgrInit()
{
    INFO("Swati  RDK EssRMgrInit: enteriiiingg");

    extern int gLogLevel;

    char *env = getenv("ESSRMGR_DEBUG");
    if (env) {
        gLogLevel = atoi(env);
    }
# if 0
    // Initialize the RDK adapter
    int ret = essos_rdk_adapter_init();
    if (ret != 0) {
        ERROR("Swati Failed to initialize RDK adapter");
        return false;
    }

    g_rdk_rm_initialized = true;
    INFO("Swati ************** RDK RM initialized successfully ******");
#endif

    INFO("Swati RDK EssRMgrInit: exitinnngggg");
    return true;
}


void EssRMgrTerm()
{
    INFO("RDK EssRMgrTerm: enter");

//    if (g_rdk_rm_initialized) 
    {
        // Clean up active requests
        for (auto& pair : g_active_requests) {
            if (pair.second) {
                free(pair.second);
            }
        }
        g_active_requests.clear();

        //essos_rdk_adapter_term();
        // Note: EssRMgrTerm is a global function without client_id context
        // Individual clients are terminated via EssRMgrDestroy() which has the client_id
        // This function just cleans up global state
        g_rdk_rm_initialized = false;
        INFO("RDK RM global state cleared (clients terminated via EssRMgrDestroy)");
    }

    INFO("RDK EssRMgrTerm: exit");
}

/*
EssRMgr* EssRMgrCreate()
{
    INFO("RDK EssRMgrCreate: enteringgggggg");
    INFO("RDK Chirag EssRMgrCreate: enteringgggggg");
    extern int gLogLevel;
    char *env = getenv("ESSRMGR_DEBUG");
    if (env) {
        gLogLevel = atoi(env);
    }
    INFO("RDK log level %d", gLogLevel);

    if (!g_rdk_rm_initialized) {
        ERROR("****RDK RM not initialized. Call EssRMgrInit() first*****");
        ERROR("****Chirag RDK RM not initialized. Call EssRMgrInit() first*****");
        return NULL;
    }

    EssRMgr *rm = (EssRMgr*)calloc(1, sizeof(EssRMgr));
    if (rm) {
        // For RDK implementation, we don't need connection structures
        // The adapter handles all communication
        g_rdk_rm = rm;
        INFO("RDK RM context created");
    } else {
        ERROR("Essos RDK Failed to allocate EssRMgr");
    }

    INFO("RDK EssRMgrCreate: exitinggg");
    return rm;
}

void EssRMgrDestroy(EssRMgr *rm)
{
    INFO("RDK EssRMgrDestroy: enter");

    if (rm) {
        if (rm == g_rdk_rm) {
            g_rdk_rm = NULL;
        }
        free(rm);
        INFO("RDK RM context destroyed");
    }

    INFO("RDK EssRMgrDestroy: exit");
}
*/

// PUBLIC API IMPLEMENTATIONS
// These override the stub implementations from essos-resmgr-shm.cpp
// ========================================================================

/**
 * EssRMgrCreate
 * Create an Essos Resource Manager context using RDK adapter
 */
/*
EssRMgr* EssRMgrCreate()
{
    INFO("EssRMgrCreate: entering chirag");

    EssRMgr *rm= 0;
    bool error= true;

    rm= (EssRMgr*)calloc( 1, sizeof(EssRMgr) );
    if ( rm )
    {
        pthread_mutex_init( &rm->mutex, 0 );

        // Initialize RDK adapter
        if (!essRMInit(rm)) {
            ERROR("Failed to initialize RDK adapter chirag");
            goto exit;
        }

        g_active_requests.clear();
        error= false;
        INFO("EssRMgr created successfully with RDK adapter chirag");
    }
    else
    {
        ERROR("Failed to allocate EssRMgr structure");
    }

exit:
    if ( error )
    {
        if ( rm )
        {
            EssRMgrDestroy( rm );
            rm= 0;
        }
    }

    TRACE1("EssRMgrCreate: exiting chirag (rm=%p)", rm);
    return rm;
}
*/

/**
* EssRMgrCreate
* Create an Essos Resource Manager context using RDK adapter
* Similar to UDS implementation:
* - Allocates EssRMgr structure
* - Establishes connection to RDK RM via adapter (essos_rdk_adapter_init)
* - The adapter maintains the client connection and handles communication
*/
EssRMgr* EssRMgrCreate()
{
    TRACE1("Chirag EssRMgrCreate: enter");
    EssRMgr *rm = 0;
    bool error = true;

    rm = (EssRMgr*)calloc(1, sizeof(EssRMgr));
    if (rm)
    {
        pthread_mutex_init(&rm->mutex, 0);
        rm->rdk_client_id = -1;  // Initialize to invalid
        // Initialize RDK adapter connection (idempotent - safe to call multiple times)
        // This is equivalent to essRMCreateClientConnection in UDS implementation
        INFO("Chirag Initializing RDK adapter client connection for new Essos session");
        int new_client_id = -1;
        int ret = essos_rdk_adapter_init(&new_client_id);
        if (ret != 0 || new_client_id <= 0) 
        {
            ERROR("Chirag Failed to establish RDK adapter connection (ret=%d, client_id=%d)", 
                  ret, new_client_id);
            ERROR("Chirag Possible causes:");
            ERROR("  1. RDK RM server not running (check: systemctl status gen-rmgr)");
            ERROR("  2. Namespace isolation (socket exists in different namespace)");
            ERROR("  3. Socket path mismatch");
            ERROR("Check diagnostics above for detailed socket connection errors");
            goto exit;
        }
       
        // Store the client ID in this Essos session
        rm->rdk_client_id = new_client_id;
        
        INFO("Chirag RDK adapter client connection established successfully");
        INFO("Chirag This Essos session has RDK client ID: %d", rm->rdk_client_id);
        error = false;
    }
    else
    {
        ERROR("Chirag Failed to allocate EssRMgr structure");
    }

exit:
    if (error)
    {
        if (rm)
        {
            EssRMgrDestroy(rm);
            rm = 0;
        }
    }

    TRACE1("Chirag EssRMgrCreate: exit (rm=%p, client_id=%d)", rm, rm ? rm->rdk_client_id : -1);
    return rm;
}


/**
 * EssRMgrDestroy
 * Destroy an Essos Resource Manager context
 */
void EssRMgrDestroy( EssRMgr *rm )
{
    INFO("EssRMgrDestroy: entering chirag (rm=%p)", rm);

    if ( rm )
    {
        // Terminate RDK adapter for THIS client only
        if (rm->rdk_client_id > 0) {
            INFO("Chirag Terminating RDK client ID: %d", rm->rdk_client_id);
            essos_rdk_adapter_term(rm->rdk_client_id);
        }

        pthread_mutex_destroy( &rm->mutex );
        free( rm );
        INFO("EssRMgr destroyed");
    }

    INFO("EssRMgrDestroy: exiting chirag");
}


bool EssRMgrGetPolicyPriorityTie(EssRMgr *rm)
{
    // RDK Resource Manager handles priority internally
    // Return true as default (requester wins tie)
    INFO("RDK EssRMgrGetPolicyPriorityTie");
    return true;
}
/*
bool EssRMgrGetBlackListState(EssRMgr *rm)
{
    // RDK Resource Manager handles blacklist internally
    // Return false (not blacklisted) as default
    INFO("RDK EssRMgrGetBlackListState");
    return false;
}
*/
bool EssRMgrGetAVState(EssRMgr *rm, int *state)
{
    INFO("RDK EssRMgrGetAVState");
    if (!rm || !state) {
        return false;
    }

    // For RDK, we can't easily determine aggregate state
    // Return idle as default
    *state = EssRMgrRes_idle;
    return true;
}

int EssRMgrResourceGetCount(EssRMgr *rm, int type)
{
	INFO("RDK EssRMgrResourceGetCount type %d", type);
    // RDK Resource Manager manages resources internally
    // Return 0 as we don't track individual resource instances
    return 0;
}

//TO DO  - Recheck
bool EssRMgrResourceGetOwner(EssRMgr *rm, int type, int id, int *client, int *priority)
{
	 INFO("RDKEssRMgrResourceGetOwner type %d id %d", type, id);
    if (!rm) {
        return false;
    }

    // Find the request for this resource
    auto it = g_active_requests.find(id);
    if (it != g_active_requests.end() && it->second) {
        if (client) *client = 0;  // RDK doesn't track client IDs
        if (priority) *priority = it->second->priority;
        return true;
    }

    return false;
}

bool EssRMgrResourceGetCaps(EssRMgr *rm, int type, int id, EssRMgrCaps *caps)
{
	INFO("RDK EssRMgrResourceGetCaps type %d id %d", type, id);
    if (!rm || !caps) {
        return false;
    }

    // Find the request for this resource
    auto it = g_active_requests.find(id);
    if (it != g_active_requests.end() && it->second) {
        caps->capabilities = it->second->assignedCaps;
        // Copy usage info if available
        caps->info = it->second->info;
        return true;
    }

    return false;
}

bool EssRMgrResourceGetState(EssRMgr *rm, int type, int id, int *state)
{
	INFO("RDK EssRMgrResourceGetState type %d id %d ", type, id);
    if (!rm || !state) {
        return false;
    }

    return essRMGetResourceState(rm, type, id, state);
}

bool EssRMgrResourceSetState(EssRMgr *rm, int type, int id, int state)
{
	INFO("RDK EssRMgrResourceSetState type %d id %d state %d", type, id, state);
    if (!rm) {
	     ERROR("No RM");
        return false;
    }

    // RDK Resource Manager manages state internally
    // Just validate the state value
    if (state < EssRMgrRes_idle || state >= EssRMgrRes_max) {
        ERROR("Invalid state value: %d", state);
        return false;
    }
	//INFO("RDK EssRMgrResourceSetState type %d id %d state %d", type, id, state);

    // State is managed by RDK, so we just acknowledge
    return true;
}

bool EssRMgrRequestResource(EssRMgr *rm, int type, EssRMgrRequest *req)
{
	INFO("RDK EssRMgrRequestResource type %d", type);
    if (!rm || !req) {
        ERROR("Invalid parameters");
        return false;
    }

    if (type != req->type) {
        ERROR("Mismatched resource types in request");
        return false;
    }

    if (!req->notifyCB) {
        ERROR("Must supply notification callback with request");
        return false;
    }

    return essRMRequestResource(rm, type, req);
}

void EssRMgrReleaseResource(EssRMgr *rm, int type, int id)
{
	INFO("RDK EssRMgrReleaseResource");
    if (!rm) {
        ERROR("Invalid parameters");
        return;
    }

    essRMReleaseResource(rm, type, id);
}

bool EssRMgrRequestSetPriority(EssRMgr *rm, int type, int requestId, int priority)
{
	INFO("RDK EssRMgrRequestSetPriority");
    if (!rm) {
	    INFO("EssRMgrRequestSetPriority RM not present");
        return false;
    }

    // Find the request and update priority
    auto it = g_active_requests.find(requestId);
    if (it != g_active_requests.end() && it->second) {
        it->second->priority = priority;
        INFO("Updated priority for request %d to %d", requestId, priority);
        return true;
    }

    ERROR("Request ID %d not found", requestId);
    return false;
}

bool EssRMgrRequestSetUsage(EssRMgr *rm, int type, int requestId, EssRMgrUsage *usage)
{
	INFO("RDK EssRMgrRequestSetUsage");
    if (!rm || !usage) {
	    INFO("EssRMgrRequestSetUsage neither rm nor usage");
        return false;
    }

    // Find the request and update usage
    auto it = g_active_requests.find(requestId);
    if (it != g_active_requests.end() && it->second) {
        it->second->usage = usage->usage;
        it->second->info = usage->info;
        INFO("Updated usage for request %d", requestId);
        return true;
    }

    ERROR("Request ID %d not found", requestId);
    return false;
}

void EssRMgrRequestCancel(EssRMgr *rm, int type, int requestId)
{
	INFO("RDK EssRMgrRequestCancel");
    if (!rm) {
	    INFO("EssRMgrRequestCancel RM not present");
        return;
    }

    // Find and release the resource
    auto it = g_active_requests.find(requestId);
    if (it != g_active_requests.end()) {
        int resource_id = it->first;
        essRMReleaseResource(rm, type, resource_id);
    } else {
        ERROR("Request ID %d not found", requestId);
    }
}

/*
bool EssRMgrAddToBlackList(EssRMgr *rm, const char *appId)
{
    // RDK Resource Manager handles blacklist internally
    // This is a no-op for now
    INFO("Blacklist add requested for appId: %s (handled by RDK RM)", appId ? appId : "NULL");
    return true;
}

bool EssRMgrRemoveFromBlackList(EssRMgr *rm, const char *appId)
{
    // RDK Resource Manager handles blacklist internally
    // This is a no-op for now
    INFO("Blacklist remove requested for appId: %s (handled by RDK RM)", appId ? appId : "NULL");
    return true;
}
*/

void EssRMgrDumpState(EssRMgr *rm)
{
	INFO("EssRMgrDumpState enter");
    if (!rm) {
	    INFO("EssRMgrDumpState RM not present");
        return;
    }

    INFO("Calling essRMDumpState()");
    essRMDumpState(rm);
}

/**
 * EssRMgrAddToBlackList
 * Public API function - routes blacklist requests to RDK adapter
 */
bool EssRMgrAddToBlackList( EssRMgr *rm, const char *appId )
{

   INFO("EssRMgrAddToBlackList Enter");
    	return essRMAddToBlackList(rm, appId);
}

/**
 * EssRMgrRemoveFromBlackList
 * Public API function - routes blacklist requests to RDK adapter
 */
bool EssRMgrRemoveFromBlackList( EssRMgr *rm, const char *appId )
{
   INFO("EssRMgrRemoveFrmBlackList Enter");	
    return essRMRemoveFromBlackList(rm, appId);
}

/**
 * EssRMgrGetBlackListState
 * Public API function - queries blacklist state
 */
bool EssRMgrGetBlackListState( EssRMgr *rm )
{
    INFO("EssRMgrGetBL state Enter");
    return essRMGetBlackListState(rm);
}



