#include "lldb/Target/StackFrame.h"

namespace lldb_private {

class FrameV8Wasm : public StackFrame {
 public:

  FrameV8Wasm(const lldb::ThreadSP &thread_sp, lldb::user_id_t frame_idx,
             lldb::user_id_t concrete_frame_idx, lldb::addr_t cfa,
             bool cfa_is_valid, lldb::addr_t pc, Kind frame_kind,
             bool behaves_like_zeroth_frame, const SymbolContext *sc_ptr);


  ~FrameV8Wasm() override {}

};

}  // namespace lldb_private
