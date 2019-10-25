//===- ProcessV8Wasm.h ------------------------------------------ *- C++ --*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "lldb/Target/Process.h"
#include "lldb/Target/Target.h"
#include <cstdio>


namespace lldb_private {

class ProcessV8Wasm : public Process {
public:
  ProcessV8Wasm(lldb::TargetSP target_sp, lldb::ListenerSP listener_sp)
      : Process(target_sp, listener_sp), fp_(nullptr) {
    fp_ = fopen("D:\\wasm\\test1\\fib.wasm", "rb");
  }
  ~ProcessV8Wasm() override { fclose(fp_); }

  bool CanDebug(lldb::TargetSP target, bool plugin_specified_by_name) override {
    return true;
  }

  Status DoDestroy() override { return {}; }

  void RefreshStateAfterStop() override {}

  size_t DoReadMemory(lldb::addr_t vm_addr, void *buf, size_t size,
                      Status &error) override {
    if (!fp_)
      return 0;

    if (0 != fseek(fp_, vm_addr & 0xffffffff, SEEK_SET))
      return 0;

    return fread(buf, 1, size, fp_);
  }

  bool UpdateThreadList(ThreadList &old_thread_list,
                        ThreadList &new_thread_list) override {
    return false;
  }

  // PluginInterface
  ConstString GetPluginName() override {
    return ConstString("process-v8-wasm");
  }
  uint32_t GetPluginVersion() override;

private:
  FILE *fp_;
};

} // namespace lldb_private
