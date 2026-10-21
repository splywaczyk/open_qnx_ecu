// ------------------------------------
// Copyright (c) 2026 Seweryn Plywaczyk
//
// SPDX-License-Identifier: Apache-2.0
// ------------------------------------

#ifndef HELLO_WORLD_H
#define HELLO_WORLD_H

#include <string>

class HelloWorld {
public:
  explicit HelloWorld(std::string message);
  ~HelloWorld();

  void print_message() const;

private:
  std::string message_;
};

#endif // HELLO_WORLD_H
