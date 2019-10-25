//===-- SystemInitializerV8.h ---------------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLDB_API_SYSTEM_INITIALIZER_V8_H
#define LLDB_API_SYSTEM_INITIALIZER_V8_H

#include "lldb/Initialization/SystemInitializerCommon.h"

namespace lldb_private {
/// Initializes lldb.
///
/// This class is responsible for initializing all of lldb system
/// services needed to use the full LLDB application.  This class is
/// not intended to be used externally, but is instead used
/// internally by SBDebugger to initialize the system.
class SystemInitializerV8 : public SystemInitializerCommon {
public:
  SystemInitializerV8();
  ~SystemInitializerV8() override;

  llvm::Error Initialize() override;
  void Terminate() override;
};

} // namespace lldb_private

#endif // LLDB_API_SYSTEM_INITIALIZER_V8_H
