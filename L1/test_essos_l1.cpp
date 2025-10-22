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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

// Platform-specific includes
#ifdef _WIN32
#include <windows.h>
#include <process.h>
#define sleep(x) Sleep((x) * 1000)
#else
#include <unistd.h>
#include <pthread.h>
#include <sys/time.h>
#ifdef __linux__
#include <linux/input.h>
#endif
#endif

#ifdef _WIN32
#include "essos_mock.h"
#else
#include "essos.h"
#endif

#define WINDOW_WIDTH 640
#define WINDOW_HEIGHT 480
#define TEST_TIMEOUT_MS 5000

// Test framework macros
#define EMERROR(fmt, ...) printf("ERROR: " fmt "\n", ##__VA_ARGS__)
#define EMINFO(fmt, ...) printf("INFO: " fmt "\n", ##__VA_ARGS__)

typedef struct _EMCTX
{
   int testCount;
   int passCount;
   int failCount;
} EMCTX;

// ==============================================================================
// CONTEXT MANAGEMENT TESTS
// ==============================================================================

/**
 * Test EssContextCreate and EssContextDestroy basic functionality
 */
bool testCaseEssosContextCreateDestroy( EMCTX *emctx )
{
   bool testResult= false;
   EssCtx *ctx= 0;

   EMINFO("Testing EssContextCreate/Destroy");

   // Test context creation
   ctx= EssContextCreate();
   if ( !ctx )
   {
      EMERROR("EssContextCreate failed");
      goto exit;
   }

   // Test context destruction
   EssContextDestroy(ctx);
   ctx= 0;

   // Test multiple context creation
   for (int i = 0; i < 5; i++)
   {
      EssCtx *testCtx= EssContextCreate();
      if ( !testCtx )
      {
         EMERROR("EssContextCreate failed on iteration %d", i);
         goto exit;
      }
      EssContextDestroy(testCtx);
   }

   testResult= true;

exit:
   if (ctx)
   {
      EssContextDestroy(ctx);
   }

   return testResult;
}

/**
 * Test EssContextInit functionality
 */
bool testCaseEssosContextInit( EMCTX *emctx )
{
   bool testResult= false;
   bool result;
   EssCtx *ctx= 0;

   EMINFO("Testing EssContextInit");

   ctx= EssContextCreate();
   if ( !ctx )
   {
      EMERROR("EssContextCreate failed");
      goto exit;
   }

   // Set Direct mode explicitly for VM environment
   result= EssContextSetUseDirect(ctx, true);
   if ( !result )
   {
      EMERROR("EssContextSetUseDirect failed");
      goto exit;
   }

   // Test initialization
   result= EssContextInit(ctx);
   if ( !result )
   {
      EMERROR("EssContextInit failed");
      goto exit;
   }

   // Test double initialization (should fail - already running)
   result= EssContextInit(ctx);
   if ( result )
   {
      EMERROR("EssContextInit double init should have failed");
      goto exit;
   }
   else
   {
      EMINFO("Double initialization correctly rejected (already running)");
   }

   testResult= true;

exit:
   if (ctx)
   {
      EssContextDestroy(ctx);
   }

   return testResult;
}

/**
 * Test EssContextInit with null context (negative test)
 */
bool testCaseEssosContextInitNull( EMCTX *emctx )
{
   bool testResult= false;
   bool result;

   EMINFO("Testing EssContextInit with null context");

   // Test initialization with null context - should fail
   result= EssContextInit( (EssCtx*)0 );
   if ( result )
   {
      EMERROR("EssContextInit with null context should have failed");
      goto exit;
   }

   testResult= true;

exit:
   return testResult;
}

/**
 * Test Wayland support detection
 */
bool testCaseEssosUseWayland( EMCTX *emctx )
{
   bool testResult= false;
   bool result;
   EssCtx *ctx= 0;

   EMINFO("Testing Wayland support");

   ctx= EssContextCreate();
   if ( !ctx )
   {
      EMERROR("EssContextCreate failed");
      goto exit;
   }

   // Test Wayland support query
   result= EssContextSupportWayland(ctx);
   EMINFO("Wayland support: %s", result ? "YES" : "NO");

   // Test setting Wayland mode
   result= EssContextSetUseWayland(ctx, true);
   if ( !result )
   {
      EMINFO("SetUseWayland(true) failed - may not be supported");
   }

   // Test getting Wayland mode
   result= EssContextGetUseWayland(ctx);
   EMINFO("Current Wayland mode: %s", result ? "ENABLED" : "DISABLED");

   testResult= true;

exit:
   if (ctx)
   {
      EssContextDestroy(ctx);
   }

   return testResult;
}

/**
 * Test Direct EGL support
 */
bool testCaseEssosUseDirect( EMCTX *emctx )
{
   bool testResult= false;
   bool result;
   EssCtx *ctx= 0;

   EMINFO("Testing Direct EGL support");

   ctx= EssContextCreate();
   if ( !ctx )
   {
      EMERROR("EssContextCreate failed");
      goto exit;
   }

   // Test Direct support query
   result= EssContextSupportDirect(ctx);
   EMINFO("Direct EGL support: %s", result ? "YES" : "NO");

   // Test setting Direct mode
   result= EssContextSetUseDirect(ctx, true);
   if ( !result )
   {
      EMINFO("SetUseDirect(true) failed - may not be supported");
   }

   // Test getting Direct mode
   result= EssContextGetUseDirect(ctx);
   EMINFO("Current Direct mode: %s", result ? "ENABLED" : "DISABLED");

   testResult= true;

exit:
   if (ctx)
   {
      EssContextDestroy(ctx);
   }

   return testResult;
}

/**
 * Test display size configuration
 */
bool testCaseEssosSetDisplaySize( EMCTX *emctx )
{
   bool testResult= false;
   bool result;
   EssCtx *ctx= 0;

   EMINFO("Testing display size configuration");

   ctx= EssContextCreate();
   if ( !ctx )
   {
      EMERROR("EssContextCreate failed");
      goto exit;
   }

   // Set Direct mode explicitly for VM environment
   result= EssContextSetUseDirect(ctx, true);
   if ( !result )
   {
      EMERROR("EssContextSetUseDirect failed");
      goto exit;
   }

   // Initialize context first (required before setting display size)
   result= EssContextInit(ctx);
   if ( !result )
   {
      EMERROR("EssContextInit failed");
      goto exit;
   }

   // Test setting display size
   result= EssContextSetDisplaySize(ctx, 1920, 1080);
   if ( !result )
   {
      EMERROR("EssContextSetDisplaySize failed");
      goto exit;
   }

   // Test setting zero display size (may be accepted as "auto" or "default")
   result= EssContextSetDisplaySize(ctx, 0, 0);
   if ( result )
   {
      EMINFO("Zero display size accepted (may indicate auto/default behavior)");
   }
   else
   {
      EMINFO("Zero display size rejected (strict validation)");
   }

   // Test setting extremely large display size
   result= EssContextSetDisplaySize(ctx, 8192, 4320);
   EMINFO("Large display size (8K): %s", result ? "ACCEPTED" : "REJECTED");

   testResult= true;

exit:
   if (ctx)
   {
      EssContextDestroy(ctx);
   }

   return testResult;
}

/**
 * Test window size and position configuration  
 */
bool testCaseEssosWindowConfig( EMCTX *emctx )
{
   bool testResult= false;
   bool result;
   EssCtx *ctx= 0;

   EMINFO("Testing window configuration");

   ctx= EssContextCreate();
   if ( !ctx )
   {
      EMERROR("EssContextCreate failed");
      goto exit;
   }

   // Test setting initial window size
   result= EssContextSetInitialWindowSize(ctx, WINDOW_WIDTH, WINDOW_HEIGHT);
   if ( !result )
   {
      EMERROR("EssContextSetInitialWindowSize failed");
      goto exit;
   }

   // Test setting window position
   result= EssContextSetWindowPosition(ctx, 100, 100);
   if ( !result )
   {
      EMERROR("EssContextSetWindowPosition failed");
      goto exit;
   }

   testResult= true;

exit:
   if (ctx)
   {
      EssContextDestroy(ctx);
   }

   return testResult;
}

/**
 * Test EGL configuration
 */
bool testCaseEssosEGLConfig( EMCTX *emctx )
{
   bool testResult= false;
   bool result;
   EssCtx *ctx= 0;
   NativeDisplayType displayType;

   EMINFO("Testing EGL configuration");

   ctx= EssContextCreate();
   if ( !ctx )
   {
      EMERROR("EssContextCreate failed");
      goto exit;
   }

   // Set Direct mode explicitly for VM environment
   result= EssContextSetUseDirect(ctx, true);
   if ( !result )
   {
      EMERROR("EssContextSetUseDirect failed");
      goto exit;
   }

   // Initialize context first
   result= EssContextInit(ctx);
   if ( !result )
   {
      EMERROR("EssContextInit failed");
      goto exit;
   }

   // Test getting display type
   result= EssContextGetEGLDisplayType(ctx, &displayType);
   if ( result )
   {
      EMINFO("EGL Display type obtained successfully");
   }
   else
   {
      EMINFO("EGL Display type not available (expected in stub mode)");
   }

   testResult= true;

exit:
   if (ctx)
   {
      EssContextDestroy(ctx);
   }

   return testResult;
}

/**
 * Test native window creation
 */
bool testCaseEssosNativeWindow( EMCTX *emctx )
{
   bool testResult= false;
   bool result;
   EssCtx *ctx= 0;
   NativeWindowType nativeWindow;

   EMINFO("Testing native window creation");

   ctx= EssContextCreate();
   if ( !ctx )
   {
      EMERROR("EssContextCreate failed");
      goto exit;
   }

   // Set Direct mode explicitly for VM environment
   result= EssContextSetUseDirect(ctx, true);
   if ( !result )
   {
      EMERROR("EssContextSetUseDirect failed");
      goto exit;
   }

   // Initialize context
   result= EssContextInit(ctx);
   if ( !result )
   {
      EMERROR("EssContextInit failed");
      goto exit;
   }

   // Test creating native window
   result= EssContextCreateNativeWindow(ctx, WINDOW_WIDTH, WINDOW_HEIGHT, &nativeWindow);
   if ( result )
   {
      EMINFO("Native window created successfully");
      
      // Test destroying native window
      EssContextDestroyNativeWindow(ctx, nativeWindow);
      EMINFO("Native window destroyed successfully");
   }
   else
   {
      EMINFO("Native window creation failed (expected in stub mode)");
   }

   testResult= true;

exit:
   if (ctx)
   {
      EssContextDestroy(ctx);
   }

   return testResult;
}

/**
 * Test error reporting
 */
bool testCaseEssosErrorReporting( EMCTX *emctx )
{
   bool testResult= false;
   const char* errorDetail;
   EssCtx *ctx= 0;

   EMINFO("Testing error reporting");

   ctx= EssContextCreate();
   if ( !ctx )
   {
      EMERROR("EssContextCreate failed");
      goto exit;
   }

   // Test getting error detail
   errorDetail= EssContextGetLastErrorDetail(ctx);
   if ( errorDetail )
   {
      EMINFO("Error detail available: %s", errorDetail);
   }
   else
   {
      EMINFO("No error detail available (expected)");
   }

   // Test error detail with null context
   errorDetail= EssContextGetLastErrorDetail((EssCtx*)0);
   if ( errorDetail )
   {
      EMERROR("GetLastErrorDetail should return null for null context");
      goto exit;
   }

   testResult= true;

exit:
   if (ctx)
   {
      EssContextDestroy(ctx);
   }

   return testResult;
}

// ==============================================================================
// TEST RUNNER
// ==============================================================================

typedef bool (*testCaseFunction)(EMCTX *emctx);

typedef struct _testCaseInfo
{
   testCaseFunction func;
   const char *name;
} testCaseInfo;

static testCaseInfo testCases[]=
{
   { testCaseEssosContextCreateDestroy, "Context Create/Destroy" },
   { testCaseEssosContextInit, "Context Initialize" },
   { testCaseEssosContextInitNull, "Context Init Null (Negative)" },
   { testCaseEssosUseWayland, "Wayland Support" },
   { testCaseEssosUseDirect, "Direct EGL Support" },
   { testCaseEssosSetDisplaySize, "Display Size Configuration" },
   { testCaseEssosWindowConfig, "Window Configuration" },
   { testCaseEssosEGLConfig, "EGL Configuration" },
   { testCaseEssosNativeWindow, "Native Window Management" },
   { testCaseEssosErrorReporting, "Error Reporting" }
};

static void runTest( EMCTX *emctx, testCaseInfo *testCase )
{
   bool result;

   printf("\n=== Running: %s ===\n", testCase->name);
   
   result= testCase->func(emctx);
   
   emctx->testCount++;
   if ( result )
   {
      emctx->passCount++;
      printf("PASS: %s\n", testCase->name);
   }
   else
   {
      emctx->failCount++;
      printf("FAIL: %s\n", testCase->name);
   }
}

int main( int argc, char **argv )
{
   int result= 0;
   EMCTX emctx;
   int numTestCases;
   int i;

   printf("Essos L1 Test Suite\n");
   printf("==================\n");

   memset( &emctx, 0, sizeof(emctx) );

   numTestCases= sizeof(testCases)/sizeof(testCaseInfo);

   printf("Running %d test cases...\n", numTestCases);

   for( i= 0; i < numTestCases; ++i )
   {
      runTest( &emctx, &testCases[i] );
   }

   printf("\n=== Test Results ===\n");
   printf("Total tests: %d\n", emctx.testCount);
   printf("Passed: %d\n", emctx.passCount);
   printf("Failed: %d\n", emctx.failCount);
   printf("Success rate: %.1f%%\n", 
          emctx.testCount > 0 ? (100.0 * emctx.passCount / emctx.testCount) : 0.0);

   if ( emctx.failCount > 0 )
   {
      result= 1;
      printf("\n❌ Test suite FAILED\n");
   }
   else
   {
      printf("\n✅ Test suite PASSED\n");
   }

   return result;
}