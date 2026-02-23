/*
 * Mock System Functions for L1 Testing
 * Copyright 2024 RDK Management
 */

#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <unistd.h>
#include <poll.h>
#include <fcntl.h>
#include <sys/time.h>
#include <pthread.h>

// Mock state for system calls
static bool g_mock_poll_should_timeout = false;
static bool g_mock_poll_should_fail = false;
static int g_mock_poll_return_value = 1;

extern "C" {

// Control functions
void mock_system_reset() {
    g_mock_poll_should_timeout = false;
    g_mock_poll_should_fail = false;
    g_mock_poll_return_value = 1;
}

void mock_system_set_poll_timeout(bool should_timeout) {
    g_mock_poll_should_timeout = should_timeout;
}

void mock_system_set_poll_fail(bool should_fail) {
    g_mock_poll_should_fail = should_fail;
}

void mock_system_set_poll_return(int value) {
    g_mock_poll_return_value = value;
}

// Mock implementations - only override if needed for testing
#ifdef L1_TESTING

int poll(struct pollfd *fds, nfds_t nfds, int timeout) {
    (void)fds;
    (void)nfds;
    (void)timeout;
    
    if (g_mock_poll_should_fail) {
        return -1;
    }
    
    if (g_mock_poll_should_timeout) {
        return 0;
    }
    
    // Simulate events on fd
    if (nfds > 0 && fds) {
        fds[0].revents = POLLIN;
    }
    
    return g_mock_poll_return_value;
}

#endif

} // extern "C"
