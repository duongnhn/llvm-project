#include "ThreadV8Wasm.h"
#include "ProcessV8Wasm.h"
#include "RegisterContextV8Wasm.h"

using namespace lldb_private;
using namespace lldb;

ThreadV8Wasm::ThreadV8Wasm(Process &process, lldb::tid_t tid): Thread(process, tid) {}

RegisterContextSP ThreadV8Wasm::CreateRegisterContextForFrame(StackFrame *frame) {
  RegisterContextSP reg_ctx_sp;
  uint32_t concrete_frame_idx = 0;

  if (frame)
    concrete_frame_idx = frame->GetConcreteFrameIndex();

  if (concrete_frame_idx == 0) {
    ProcessSP process_sp(GetProcess());
    if (process_sp) {
      // ProcessV8Wasm *wasm_process =
      //     static_cast<ProcessV8Wasm *>(process_sp.get());
      //////// read_all_registers_at_once will be true if 'p' packet is not
      //////// supported.
      //////bool read_all_registers_at_once =
      //////    !gdb_process->GetGDBRemote().GetpPacketSupported(GetID());
      reg_ctx_sp = std::make_shared<RegisterContextV8Wasm>(
          *this, concrete_frame_idx);// /*wasm_process->m_register_info*/ nullptr, false);
    }
  } else {
    //////Unwind *unwinder = GetUnwinder();
    //////if (unwinder != nullptr)
    //////  reg_ctx_sp = unwinder->CreateRegisterContextForFrame(frame);
  }
  return reg_ctx_sp;
}
