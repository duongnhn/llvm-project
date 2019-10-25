#include "lldb/Target/Thread.h"

namespace lldb_private {

class ProcessV8Wasm;

class ThreadV8Wasm : public Thread {
 public:
  ThreadV8Wasm(Process &process, lldb::tid_t tid);
  ~ThreadV8Wasm() override {}

  lldb::RegisterContextSP GetRegisterContext() override {
    if (!m_reg_context_sp)
      m_reg_context_sp = CreateRegisterContextForFrame(nullptr);
    return m_reg_context_sp;
  }

  void RefreshStateAfterStop() override {}
  bool CalculateStopInfo() override { return false; }

private:
  lldb::RegisterContextSP CreateRegisterContextForFrame(StackFrame *frame);
};

}  // namespace lldb_private
