// clang-format off
//      Compile the test case into assembly.
// RUN: clang -O2 -S -emit-llvm -c %s -o %basename_t.ll
//      Run the FunctionInfo pass. The `-disable-output` option disables
//      outputting the bytecode because we are only checking the pass outputs
//      here.
// RUN: opt -load-pass-plugin=%dylibdir/libFunctionInfo.so -passes=function-info -disable-output %basename_t.ll 2>&1 | \
//      Check the output "CSCD70 Function Information Pass".
// RUN: FileCheck --match-full-lines %s
// clang-format on
/// @todo(CSCD70) Please remove the `--check-prefix=SAMPLE` option.
// SAMPLE: CSCD70 Function Information Pass

int g;

// CHECK-LABEL: Function Name: g_incr
// CHECK-NEXT: Number of Arguments: 1
// CHECK-NEXT: Number of Calls: 0
// CHECK-NEXT: Number OF BBs: 1
// CHECK-NEXT: Number of Instructions: 4
int g_incr(int c) {
  g += c;
  return g;
}

// CHECK-LABEL: Function Name: loop
// CHECK-NEXT: Number of Arguments: 3
// CHECK-NEXT: Number of Calls: 0
// CHECK-NEXT: Number OF BBs: 3
// CHECK-NEXT: Number of Instructions: 10
int loop(int a, int b, int c) {
  int i, ret = 0;

  for (i = a; i < b; i++) {
    g_incr(c);
  }

  return ret + g;
}
