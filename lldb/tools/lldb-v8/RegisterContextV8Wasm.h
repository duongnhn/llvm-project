
#include "lldb/Target/RegisterContext.h"

namespace lldb_private {

class ThreadV8Wasm;

class RegisterContextV8Wasm : public RegisterContext {
 public:
  RegisterContextV8Wasm(ThreadV8Wasm &thread, uint32_t concrete_frame_idx);


  void InvalidateAllRegisters() override {}

  size_t GetRegisterCount() override { return 0; }

  const RegisterInfo *GetRegisterInfoAtIndex(size_t reg) override {
    return nullptr;
  }

  size_t GetRegisterSetCount() override { return 0; }

  const RegisterSet *GetRegisterSet (size_t reg_set) override {
    return nullptr;
  }

  bool ReadRegister(const RegisterInfo *reg_info,
                            RegisterValue &reg_value) override {
                              return false;
                            }

  bool WriteRegister(const RegisterInfo *reg_info,
                             const RegisterValue &reg_value) override {
                               return false;
                             }

  uint32_t ConvertRegisterKindToRegisterNumber(lldb::RegisterKind kind,
                                                       uint32_t num) override { return 0; }
};

}  // namespace lldb_private
