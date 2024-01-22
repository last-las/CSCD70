// clang-format off
// RUN: clang -O2 -S -emit-llvm -c %s -o %basename_t.ll
// RUN: opt -load-pass-plugin=%dylibdir/libFunctionInfo.so -passes=function-info -disable-output %basename_t.ll 2>&1 | \
//      Check the output "CSCD70 Function Information Pass".
// RUN: FileCheck --match-full-lines %s
// clang-format on
/// @todo(CSCD70) Please Remove the `--check-prefix=SAMPLE` option and add the
///               CHECK directives similar to those in Loop.c.
// SAMPLE: CSCD70 Function Information Pass
#include <stdarg.h>
#include <stdio.h>

// CHECK-LABEL: Function Name: printf
// CHECK-NEXT: Number of Arguments: 1+*
// CHECK-NEXT: Number of Calls: 5
// CHECK-NEXT: Number OF BBs: 1
// CHECK-NEXT: Number of Instructions: 8
int printf(const char *format, ...) {
  int ret;
  va_list args;
  va_start(args, format);
  ret = vfprintf(stdout, format, args);
  va_end(args);

  return ret;
}

// CHECK-LABEL: Function Name: Fibonacci
// CHECK-NEXT: Number of Arguments: 1
// CHECK-NEXT: Number of Calls: 3
// CHECK-NEXT: Number OF BBs: 5
// CHECK-NEXT: Number of Instructions: 15
int Fibonacci(const int n) {
  if (n == 0) {
    printf("f(0) = 0");
    return 0;
  }
  if (n == 1) {
    printf("f(1) = 1");
    return 1;
  }
  printf("f(%d) = f(%d) + f(%d)", n, n - 1, n - 2);
  return Fibonacci(n - 1) + Fibonacci(n - 2);
}
