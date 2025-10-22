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
#include <unistd.h>
#include <linux/input.h>
#include <pthread.h>
#include <sys/time.h>

#include "essos.h"

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

   // Test successful initialization
   result= EssContextInit( ctx );
   if ( !result )
   {
      EMERROR("EssContextInit failed");
      goto exit;
   }

   // Test double initialization (should not fail)
   result= EssContextInit( ctx );
   if ( !result )
   {
      EMERROR("EssContextInit double init failed");
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
 * Test EssContextGetLastErrorDetail functionality
 */
bool testCaseEssosGetLastErrorDetail( EMCTX *emctx )
{
   bool testResult= false;
   EssCtx *ctx= 0;
   const char *errorDetail;

   EMINFO("Testing EssContextGetLastErrorDetail");

   ctx= EssContextCreate();
   if ( !ctx )
   {
      EMERROR("EssContextCreate failed");
      goto exit;
   }

   // Test getting error details from valid context
   errorDetail= EssContextGetLastErrorDetail( ctx );
   // Should either be null or empty string for new context
   if ( errorDetail != 0 && strlen(errorDetail) > 0 )
   {
      EMINFO("Initial error detail: %s", errorDetail);
   }

   // Test with null context - should return null
   errorDetail= EssContextGetLastErrorDetail( (EssCtx*)0 );
   if ( errorDetail != 0 )
   {
      EMERROR("EssContextGetLastErrorDetail with null context should return null");
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
// WAYLAND AND DIRECT MODE TESTS
// ==============================================================================

/**
 * Test EssContextSetUseWayland and EssContextGetUseWayland
 */
bool testCaseEssosUseWayland( EMCTX *emctx )
{
   bool testResult= false;
   bool result;
   EssCtx *ctx= 0;
   bool useWayland;
   bool value;

   EMINFO("Testing EssContextSetUseWayland/GetUseWayland");

   // Test with null context - should fail
   result= EssContextSetUseWayland( (EssCtx*)0, true );
   if ( result )
   {
      EMERROR("EssContextSetUseWayland did not fail with null handle");
      goto exit;
   }

   ctx= EssContextCreate();
   if ( !ctx )
   {
      EMERROR("EssContextCreate failed");
      goto exit;
   }

   // Test setting Wayland mode to true
   useWayland= true;
   result= EssContextSetUseWayland( ctx, useWayland );
   if ( result == false )
   {
      EMINFO("EssContextSetUseWayland failed (may not be supported)");
   }
   else
   {
      value= EssContextGetUseWayland( ctx );
      if ( value != useWayland )
      {
         EMERROR("EssContextGetUseWayland reports unexpected value: expected(%d) actual(%d)", useWayland, value );
         goto exit;
      }
   }

   // Test setting Wayland mode to false
   useWayland= false;
   result= EssContextSetUseWayland( ctx, useWayland );
   if ( result == false )
   {
      EMINFO("EssContextSetUseWayland to false failed (may not be supported)");
   }
   else
   {
      value= EssContextGetUseWayland( ctx );
      if ( value != useWayland )
      {
         EMERROR("EssContextGetUseWayland reports unexpected value: expected(%d) actual(%d)", useWayland, value );
         goto exit;
      }
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
 * Test EssContextSetUseDirect and EssContextGetUseDirect
 */
bool testCaseEssosUseDirect( EMCTX *emctx )
{
   bool testResult= false;
   bool result;
   EssCtx *ctx= 0;
   bool useDirect;
   bool value;

   EMINFO("Testing EssContextSetUseDirect/GetUseDirect");

   // Test with null context - should fail
   result= EssContextSetUseDirect( (EssCtx*)0, true );
   if ( result )
   {
      EMERROR("EssContextSetUseDirect did not fail with null handle");
      goto exit;
   }

   ctx= EssContextCreate();
   if ( !ctx )
   {
      EMERROR("EssContextCreate failed");
      goto exit;
   }

   // Test setting Direct mode to true
   useDirect= true;
   result= EssContextSetUseDirect( ctx, useDirect );
   if ( result == false )
   {
      EMINFO("EssContextSetUseDirect failed (may not be supported)");
   }
   else
   {
      value= EssContextGetUseDirect( ctx );
      if ( value != useDirect )
      {
         EMERROR("EssContextGetUseDirect reports unexpected value: expected(%d) actual(%d)", useDirect, value );
         goto exit;
      }
   }

   // Test setting Direct mode to false
   useDirect= false;
   result= EssContextSetUseDirect( ctx, useDirect );
   if ( result == false )
   {
      EMINFO("EssContextSetUseDirect to false failed (may not be supported)");
   }
   else
   {
      value= EssContextGetUseDirect( ctx );
      if ( value != useDirect )
      {
         EMERROR("EssContextGetUseDirect reports unexpected value: expected(%d) actual(%d)", useDirect, value );
         goto exit;
      }
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
 * Test EssContextSupportWayland and EssContextSupportDirect
 */
bool testCaseEssosSupportQueries( EMCTX *emctx )
{
   bool testResult= false;
   EssCtx *ctx= 0;
   bool supportsWayland;
   bool supportsDirect;

   EMINFO("Testing EssContextSupportWayland/SupportDirect");

   ctx= EssContextCreate();
   if ( !ctx )
   {
      EMERROR("EssContextCreate failed");
      goto exit;
   }

   // Test Wayland support query
   supportsWayland= EssContextSupportWayland( ctx );
   EMINFO("Wayland support: %s", supportsWayland ? "YES" : "NO");

   // Test Direct mode support query  
   supportsDirect= EssContextSupportDirect( ctx );
   EMINFO("Direct mode support: %s", supportsDirect ? "YES" : "NO");

   // Test with null context
   supportsWayland= EssContextSupportWayland( (EssCtx*)0 );
   if ( supportsWayland )
   {
      EMERROR("EssContextSupportWayland with null context should return false");
      goto exit;
   }

   supportsDirect= EssContextSupportDirect( (EssCtx*)0 );
   if ( supportsDirect )
   {
      EMERROR("EssContextSupportDirect with null context should return false");
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
 * Test EssContextGetAppPlatformDisplayType
 */
bool testCaseEssosGetAppPlatformDisplayType( EMCTX *emctx )
{
   bool testResult= false;
   EssCtx *ctx= 0;
   EssAppPlatformDisplayType displayType;

   EMINFO("Testing EssContextGetAppPlatformDisplayType");

   ctx= EssContextCreate();
   if ( !ctx )
   {
      EMERROR("EssContextCreate failed");
      goto exit;
   }

   // Test getting platform display type
   displayType= EssContextGetAppPlatformDisplayType( ctx );
   
   // Should be one of the valid enum values
   if ( displayType != EssAppPlatformDisplayType_direct &&
        displayType != EssAppPlatformDisplayType_wayland &&
        displayType != EssAppPlatformDisplayType_waylandExtension )
   {
      EMERROR("EssContextGetAppPlatformDisplayType returned invalid type: %d", displayType);
      goto exit;
   }

   EMINFO("Platform display type: %d", displayType);

   testResult= true;

exit:
   if (ctx)
   {
      EssContextDestroy(ctx);
   }

   return testResult;
}

// ==============================================================================
// DISPLAY CONFIGURATION TESTS
// ==============================================================================

/**
 * Test EssContextSetDisplaySize
 */
bool testCaseEssosSetDisplaySize( EMCTX *emctx )
{
   bool testResult= false;
   bool result;
   EssCtx *ctx= 0;

   EMINFO("Testing EssContextSetDisplaySize");

   ctx= EssContextCreate();
   if ( !ctx )
   {
      EMERROR("EssContextCreate failed");
      goto exit;
   }

   // Test valid display sizes
   result= EssContextSetDisplaySize( ctx, 1920, 1080 );
   if ( !result )
   {
      EMERROR("EssContextSetDisplaySize failed for 1920x1080");
      goto exit;
   }

   result= EssContextSetDisplaySize( ctx, 1280, 720 );
   if ( !result )
   {
      EMERROR("EssContextSetDisplaySize failed for 1280x720");
      goto exit;
   }

   result= EssContextSetDisplaySize( ctx, 3840, 2160 );
   if ( !result )
   {
      EMERROR("EssContextSetDisplaySize failed for 3840x2160");
      goto exit;
   }

   // Test boundary values
   result= EssContextSetDisplaySize( ctx, 1, 1 );
   if ( !result )
   {
      EMINFO("EssContextSetDisplaySize failed for 1x1 (may be expected)");
   }

   // Test invalid values - should fail
   result= EssContextSetDisplaySize( ctx, 0, 1080 );
   if ( result )
   {
      EMERROR("EssContextSetDisplaySize should have failed for width=0");
      goto exit;
   }

   result= EssContextSetDisplaySize( ctx, 1920, 0 );
   if ( result )
   {
      EMERROR("EssContextSetDisplaySize should have failed for height=0");
      goto exit;
   }

   result= EssContextSetDisplaySize( ctx, -100, -100 );
   if ( result )
   {
      EMERROR("EssContextSetDisplaySize should have failed for negative values");
      goto exit;
   }

   // Test with null context - should fail
   result= EssContextSetDisplaySize( (EssCtx*)0, 1920, 1080 );
   if ( result )
   {
      EMERROR("EssContextSetDisplaySize should have failed with null context");
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
 * Test EssContextSetInitialWindowSize
 */
bool testCaseEssosSetInitialWindowSize( EMCTX *emctx )
{
   bool testResult= false;
   bool result;
   EssCtx *ctx= 0;

   EMINFO("Testing EssContextSetInitialWindowSize");

   ctx= EssContextCreate();
   if ( !ctx )
   {
      EMERROR("EssContextCreate failed");
      goto exit;
   }

   // Test valid window sizes
   result= EssContextSetInitialWindowSize( ctx, 800, 600 );
   if ( !result )
   {
      EMERROR("EssContextSetInitialWindowSize failed for 800x600");
      goto exit;
   }

   result= EssContextSetInitialWindowSize( ctx, 1024, 768 );
   if ( !result )
   {
      EMERROR("EssContextSetInitialWindowSize failed for 1024x768");
      goto exit;
   }

   result= EssContextSetInitialWindowSize( ctx, 640, 480 );
   if ( !result )
   {
      EMERROR("EssContextSetInitialWindowSize failed for 640x480");
      goto exit;
   }

   // Test invalid values - should fail
   result= EssContextSetInitialWindowSize( ctx, 0, 600 );
   if ( result )
   {
      EMERROR("EssContextSetInitialWindowSize should have failed for width=0");
      goto exit;
   }

   result= EssContextSetInitialWindowSize( ctx, 800, 0 );
   if ( result )
   {
      EMERROR("EssContextSetInitialWindowSize should have failed for height=0");
      goto exit;
   }

   // Test with null context - should fail
   result= EssContextSetInitialWindowSize( (EssCtx*)0, 800, 600 );
   if ( result )
   {
      EMERROR("EssContextSetInitialWindowSize should have failed with null context");
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

/**
 * Test runner structure - maps test names to test functions
 */
typedef struct _TestCase
{
   const char *name;
   bool (*testFunc)(EMCTX *emctx);
} TestCase;

// Array of all test cases
static TestCase testCases[] = {
   // Context Management Tests
   {"EssosContextCreateDestroy", testCaseEssosContextCreateDestroy},
   {"EssosContextInit", testCaseEssosContextInit}, 
   {"EssosContextInitNull", testCaseEssosContextInitNull},
   {"EssosGetLastErrorDetail", testCaseEssosGetLastErrorDetail},
   
   // Wayland and Direct Mode Tests
   {"EssosUseWayland", testCaseEssosUseWayland},
   {"EssosUseDirect", testCaseEssosUseDirect},
   {"EssosSupportQueries", testCaseEssosSupportQueries},
   {"EssosGetAppPlatformDisplayType", testCaseEssosGetAppPlatformDisplayType},
   
   // Display Configuration Tests  
   {"EssosSetDisplaySize", testCaseEssosSetDisplaySize},
   {"EssosSetInitialWindowSize", testCaseEssosSetInitialWindowSize},
};

/**
 * Main test runner function
 */
int main(int argc, char **argv)
{
   EMCTX emctx = {0};
   int numTests = sizeof(testCases) / sizeof(testCases[0]);
   bool runAll = true;
   char *testFilter = NULL;

   printf("=== Essos L1 Test Suite ===\n");
   printf("Total tests available: %d\n\n", numTests);

   // Simple command line parsing
   if (argc > 1)
   {
      if (strcmp(argv[1], "--help") == 0 || strcmp(argv[1], "-h") == 0)
      {
         printf("Usage: %s [test_name_filter]\n", argv[0]);
         printf("  Run all tests or filter by test name containing the filter string\n");
         printf("  --help, -h    Show this help message\n");
         printf("  --list, -l    List all available tests\n\n");
         printf("Examples:\n");
         printf("  %s                     # Run all tests\n", argv[0]);
         printf("  %s Context             # Run tests with 'Context' in name\n", argv[0]);
         printf("  %s Display             # Run tests with 'Display' in name\n", argv[0]);
         return 0;
      }
      else if (strcmp(argv[1], "--list") == 0 || strcmp(argv[1], "-l") == 0)
      {
         printf("Available tests:\n");
         for (int i = 0; i < numTests; i++)
         {
            printf("  %2d. %s\n", i + 1, testCases[i].name);
         }
         return 0;
      }
      else
      {
         testFilter = argv[1];
         runAll = false;
         printf("Running tests matching filter: '%s'\n\n", testFilter);
      }
   }

   // Run tests
   emctx.testCount = 0;
   emctx.passCount = 0;
   emctx.failCount = 0;

   for (int i = 0; i < numTests; i++)
   {
      bool shouldRun = runAll;
      
      if (!runAll && testFilter)
      {
         shouldRun = (strstr(testCases[i].name, testFilter) != NULL);
      }
      
      if (shouldRun)
      {
         emctx.testCount++;
         printf("Running test %d/%d: %s\n", emctx.testCount, numTests, testCases[i].name);
         
         bool result = testCases[i].testFunc(&emctx);
         
         if (result)
         {
            printf("  PASS\n");
            emctx.passCount++;
         }
         else
         {
            printf("  FAIL\n");
            emctx.failCount++;
         }
         printf("\n");
      }
   }

   // Print summary
   printf("=== Test Results Summary ===\n");
   printf("Tests run: %d\n", emctx.testCount);
   printf("Passed:    %d\n", emctx.passCount);
   printf("Failed:    %d\n", emctx.failCount);
   
   if (emctx.failCount == 0)
   {
      printf("Result:    ALL TESTS PASSED!\n");
      return 0;
   }
   else
   {
      printf("Result:    %d TEST(S) FAILED\n", emctx.failCount);
      return 1;
   }
}