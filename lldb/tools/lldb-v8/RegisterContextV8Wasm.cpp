#include "RegisterContextV8Wasm.h"
#include "ThreadV8Wasm.h"

using namespace lldb_private;

RegisterContextV8Wasm::RegisterContextV8Wasm(ThreadV8Wasm &thread, uint32_t concrete_frame_idx)
:RegisterContext(thread, concrete_frame_idx) {}
