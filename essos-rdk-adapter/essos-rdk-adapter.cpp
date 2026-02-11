#include "essos-rdk-adapter.h"
#include "ClientWrapper.h"
#include <stdio.h>
#include <string.h>
#include <map>
#include <mutex>
#include <utility>

// cursor changes ->
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>


extern "C" {
    typedef void (*RevokeNotificationCallback)(const char* resourceId, const char* clientId, int pid, void* userData);
    void RevokeNotification_SetCallback(RevokeNotificationCallback callback, void* userData);
    void RevokeNotification_ClearCallback();
    int Get_BlackList_State_client(int clientID, const char* resource_type, int resource_id, int* blacklisted);
    int Remove_From_BlackList_client(int clientID, const char* resource_type, int resource_id);
}
 
// ========================================================================
// INTERNAL STATE
// ========================================================================

//Mari added Per-client tracking structures
struct ClientContext {
    int rdk_client_id;
    std::map<int, int> essos_to_rdk_map;  // Essos ID → RDK ID for this client
    std::map<int, int> rdk_to_essos_map;  // RDK ID → Essos ID for this client
    int next_essos_id;
};
static std::map<int, ClientContext*> g_client_contexts;  // client_id → context 
static std::mutex g_adapter_mutex;
//Mari added Per-client tracking

// Revoke callback handler from RDK RM
static void rdk_revoke_callback(const char* resourceId, const char* clientId, int pid, void* userData) {
    fprintf(stderr, "\n");
    fprintf(stderr, "========================================\n");
    fprintf(stderr, "[Essos-RDK Adapter] ⚡⚡⚡ REVOKE CALLBACK INVOKED ⚡⚡⚡\n");
    fprintf(stderr, "[Essos-RDK Adapter] ⚡ Revoke callback from RDK RM: resourceId=%s, clientId=%s, pid=%d\n",
            resourceId ? resourceId : "NULL", clientId ? clientId : "NULL", pid);
    fprintf(stderr, "========================================\n");

    //Mari added Per-client tracking ClientId
   if (!resourceId || !clientId) {
        fprintf(stderr, "[Essos-RDK Adapter] ✗ Invalid resourceId or clientId in revoke callback\n");
        return;
    }
    //Mari added Per-client tracking ClientId

    // Convert strings to integers
    int rdk_resource_id = atoi(resourceId);
   
    //Mari added Per-client tracking ClientId
    int rdk_client_id = atoi(clientId);
    
    fprintf(stderr, "[Essos-RDK Adapter] Converted RDK resource ID: %d, client ID: %d\n", 
            rdk_resource_id, rdk_client_id);
    //Mari added Per-client tracking ClientId
    // Find corresponding Essos resource ID
    std::lock_guard<std::mutex> lock(g_adapter_mutex);

    //Mari added Per-client tracking ClientId
    // Find client context
    auto client_it = g_client_contexts.find(rdk_client_id);
    if (client_it == g_client_contexts.end()) {
        fprintf(stderr, "[Essos-RDK Adapter] ✗ No client context for RDK client ID: %d\n", rdk_client_id);
        return;
    }    

    ClientContext* ctx = client_it->second;
    fprintf(stderr, "[Essos-RDK Adapter] Found client context with %zu resource mappings\n", 
            ctx->rdk_to_essos_map.size());
    
    // Find corresponding Essos resource ID
    
//Mari added Per-client tracking ClientId
    // Find corresponding Essos resource ID
    auto rdk_it = ctx->rdk_to_essos_map.find(rdk_resource_id);
    if (rdk_it == ctx->rdk_to_essos_map.end())
    //Mari added Per-client tracking ClientId
    {
        fprintf(stderr, "[Essos-RDK Adapter] ✗ No Essos mapping for RDK resource ID: %d\n", rdk_resource_id);
        return;
    }

    int essos_resource_id = rdk_it->second;
    fprintf(stderr, "[Essos-RDK Adapter] Mapped RDK ID %d → Essos ID %d\n", rdk_resource_id, essos_resource_id);

    // Call the adapter's revoke handler with default timeout (3000ms)
    essos_rdk_adapter_handle_revoke(essos_resource_id, 3000);
}


// ========================================================================
// TYPE MAPPING
// ========================================================================

static const char* map_essos_type_to_rdk(EssosResourceType type)
{
    switch (type) {
        case ESSOS_RES_VIDEO_DECODER:
            return "video_decoder";
        case ESSOS_RES_AUDIO_DECODER:
            return "audio_decoder";
        case ESSOS_RES_FRONTEND:
            return "frontend";
        case ESSOS_RES_SVP_ALLOCATOR:
            return "svp_allocator";
        default:
            return "unknown";
    }
}

// ========================================================================
// CAPABILITY CONVERSION
// ========================================================================

static void convert_essos_caps_to_rdk(
    EssosCapability* essos_caps,
    int cap_count,
    Capability** rdk_caps_out
)
{   fprintf(stderr, "[Essos-RDK Adapter] Con essos caps to rdk: \n");
    if (cap_count <= 0 || !essos_caps) {
        *rdk_caps_out = NULL;  
        fprintf(stderr, "Chirag [Essos-RDK Adapter] Con essos caps to rdk Count:%d \n",cap_count);
        return;
    }

    fprintf(stderr, "Chirag [Essos-RDK Adapter] Con essos caps to rdk: %d\n",cap_count);

    Capability* rdk_caps = new Capability[cap_count];
    for (int i = 0; i < cap_count; i++) {
        rdk_caps[i].key = (char*)essos_caps[i].key;
        rdk_caps[i].value = (char*)essos_caps[i].value;
        fprintf(stderr, "Chirag[Essos-RDK Adapter] Con essos caps to rdk: key:%s Value:%s\n",essos_caps[i].key,essos_caps[i].value);
    }

    *rdk_caps_out = rdk_caps;
}

// Notification callback storage
struct NotifyInfo {
    void (*callback)(int resource_id, int state, void* user_data);
    void* user_data;
};
static std::map<int, NotifyInfo> g_notify_callbacks;

// ========================================================================
// PUBLIC API IMPLEMENTATION
// ========================================================================
//Mari added Per-client tracking ClientId
int essos_rdk_adapter_init(int* client_id_out)
{
	fprintf(stderr, "Chirag [Essos-RDK Adapter] Init +\n");
    //Mari added Per-client tracking ClientId
    if (!client_id_out) {
        fprintf(stderr, "[Essos-RDK Adapter] ✗ Invalid output parameter\n");
        return -1;
    }
    //Mari added Per-client tracking ClientId
    std::lock_guard<std::mutex> lock(g_adapter_mutex);

    fprintf(stderr, "Chirag [Essos-RDK Adapter] Initializing new client...\n");

    // Check if RDK Resource Manager server socket exists before attempting connection
    const char* socket_path = "/tmp/unix_socket";
    struct stat st;
    if (stat(socket_path, &st) != 0) {
        fprintf(stderr, "[Essos-RDK Adapter] ✗ RDK Resource Manager server socket not found: %s\n", socket_path);
        fprintf(stderr, "[Essos-RDK Adapter] ✗ Error: %s (errno: %d)\n", strerror(errno), errno);
        fprintf(stderr, "[Essos-RDK Adapter] ✗ The RDK Resource Manager server is not running.\n");
        fprintf(stderr, "[Essos-RDK Adapter] ✗ Please start the server before initializing the adapter.\n");
        return -1;
    }

    // Verify it's actually a socket file
    if (!S_ISSOCK(st.st_mode)) {
        fprintf(stderr, "[Essos-RDK Adapter] ✗ Path exists but is not a socket: %s\n", socket_path);
        return -1;
    }

    fprintf(stderr, "[Essos-RDK Adapter] ✓ Server socket found, attempting connection...\n");
    

    // Create RDK client (0 = auto-generate ID)
    //Mari added Per-client tracking ClientId
    int rdk_client_id = Create_RM_client(0);

    if (rdk_client_id <= 0) {
        //fprintf(stderr, "[Essos-RDK Adapter] Failed to create RDK client\n");
	
	    fprintf(stderr, "[Essos-RDK Adapter] ✗ Failed to create RDK client (returned: %d)\n", rdk_client_id);
        fprintf(stderr, "[Essos-RDK Adapter] ✗ Connection to RDK Resource Manager server failed.\n");
        fprintf(stderr, "[Essos-RDK Adapter] ✗ The server may have crashed or is not accepting connections.\n");
        
	// cursor
        return -1;
    }
    //Mari added Per-client tracking ClientId

     // Create new client context
    ClientContext* ctx = new ClientContext();
    ctx->rdk_client_id = rdk_client_id;
    ctx->next_essos_id = 1;
    
    g_client_contexts[rdk_client_id] = ctx;

    // Register revoke callback with RDK RM (only once globally)
    static bool callback_registered = false;
    if (!callback_registered) {
        RevokeNotification_SetCallback(rdk_revoke_callback, nullptr);
        fprintf(stderr, "Chirag [Essos-RDK Adapter] ✓ Revoke callback registered\n");
        callback_registered = true;
    }

    *client_id_out = rdk_client_id;
    
    fprintf(stderr, "Chirag [Essos-RDK Adapter] Initialized with RDK client ID: %d\n", rdk_client_id);
    fprintf(stderr, "Chirag [Essos-RDK Adapter] Total active clients: %zu\n", g_client_contexts.size());
    return 0;
}

/*
int essos_rdk_adapter_get_client_id(void)
{
    std::lock_guard<std::mutex> lock(g_adapter_mutex);
    fprintf(stderr, "Swati essos_rdk_adapter_get_client_id %d", g_rdk_client_id);
    return g_rdk_client_id;
}

int essos_rdk_adapter_is_connected(void)
{
    std::lock_guard<std::mutex> lock(g_adapter_mutex);
    return (g_rdk_client_id > 0) ? 1 : 0;
    //return (g_initialized && g_rdk_client_id > 0) ? 1 : 0;
}
*/
//Mari added Per-client tracking
int essos_rdk_adapter_term(int client_id)
{
    std::lock_guard<std::mutex> lock(g_adapter_mutex);

    //Mari added Per-client tracking
    auto it = g_client_contexts.find(client_id);
    if (it == g_client_contexts.end()) {
        fprintf(stderr, "[Essos-RDK Adapter] ✗ Invalid client ID: %d\n", client_id);
        return -1;
    }
     ClientContext* ctx = it->second;
    fprintf(stderr, "[Essos-RDK Adapter] Terminating client ID: %d...\n", client_id);
    //Mari added Per-client tracking

    // Release all resources
    for (auto& pair : ctx->essos_to_rdk_map) {
        int rdk_id = pair.second;
        Release_Resource_client(client_id, rdk_id);
        fprintf(stderr, "[Essos-RDK Adapter] Released resource RDK ID: %d\n", rdk_id);
    }

    // Clear mappings
    ctx->essos_to_rdk_map.clear();
    ctx->rdk_to_essos_map.clear();

    Delete_RM_client(client_id);
    
    // Remove client context
    delete ctx;
    g_client_contexts.erase(it);

    fprintf(stderr, "Chirag[Essos-RDK Adapter] ✓ Terminated client ID: %d\n", client_id);
    fprintf(stderr, "Chirag[Essos-RDK Adapter] Remaining active clients: %zu\n", g_client_contexts.size());
    
    // Unregister revoke callback only when last client terminates
    if (g_client_contexts.empty()) {
        RevokeNotification_ClearCallback();
        fprintf(stderr, "Chirag [Essos-RDK Adapter] ✓ Revoke callback unregistered (no clients remaining)\n");
    }

    return 0;
}

int essos_rdk_adapter_request_resource(
    int client_id,
    EssosResourceRequest* request,
    int* essos_resource_id
)
{
    fprintf(stderr, " chirag entering essos_rdk_adapter_request_resource() for client_id=%d\n", client_id);
    if (!request || !essos_resource_id) {
        fprintf(stderr, "[Essos-RDK Adapter] ✗ Invalid parameters\n");
        return -1;
    }

    std::lock_guard<std::mutex> lock(g_adapter_mutex);

    // Mari added Per-client tracking
    auto it = g_client_contexts.find(client_id);
    if (it == g_client_contexts.end()) {
        fprintf(stderr, "[Essos-RDK Adapter] ✗ Invalid client ID: %d\n", client_id);
        return -1;
    }

    ClientContext* ctx = it->second;

    // Map resource type
    const char* rdk_type = map_essos_type_to_rdk(request->type);
    fprintf(stderr, "[Essos-RDK Adapter] Client %d requesting resource: %s\n", client_id, rdk_type);

    // Convert capabilities
    Capability* rdk_caps = NULL;
    Capability rdk_caps_new[3];
    convert_essos_caps_to_rdk(
        request->capabilities,
        request->capability_count,
        &rdk_caps
    );

    

    // Create RDK resource info
    RDKRMgrResInfo rdk_res_info;
    rdk_res_info.res_type = (char*)rdk_type;
    rdk_res_info.capability_count = request->capability_count;
    rdk_res_info.capabilities = rdk_caps;

    fprintf(stderr, "Chirag Req Cap Count: %d\n",request->capability_count);
    // Log capabilities
    for (int i = 0; i < request->capability_count; i++) {
        fprintf(stderr, "[Essos-RDK Adapter]   Cap[%d]: %s = %s\n",
               i, rdk_caps[i].key, rdk_caps[i].value);
    }

    
    // Request resource from RDK
    fprintf(stderr, " chirag calling Request_Resource_client() for client %d\n", client_id);
    int ret = Request_Resource_client(client_id, &rdk_res_info);
    fprintf(stderr, " After Request Resource client after %d \n",ret);    
    // Cleanup temporary capabilities
    delete[] rdk_caps;

    // Request_Resource_client returns the request ID (positive number) on success, or INVALID_VALUE (-1) on failure
    if (ret <= 0) {
        fprintf(stderr, "[Essos-RDK Adapter] ✗ RDK request failed (returned: %d)\n", ret);
        return -1;
    }

    fprintf(stderr, "[Essos-RDK Adapter] ✓ RDK request successful (request ID: %d)\n", ret);

    // Get RDK resource ID (blocking call)
    fprintf(stderr, " chirag calling get_resourceID for client %d\n", client_id);
    int rdk_resource_id = get_resourceID(client_id);
    fprintf(stderr, " After Get Resource id: %d \n",rdk_resource_id);
    if (rdk_resource_id <= 0) {
        fprintf(stderr, "[Essos-RDK Adapter] ✗ Failed to get RDK resource ID\n");
        ClientWrapper_closeClient(client_id);
        return -1;
    }

    // Generate Essos resource ID (unique per client)
    int new_essos_id = ctx->next_essos_id++;

     // Store bidirectional mapping in client context
    ctx->essos_to_rdk_map[new_essos_id] = rdk_resource_id;
    ctx->rdk_to_essos_map[rdk_resource_id] = new_essos_id;

    *essos_resource_id = new_essos_id;

    fprintf(stderr, "[Essos-RDK Adapter] ✓ Resource acquired for client %d\n", client_id);
    fprintf(stderr, "[Essos-RDK Adapter]   Essos ID: %d → RDK ID: %d\n",
           new_essos_id, rdk_resource_id);
    fprintf(stderr, "[Essos-RDK Adapter]   Client %d now has %zu active resources\n",
           client_id, ctx->essos_to_rdk_map.size());       

    return 0;
}

int essos_rdk_adapter_is_connected(int client_id)
{
    std::lock_guard<std::mutex> lock(g_adapter_mutex);
    auto it = g_client_contexts.find(client_id);
    return (it != g_client_contexts.end()) ? 1 : 0;
}

int essos_rdk_adapter_release_resource(int client_id, int essos_resource_id)
{
    fprintf(stderr, " chirag entering essos_rdk_adapter_release_resource() for client %d, essos_id=%d\n",
            client_id, essos_resource_id);
    std::lock_guard<std::mutex> lock(g_adapter_mutex);
    // Find client context
    auto client_it = g_client_contexts.find(client_id);
    if (client_it == g_client_contexts.end()) {
        fprintf(stderr, "[Essos-RDK Adapter] ✗ Invalid client ID: %d\n", client_id);
        return -1;
    }
    ClientContext* ctx = client_it->second;

    // Find RDK resource ID
     auto it = ctx->essos_to_rdk_map.find(essos_resource_id);
    if (it == ctx->essos_to_rdk_map.end()) {
        fprintf(stderr, "[Essos-RDK Adapter] ✗ Invalid Essos resource ID: %d for client %d\n",
               essos_resource_id, client_id);
        return -1;
    }

    int rdk_resource_id = it->second;

    fprintf(stderr, "[Essos-RDK Adapter] Releasing resource for client %d...\n", client_id);
    fprintf(stderr, "[Essos-RDK Adapter]   Essos ID: %d → RDK ID: %d\n",
           essos_resource_id, rdk_resource_id);

    // Release resource in RDK
    int ret = Release_Resource_client(client_id, rdk_resource_id);

    if (ret == 0) {
        // Remove from both mappings
         ctx->essos_to_rdk_map.erase(essos_resource_id);
        ctx->rdk_to_essos_map.erase(rdk_resource_id);
	    g_notify_callbacks.erase(essos_resource_id);
        fprintf(stderr, "[Essos-RDK Adapter]   Client %d now has %zu active resources\n",
               client_id, ctx->essos_to_rdk_map.size());
    } else {
        fprintf(stderr, "[Essos-RDK Adapter] ✗ Failed to release resource\n");
    }

    return ret;
}

int essos_rdk_adapter_get_status(int client_id, int essos_resource_id, int* status)
{
   fprintf(stderr, " chirag entering essos_rdk_adapter_get_status() for client %d\n", client_id);
    if (!status) {
        return -1;
    }

    std::lock_guard<std::mutex> lock(g_adapter_mutex);

     // Find client context
    auto client_it = g_client_contexts.find(client_id);
    if (client_it == g_client_contexts.end()) {
        *status = 0;  // Not allocated - client doesn't exist
        return -1;
    }

    ClientContext* ctx = client_it->second;

    // Check if resource exists in mapping
    auto it = ctx->essos_to_rdk_map.find(essos_resource_id);
    if (it == ctx->essos_to_rdk_map.end()) 
    {
        *status = 0;  // Not allocated
        return -1;
    }

    *status = 1;  // Allocated
    return 0;
}

// Notification callback storage
//static std::map<int, std::pair<EssosRdkAdapterNotifyCB, void*>> g_notify_callbacks;
static std::mutex g_notify_mutex;

/*
int essos_rdk_adapter_set_notify(int essos_resource_id, EssosRdkAdapterNotifyCB notify_callback, void* user_data)
{
    fprintf(stderr, " chirag entering essos_rdk_adapter_set_notify()\n");
    NotifyInfo info;
    info.callback = notify_callback;
    info.user_data = user_data;
    g_notify_callbacks[essos_resource_id] = info;

    std::lock_guard<std::mutex> lock(g_notify_mutex);

    if (notify_callback) {
        g_notify_callbacks[essos_resource_id] = std::make_pair(notify_callback, user_data);
        fprintf(stderr, "[Essos-RDK Adapter] Notification callback set for resource ID: %d\n", essos_resource_id);
    } else {
        g_notify_callbacks.erase(essos_resource_id);
        fprintf(stderr, "[Essos-RDK Adapter] Notification callback removed for resource ID: %d\n", essos_resource_id);
    }

    return 0;
}
*/

int essos_rdk_adapter_set_notify(
    int client_id,
    int essos_resource_id,
    void (*notify_callback)(int resource_id, int state, void* user_data),
    void* user_data
)
{
    fprintf(stderr, " chirag entering essos_rdk_adapter_set_notify()\n");
    std::lock_guard<std::mutex> lock(g_adapter_mutex);

    // Verify resource exists
    auto client_it = g_client_contexts.find(client_id);
    if (client_it == g_client_contexts.end()) {
        fprintf(stderr, "[Essos-RDK Adapter] ✗ Invalid client ID: %d\n", client_id);
        return -1;
    }    
     ClientContext* ctx = client_it->second;
     
     auto it = ctx->essos_to_rdk_map.find(essos_resource_id);
    if (it == ctx->essos_to_rdk_map.end()) {
        fprintf(stderr, "[Essos-RDK Adapter] ✗ Invalid Essos resource ID: %d for client %d\n",
                essos_resource_id, client_id);
        return -1;
    }
    // Store callback
    NotifyInfo info;
    info.callback = notify_callback;
    info.user_data = user_data;
    g_notify_callbacks[essos_resource_id] = info;

    fprintf(stderr, "[Essos-RDK Adapter] ✓ Notification callback set for client %d, resource ID: %d\n",
            client_id, essos_resource_id);

    return 0;
}

int essos_rdk_adapter_add_to_blacklist(
    int client_id,
    EssosResourceType type,
    int resource_id
)
{
    fprintf(stderr, " chirag entering essos_rdk_adapter_add_to_blacklist() for client %d\n", client_id);
    std::lock_guard<std::mutex> lock(g_adapter_mutex);

    // Verify client exists
    auto it = g_client_contexts.find(client_id);
    if (it == g_client_contexts.end()) {
        fprintf(stderr, "[Essos-RDK Adapter] ✗ Invalid client ID: %d\n", client_id);
        return -1;
    }

    const char* rdk_type = map_essos_type_to_rdk(type);
    fprintf(stderr, "[Essos-RDK Adapter] Client %d adding to blacklist: %s, ID=%d\n", 
            client_id, rdk_type, resource_id);

    // bool result = addToBlackList(g_rdk_client_id, rdk_type, resource_id);
    // Call RDK RM blacklist function through ClientWrapper
    int result = Add_To_BlackList_client(client_id, rdk_type, resource_id);


    if (result != 0) {
        fprintf(stderr, "[Essos-RDK Adapter] ⚠ Blacklist not supported in RDK RM (stub)\n");
        return -1;
    }

    fprintf(stderr, "[Essos-RDK Adapter] ✓ Added to blacklist\n");
    return 0;
}
 

int essos_rdk_adapter_remove_from_blacklist(
    int client_id,
    EssosResourceType type,
    int resource_id
)
{
    fprintf(stderr, " chirag entering essos_rdk_adapter_remove_from_blacklist() for client %d\n", client_id);
    std::lock_guard<std::mutex> lock(g_adapter_mutex);

    // Verify client exists
    auto it = g_client_contexts.find(client_id);
    if (it == g_client_contexts.end()) {
        fprintf(stderr, "[Essos-RDK Adapter] ✗ Invalid client ID: %d\n", client_id);
        return -1;
    }

    const char* rdk_type = map_essos_type_to_rdk(type);
    fprintf(stderr, "[Essos-RDK Adapter] Client %d removing from blacklist: %s, ID=%d\n", 
            client_id, rdk_type, resource_id);

    // Call RDK RM blacklist function (currently returns false/unimplemented)
    bool result = Remove_From_BlackList_client(client_id, rdk_type, resource_id);

    if (!result) {
        fprintf(stderr, "[Essos-RDK Adapter] ⚠ Blacklist not supported in RDK RM (stub)\n");
        return -1;
    }

    fprintf(stderr, "[Essos-RDK Adapter] ✓ Removed from blacklist\n");
    return 0;
}

int essos_rdk_adapter_get_blacklist_state(
    int client_id,
    EssosResourceType type,
    int resource_id,
    int* blacklisted
)
{
     fprintf(stderr, " chirag entering essos_rdk_adapter_get_blacklist_state() for client %d\n", client_id);
    if (!blacklisted) {
        fprintf(stderr, "[Essos-RDK Adapter] ✗ Invalid output parameter\n");
        return -1;
    }

    std::lock_guard<std::mutex> lock(g_adapter_mutex);

    // Verify client exists
    auto it = g_client_contexts.find(client_id);
    if (it == g_client_contexts.end()) {
        fprintf(stderr, "[Essos-RDK Adapter] ✗ Invalid client ID: %d\n", client_id);
        return -1;
    }

    const char* rdk_type = map_essos_type_to_rdk(type);
        fprintf(stderr, "[Essos-RDK Adapter] Client %d checking blacklist state: %s, ID=%d\n", 
            client_id, rdk_type, resource_id);

    // Call RDK RM blacklist query (currently returns false/unimplemented)
    bool result = Get_BlackList_State_client(client_id, rdk_type, resource_id, blacklisted);

    *blacklisted = result ? 1 : 0;

    fprintf(stderr, "[Essos-RDK Adapter] Blacklist state: %s\n", result ? "blacklisted" : "not blacklisted");
    return 0;
}

int essos_rdk_adapter_handle_revoke(
    int essos_resource_id,
    int timeout_ms
)
{
    fprintf(stderr, " chirag entering essos_rdk_adapter_handle_revoke()\n");
    std::lock_guard<std::mutex> lock(g_adapter_mutex);

     // Search for the resource across all client contexts
    // We need to find which client owns this essos_resource_id
    int rdk_resource_id = -1;
    int owner_client_id = -1;
    
    for (auto& ctx_pair : g_client_contexts) {
        ClientContext* ctx = ctx_pair.second;
        auto essos_it = ctx->essos_to_rdk_map.find(essos_resource_id);
        if (essos_it != ctx->essos_to_rdk_map.end()) {
            rdk_resource_id = essos_it->second;
            owner_client_id = ctx_pair.first;
            fprintf(stderr, "!!! [Essos-RDK Adapter] Found resource: Client %d, Essos ID=%d, RDK ID=%d\n",
                    owner_client_id, essos_resource_id, rdk_resource_id);
            break;
        }
    }
    
    if (rdk_resource_id == -1) {
        fprintf(stderr, "!!! [Essos-RDK Adapter] ✗ Unknown Essos resource ID: %d (not found in any client context)\n", 
                essos_resource_id);
        return -1;
    }
    
    fprintf(stderr, "!!! [Essos-RDK Adapter] Revoke notification: Essos ID=%d, RDK ID=%d, timeout=%dms\n",
            essos_resource_id, rdk_resource_id, timeout_ms);

    // Find and trigger notification callback if set
    auto notify_it = g_notify_callbacks.find(essos_resource_id);
    if (notify_it != g_notify_callbacks.end()) {
        NotifyInfo& info = notify_it->second;
        if (info.callback) {
            // State code for revoke: 0 = revoke notification (per essos-resmgr-rdk.cpp expectations)
            const int ESSOS_STATE_REVOKE = 0;

            fprintf(stderr, "!!! [Essos-RDK Adapter] → Triggering Essos revoke callback for resource %d\n",
                    essos_resource_id);
            info.callback(essos_resource_id, ESSOS_STATE_REVOKE, info.user_data);
        } else {
            fprintf(stderr, "[Essos-RDK Adapter] ⚠ No callback set for revoke notification\n");
        }
    } else {
        fprintf(stderr, "!!! [Essos-RDK Adapter] ⚠ No notify info registered for resource %d\n",
                essos_resource_id);
    }

    // Note: Actual resource release should be done by Essos application in response to callback
    // The adapter waits for essos_rdk_adapter_release_resource() to be called

    fprintf(stderr, "!!! [Essos-RDK Adapter] ✓ Revoke notification processed\n");
    return 0;
}

