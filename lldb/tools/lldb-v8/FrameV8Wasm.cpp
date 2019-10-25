#include "FrameV8Wasm.h"

using namespace lldb_private;
using namespace lldb;

FrameV8Wasm::FrameV8Wasm(const lldb::ThreadSP &thread_sp, lldb::user_id_t frame_idx,
             lldb::user_id_t concrete_frame_idx, lldb::addr_t cfa,
             bool cfa_is_valid, lldb::addr_t pc, Kind frame_kind,
             bool behaves_like_zeroth_frame, const SymbolContext *sc_ptr)
  : StackFrame(thread_sp, frame_idx, concrete_frame_idx, cfa, cfa_is_valid, pc, frame_kind, behaves_like_zeroth_frame, sc_ptr) {

  }
