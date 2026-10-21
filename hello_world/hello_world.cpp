// ------------------------------------
// Copyright (c) 2026 Seweryn Plywaczyk
//
// SPDX-License-Identifier: Apache-2.0
// ------------------------------------

#include "hello_world.h"
#include <iostream>

HelloWorld::HelloWorld(std::string message) : message_{std::move(message)} {}
HelloWorld::~HelloWorld() {}

void HelloWorld::print_message() const { std::cout << message_ << std::endl; }
