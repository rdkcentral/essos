#ifndef __WESTEROS_VERSION_H__
#define __WESTEROS_VERSION_H__

/*
 * Stub header for westeros-version.h
 * Used when compiling without full Westeros compositor support
 */

// Version information stubs
#define WESTEROS_VERSION_MAJOR 1
#define WESTEROS_VERSION_MINOR 0
#define WESTEROS_VERSION_MICRO 0

#define WESTEROS_VERSION_STRING "1.0.0-stub"

// Additional version macros used by essos.cpp
#define WESTEROS_VERSION_FMT "%d.%d.%d"
#define WESTEROS_VERSION WESTEROS_VERSION_MAJOR, WESTEROS_VERSION_MINOR, WESTEROS_VERSION_MICRO

#endif // __WESTEROS_VERSION_H__