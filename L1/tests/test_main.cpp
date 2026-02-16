/*
 * Main entry point for Essos L1 Tests
 * Copyright 2024 RDK Management
 */

#include <gtest/gtest.h>

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
