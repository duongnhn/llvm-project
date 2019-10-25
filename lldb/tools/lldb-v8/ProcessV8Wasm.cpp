//===- ProcessV8Wasm.cpp ------------------------------------------ *- C++ --*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "ProcessV8Wasm.h"

using namespace lldb_private;


uint32_t ProcessV8Wasm::GetPluginVersion() {
  return 1;
}
