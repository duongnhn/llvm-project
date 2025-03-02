// RUN: rm -rf %t.dir
// RUN: rm -rf %t.cdb
// RUN: mkdir -p %t.dir
// RUN: cp %s %t.dir/regular_cdb_input.cpp
// RUN: cp %s %t.dir/regular_cdb_input2.cpp
// RUN: mkdir %t.dir/Inputs
// RUN: cp %S/Inputs/header.h %t.dir/Inputs/header.h
// RUN: cp %S/Inputs/header2.h %t.dir/Inputs/header2.h
// RUN: sed -e "s|DIR|%/t.dir|g" %S/Inputs/regular_cdb.json > %t.cdb
//
// RUN: clang-scan-deps -compilation-database %t.cdb -j 1 -mode preprocess-minimized-sources | \
// RUN:   FileCheck --check-prefixes=CHECK1,CHECK2,CHECK2NO %s
// RUN: clang-scan-deps -compilation-database %t.cdb -j 1 -mode preprocess | \
// RUN:   FileCheck --check-prefixes=CHECK1,CHECK2,CHECK2NO %s
// RUN: clang-scan-deps -compilation-database %t.cdb -j 1 -mode preprocess-minimized-sources \
// RUN:   -skip-excluded-pp-ranges=0 | FileCheck --check-prefixes=CHECK1,CHECK2,CHECK2NO %s
//
// Make sure we didn't produce any dependency files!
// RUN: not cat %t.dir/regular_cdb.d
// RUN: not cat %t.dir/regular_cdb2.d
//
// The output order is non-deterministic when using more than one thread,
// so check the output using two runs. Note that the 'NOT' check is not used
// as it might fail if the results for `regular_cdb_input.cpp` are reported before
// `regular_cdb_input2.cpp`.
//
// RUN: clang-scan-deps -compilation-database %t.cdb -j 2 -mode preprocess-minimized-sources | \
// RUN:   FileCheck --check-prefix=CHECK1 %s
// RUN: clang-scan-deps -compilation-database %t.cdb -j 2 -mode preprocess | \
// RUN:   FileCheck --check-prefix=CHECK1 %s
// RUN: clang-scan-deps -compilation-database %t.cdb -j 2 -mode preprocess-minimized-sources | \
// RUN:   FileCheck --check-prefix=CHECK2 %s
// RUN: clang-scan-deps -compilation-database %t.cdb -j 2 -mode preprocess | \
// RUN:   FileCheck --check-prefix=CHECK2 %s

#include "header.h"

// CHECK1: regular_cdb_input2.cpp
// CHECK1-NEXT: Inputs{{/|\\}}header.h
// CHECK1-NEXT: Inputs{{/|\\}}header2.h

// CHECK2: regular_cdb_input.cpp
// CHECK2-NEXT: Inputs{{/|\\}}header.h
// CHECK2NO-NOT: header2
