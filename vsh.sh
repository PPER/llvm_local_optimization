clang -O0 -emit-llvm -c $1.c
opt -mem2reg $1.bc -o $1-simp.bc
opt -load ./LocalOpts.so -my-local-opts $1-simp.bc -o $1-opt.bc
