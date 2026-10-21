// ------------------------------------
// Copyright (c) 2026 Seweryn Plywaczyk
//
// SPDX-License-Identifier: Apache-2.0
// ------------------------------------

#include "hello_world.h"

int main() {
  HelloWorld hello("Welcome to Open QNX ECU");
  hello.print_message();
  return 0;
}
