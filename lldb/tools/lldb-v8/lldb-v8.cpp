//===- lldb-v8.cpp ------------------------------------------ *- C++ --*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#include "SystemInitializerV8.h"
#include "ProcessV8Wasm.h"
#include "ThreadV8Wasm.h"
#include "FrameV8Wasm.h"

#include "lldb/API/SBFileSpec.h"
#include "lldb/API/SBModule.h"
#include "lldb/API/SBModuleSpec.h"
#include "lldb/API/SBTarget.h"

#include "Plugins/SymbolFile/DWARF/SymbolFileDWARF.h"
#include "lldb/Breakpoint/BreakpointLocation.h"
#include "lldb/Core/Debugger.h"
#include "lldb/Core/Module.h"
#include "lldb/Core/Section.h"
#include "lldb/Core/ValueObjectVariable.h"
#include "lldb/Expression/IRMemoryMap.h"
#include "lldb/Initialization/SystemLifetimeManager.h"
#include "lldb/Interpreter/CommandInterpreter.h"
#include "lldb/Interpreter/CommandReturnObject.h"
#include "lldb/Symbol/ClangASTContext.h"
#include "lldb/Symbol/ClangASTImporter.h"
#include "lldb/Symbol/CompileUnit.h"
#include "lldb/Symbol/LineTable.h"
#include "lldb/Symbol/SymbolFile.h"
#include "lldb/Symbol/TypeList.h"
#include "lldb/Symbol/TypeMap.h"
#include "lldb/Symbol/VariableList.h"
#include "lldb/Target/Language.h"
#include "lldb/Target/Process.h"
#include "lldb/Target/RegisterContext.h"
#include "lldb/Utility/DataExtractor.h"
#include "lldb/Utility/State.h"
#include "lldb/Utility/StreamString.h"

#include "Plugins/DynamicLoader/WASM-DYLD/DynamicLoaderWasmDYLD.h"
#include "Plugins/SymbolVendor/WASM/SymbolVendorWasm.h"

#include "llvm/ADT/IntervalMap.h"
#include "llvm/ADT/ScopeExit.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/ManagedStatic.h"
#include "llvm/Support/MathExtras.h"
#include "llvm/Support/Path.h"
#include "llvm/Support/PrettyStackTrace.h"
#include "llvm/Support/Signals.h"
#include "llvm/Support/WithColor.h"
#include <cstdio>
#include <thread>
#include <iostream>
using namespace std;
using namespace lldb;
using namespace lldb_private;
using namespace llvm;


int main(int argc, const char *argv[]) {

  SystemLifetimeManager DebuggerLifetime;
  if (auto e = DebuggerLifetime.Initialize(
          std::make_unique<SystemInitializerV8>(), nullptr)) {
    // WithColor::error() << "initialization failed: " <<
    // toString(std::move(e))
    //                   << '\n';
    cout << "initialization failed";
    return 1;
  }

  ArchSpec arch("wasm32-unknown-unknown-wasm");
  const char *triple = arch.GetTriple().getTriple().c_str();

  DebuggerSP debugger_sp = Debugger::CreateInstance();

  TargetSP target_sp;
  lldb_private::Status error = debugger_sp->GetTargetList().CreateTarget(
      *debugger_sp, "", triple, eLoadDependentsNo, nullptr, target_sp);
  if (!target_sp || error.Fail())
    return 1;

  debugger_sp->GetTargetList().SetSelectedTarget(target_sp.get());

  ListenerSP listener_sp = debugger_sp->GetListener();
  ProcessSP process_sp = /*target_sp->CreateProcess(
      debugger_sp->GetListener(), "gdb-remote", nullptr);*/
      std::make_shared<ProcessV8Wasm>(target_sp, listener_sp);
  if (!process_sp)
    return 1;
  // process_sp->CompleteAttach();

  FileSpec file;
  DynamicLoader *loader =
      DynamicLoaderWasmDYLD::CreateInstance(process_sp.get(), true);
  ModuleSP module_sp =
      loader->LoadModuleAtAddress(file, -1, 0x500000000, false);
  delete loader;


  SymbolVendor *symbol_vendor =
      SymbolVendorWasm::CreateInstance(module_sp, nullptr);
  delete symbol_vendor;

  // Dump by address
  // Address addr(0x000007f3, module_sp->GetSectionList());
  Address addr(0x00000087, module_sp->GetSectionList());

  SymbolContextItem actual_resolve_scope = SymbolContextItem::eSymbolContextEverything;
  SymbolContext sc;
  uint32_t resolved = 0x18;
  // eSymbolContextBlock | eSymbolContextFunction;
  resolved |= module_sp->ResolveSymbolContextForAddress(
      addr, actual_resolve_scope, sc,
      /*resolve_tail_call_address =*/false);

  StreamString stream;
  sc.Dump(&stream, target_sp.get());
  cout << "\n Start: \n";
  cout << stream.GetData() << '\n';
  cout << "End. \n";

  tid_t tid = 1;
  ThreadSP thread_sp = std::make_shared<ThreadV8Wasm>(*process_sp, tid);
  // ExecutionContext exe_ctx;
  // exe_ctx.SetTargetSP(target_sp);
  // exe_ctx.SetProcessSP(process_sp);
  // exe_ctx.SetThreadSP(thread_sp);
  // exe_ctx.SetFrameSP(thread_sp->GetStackFrameAtIndex(0));


  // Get variable list from a stackframe.

  // This is a customized stackframe, also we mock the response in GDBRemoteCommunicationClient::GetWasmCallStack (not needed)
  // to return
  // [21474836736, 21474836770] 21474836480 = 0x500000000
  // [21474836615, 21474836706, 21474836770]
  lldb::user_id_t frame_idx = 0;
  lldb::user_id_t concrete_frame_idx = 0;
  lldb::addr_t pc = 0x500000003;
  lldb::addr_t cfa = 0;
  bool cfa_is_valid = true;
  StackFrame::Kind frame_kind = StackFrame::Kind::Regular;
  bool behaves_like_zeroth_frame = true;

  SymbolContext sc_ptr;

  // StackFrame frame(thread_sp, frame_idx,
  //            concrete_frame_idx, cfa,
  //            cfa_is_valid, pc, frame_kind,
  //            behaves_like_zeroth_frame, &sc_ptr);
  StackFrameSP frame_sp = std::make_shared<FrameV8Wasm>(thread_sp, frame_idx,
             concrete_frame_idx, cfa,
             cfa_is_valid, pc, frame_kind,
             behaves_like_zeroth_frame, &sc_ptr);
  StackFrame *frame = frame_sp.get();

  //StackFrame: pc = 21474836615, cfa = 0; stack_frame_kind = Regular
  // 21474836615

  VariableList *variable_list = frame->GetVariableList(/*show_globals*/true);

  stream.Clear();
  if (variable_list) {
    variable_list->Dump(&stream, false);
  }
  cout << "\n Start: \n";
  cout << variable_list->GetSize() << "\n";
  cout << stream.GetData() << "\n";
  cout << "End. \n";

  ConstString name = ConstString("out");
  VariableSP variable_sp = variable_list->FindVariable(name);
  Variable *variable = variable_sp.get();

  stream.Clear();
  variable->Dump(&stream, false);

  cout << "\n Start: \n";
  cout << stream.GetData() << "\n";
  cout << "End. \n";

  // // ValueObjectVariable variable_object_variable =   static lldb::ValueObjectSP Create(ExecutionContextScope *exe_scope,
  //                                   const lldb::VariableSP &var_sp);


  // Go through ValueObject, need to pass better ExecutionContextScope with Process, Thread, Frame
  // Target *target = target_sp.get();
  // lldb::ValueObjectSP value_object_sp = lldb_private::ValueObjectVariable::Create(target_sp.get(), variable_sp);
  // ValueObject *value_object = value_object_sp.get();

  // stream.Clear();
  // value_object->UpdateValueIfNeeded(/*update_format*/ false);

  // Note: Frame need a SP to work in ExecutionContext
  // Variable *variable = m_variable_sp.get();
  ExecutionContext exe_ctx(process_sp.get(), thread_sp.get(), frame);
  exe_ctx.SetTargetSP(target_sp);
  lldb_private::DWARFExpression &expr = variable->LocationExpression();
  lldb_private::Value value;
  uint64_t byte_size = 4; //GetByteSize();
  Status m_error;
  lldb::addr_t loclist_base_load_addr = LLDB_INVALID_ADDRESS;
  if (expr.Evaluate(&exe_ctx, nullptr, loclist_base_load_addr, nullptr,
                    nullptr, byte_size, value, &m_error)) {
  }

  stream.Clear();
  value.Dump(&stream);

  cout << "\n Start: \n";
  cout << stream.GetData() << "\n";
  cout << "End. \n";

  // cout << "\n Start: \n";
  // cout << stream.GetData() << "\n";
  // cout << "End. \n";
  // call UpdateValueIfNeeded

  // bool StackFrame::GetFrameBaseValue(Scalar &frame_base, Status *error_ptr)

  // wasmModuleId = 5; frame_index = 0; index = 3 -> value = 66528

  // WasmReadMemory buffer[4096] = [6, 0, 0, 0], byte_size = 4; -> address = 66540 (uconst_value = 0xc)

  // WasmReadMemory buffer[4096] = [3, 0, 0, 0], byte_size = 4; -> address = 66536 (uconst_value = 0x8)

  // m_integer = 1269615769504
  return 0;
}
