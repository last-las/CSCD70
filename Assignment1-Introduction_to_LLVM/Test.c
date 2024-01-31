#include <stdio.h>
// Command:
/*
clang-16 -O0 -Xclang -disable-O0-optnone -emit-llvm -c Test.c
llvm-dis-16 ./Test.bc -o=./Test.ll
opt-16 -passes=mem2reg Test.bc -o TestM2R.bc
llvm-dis-16 ./TestM2R.bc -o=./TestM2R.ll
 * */
void bar(int b) {
	int x = b;
	int y = x << 2;
	printf("%d, %d\n", x, y);
}
