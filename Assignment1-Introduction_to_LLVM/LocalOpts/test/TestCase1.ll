; RUN: opt -load-pass-plugin=%dylibdir/libLocalOpts.so \
; RUN:     -p=algebraic-identity,strength-reduction,multi-inst-opt \
; RUN:     -S %s -o %basename_t
; RUN: FileCheck --match-full-lines %s --input-file=%basename_t

; #include <stdio.h>

; void foo(int a) {
;   int r0 = a + 0;
;   int r1 = r0 * 16;
;   int r2 = r1 * r0;
;   int r3 = r2 / a;
;   int r4 = r2 / 10;
;   int r5 = 54 * r3;
;   int r6 = r4 / 128;
;   int r7 = r5 / 54;
;   int r8 = r4 / 1;
;   int r9 = r7 - 0;
;   printf("%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\n", r0, r1, r2, r3, r4, r5, r6, r7, r8,
;          r9);
; }

@.str = private unnamed_addr constant [31 x i8] c"%d,%d,%d,%d,%d,%d,%d,%d,%d,%d\0A\00", align 1

define dso_local void @foo(i32 noundef %0) {
; CHECK-LABEL: define dso_local void @foo(i32 noundef %0) {
; @todo(CSCD70) Please complete the CHECK directives.
; NOTE: The next inst is removed: %2(old) = %0(new)
  %2 = add nsw i32 %0, 0
; CHECK-NEXT: %2 = shl i32 %0, 4
  %3 = mul nsw i32 %2, 16
; CHECK-NEXT: %3 = mul nsw i32 %2, %0
  %4 = mul nsw i32 %3, %2
; NOTE: The next inst is removed: %5(old) = %2(new)
  %5 = sdiv i32 %4, %0
; CHECK-NEXT: %4 = sdiv i32 %3, 10
  %6 = sdiv i32 %4, 10
; CHECK-NEXT: %5 = mul nsw i32 54, %2
  %7 = mul nsw i32 54, %5
; CHECK-NEXT: %6 = ashr i32 %4, 7
  %8 = sdiv i32 %6, 128
; NOTE: The next inst is removed: %9(old) = %5(new)
  %9 = sdiv i32 %7, 54
; NOTE: The next inst is removed: %10(old) = %4(new)
  %10 = sdiv i32 %6, 1
; NOTE: The next inst is removed: %11(old) = %5(new)
  %11 = sub nsw i32 %9, 0
; CHECK-NEXT: %7 = call i32 (ptr, ...) @printf(ptr noundef @.str, i32 noundef %0, i32 noundef %2, i32 noundef %3, i32 noundef %2, i32 noundef %4, i32 noundef %5, i32 noundef %6, i32 noundef %2, i32 noundef %4, i32 noundef %2)
  %12 = call i32 (ptr, ...) @printf(ptr noundef @.str, i32 noundef %2, i32 noundef %3, i32 noundef %4, i32 noundef %5, i32 noundef %6, i32 noundef %7, i32 noundef %8, i32 noundef %9, i32 noundef %10, i32 noundef %11)
  ret void
}

declare i32 @printf(ptr noundef, ...) #1
