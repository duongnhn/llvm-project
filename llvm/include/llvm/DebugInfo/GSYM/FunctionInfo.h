//===- FunctionInfo.h -------------------------------------------*- C++ -*-===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//

#ifndef LLVM_DEBUGINFO_GSYM_FUNCTIONINFO_H
#define LLVM_DEBUGINFO_GSYM_FUNCTIONINFO_H

#include "llvm/ADT/Optional.h"
#include "llvm/DebugInfo/GSYM/InlineInfo.h"
#include "llvm/DebugInfo/GSYM/LineTable.h"
#include "llvm/DebugInfo/GSYM/Range.h"
#include "llvm/DebugInfo/GSYM/StringTable.h"
#include <tuple>
#include <vector>

namespace llvm {
class raw_ostream;
namespace gsym {

/// Function information in GSYM files encodes information for one
/// contiguous address range. The name of the function is encoded as
/// a string table offset and allows multiple functions with the same
/// name to share the name string in the string table. Line tables are
/// stored in a sorted vector of gsym::LineEntry objects and are split
/// into line tables for each function. If a function has a discontiguous
/// range, it will be split into two gsym::FunctionInfo objects. If the
/// function has inline functions, the information will be encoded in
/// the "Inline" member, see gsym::InlineInfo for more information.
struct FunctionInfo {
  AddressRange Range;
  uint32_t Name; ///< String table offset in the string table.
  llvm::Optional<LineTable> LineTable;
  llvm::Optional<InlineInfo> Inline;

  FunctionInfo(uint64_t Addr = 0, uint64_t Size = 0, uint32_t N = 0)
      : Range(Addr, Addr + Size), Name(N) {}

  bool hasRichInfo() const {
    /// Returns whether we have something else than range and name. When
    /// converting information from a symbol table and from debug info, we
    /// might end up with multiple FunctionInfo objects for the same range
    /// and we need to be able to tell which one is the better object to use.
    return LineTable.hasValue() || Inline.hasValue();
  }

  bool isValid() const {
    /// Address and size can be zero and there can be no line entries for a
    /// symbol so the only indication this entry is valid is if the name is
    /// not zero. This can happen when extracting information from symbol
    /// tables that do not encode symbol sizes. In that case only the
    /// address and name will be filled in.
    return Name != 0;
  }

  uint64_t startAddress() const { return Range.Start; }
  uint64_t endAddress() const { return Range.End; }
  uint64_t size() const { return Range.size(); }
  void setStartAddress(uint64_t Addr) { Range.Start = Addr; }
  void setEndAddress(uint64_t Addr) { Range.End = Addr; }
  void setSize(uint64_t Size) { Range.End = Range.Start + Size; }

  void clear() {
    Range = {0, 0};
    Name = 0;
    LineTable = llvm::None;
    Inline = llvm::None;
  }
};

inline bool operator==(const FunctionInfo &LHS, const FunctionInfo &RHS) {
  return LHS.Range == RHS.Range && LHS.Name == RHS.Name &&
         LHS.LineTable == RHS.LineTable && LHS.Inline == RHS.Inline;
}
inline bool operator!=(const FunctionInfo &LHS, const FunctionInfo &RHS) {
  return !(LHS == RHS);
}
/// This sorting will order things consistently by address range first, but then
/// followed by inlining being valid and line tables. We might end up with a
/// FunctionInfo from debug info that will have the same range as one from the
/// symbol table, but we want to quickly be able to sort and use the best version
/// when creating the final GSYM file.
inline bool operator<(const FunctionInfo &LHS, const FunctionInfo &RHS) {
  // First sort by address range
  if (LHS.Range != RHS.Range)
    return LHS.Range < RHS.Range;

  // Then sort by inline
  if (LHS.Inline.hasValue() != RHS.Inline.hasValue())
    return RHS.Inline.hasValue();

  return LHS.LineTable < RHS.LineTable;
}

raw_ostream &operator<<(raw_ostream &OS, const FunctionInfo &R);

} // namespace gsym
} // namespace llvm

#endif // #ifndef LLVM_DEBUGINFO_GSYM_FUNCTIONINFO_H
