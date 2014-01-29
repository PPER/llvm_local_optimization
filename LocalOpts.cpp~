// 15-745 S13 Assignment 1: FunctionInfo.cpp
// 
// Based on code from Todd C. Mowry
// Modified by Arthur Peters
////////////////////////////////////////////////////////////////////////////////

#include "llvm/Pass.h"
#include "llvm/PassManager.h"
#include "llvm/Transforms/IPO/PassManagerBuilder.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Module.h"
#include "llvm/Support/raw_ostream.h"

#include "llvm/IR/Constants.h"
#include "llvm/IR/Instructions.h"


#include <ostream>
#include <fstream>
#include <iostream>

using namespace llvm;

namespace {

	class OptSummary {
		public:
			int algbraicIdent;
			int constantFold;
			int strengthReduce;
			OptSummary(): algbraicIdent(0), constantFold(0), strengthReduce(0) {}
	};

	class LocalOpts : public FunctionPass {
		public:
			static char ID;
			LocalOpts() : FunctionPass(ID) {}

			virtual bool runOnFunction(Function &F) {
				OptSummary *ops = new OptSummary();
				bool funcchanged = false;


				for (Function::iterator BB = F.begin(), Be = F.end(); BB != Be; ++BB) {
					//for (BasicBlock::iterator ii = BB->begin(), ie = BB->end(); ii != ie; ++ii) {
					BasicBlock::iterator ii = BB->begin(), ie = BB->end();
					while (ii != ie) {
						bool instchanged = false;
						Value *L, *R;
						if (ii->getNumOperands() == 2) {
							L = ii->getOperand(0);
							R = ii->getOperand(1);
						}

						unsigned op = ii->getOpcode();

						APInt zero, one;
						//32 bit or 64 bit integer?
						if (IntegerType *inttype = dyn_cast<IntegerType>(ii->getType())) {
							zero = APInt(inttype->getBitWidth(), 0);
							one = APInt(inttype->getBitWidth(), 1);	
						}


						errs() << "flag 1" << "\n";

						//while () {

						//need to handle with store, because register too many cases
						if (op == Instruction::Store) {
							Value *varptr = dyn_cast<StoreInst>(ii)->getPointerOperand();
							Value *value = dyn_cast<StoreInst>(ii)->getValueOperand();
							if (isa<Constant>(*value)) {
								for (Value::use_iterator u = varptr->use_begin(), ue = varptr->use_end(); u != ue; ++u) {
									if (LoadInst *li = dyn_cast<LoadInst>(*u)) {
										li->replaceAllUsesWith(value);
										++ii;
										//we need to add counter to marker......
										//ops->algbraicIdent++;
										instchanged = true;	funcchanged = true;
										errs() << "flag***666" << "\n";
										continue;
									}
								}
							}
						} else if (op == Instruction::Add) {
							errs() << "flag3" << "\n";
							if (ConstantInt *LC = dyn_cast<ConstantInt>(L)) {
								if (LC->getValue() == zero) {
									//How to replace and delete the instruction?
									ii->replaceAllUsesWith(R);
									BasicBlock::iterator tmp = ii;
									++ii;
									tmp->eraseFromParent();
									//--ii;
									//...........the next instruction cannot be optimized??????.......
									//we need to add counter to marker......
									ops->algbraicIdent++;
									instchanged = true; funcchanged = true;
									continue;
								}
							} else if (ConstantInt *RC = dyn_cast<ConstantInt>(R)) {
								if (RC->getValue() == zero) {
									//How to replace and delete the instruction?
									ii->replaceAllUsesWith(L);

									BasicBlock::iterator tmp = ii;
									++ii;
									tmp->eraseFromParent();
									//--ii;
									//...........the next instruction cannot be optimized??????.......
									//we need to add counter to marker......
									ops->algbraicIdent++;
									instchanged = true; funcchanged = true;
									continue;
								}
							}
						} else if (op == Instruction::Sub) {
							//dyn_cast<Instruction>(L);
							
							//if ((dyn_cast<Instruction>(L))->isIdenticalTo(dyn_cast<Instruction>(R))) {
							if (L == R) {
								errs() << "flag4" << "\n";
								ii->replaceAllUsesWith(ConstantInt::get(L->getContext(), zero));
								BasicBlock::iterator tmp = ii;
								++ii;
								tmp->eraseFromParent();
								ops->algbraicIdent++;
								instchanged = true; funcchanged = true;
								continue;
							} else if (ConstantInt *RC = dyn_cast<ConstantInt>(R)) {
								if (RC->getValue() == zero) {
									ii->replaceAllUsesWith(L);
									BasicBlock::iterator tmp = ii;
									++ii;
									tmp->eraseFromParent();
									ops->algbraicIdent++;
									instchanged = true; funcchanged = true;
									continue;
								}
								//change the following cast into dyn_cast???
							} /*else if (cast<Instruction>(L)->isIdenticalTo(cast<Instruction>(R))) {
								errs() << "flag4" << "\n";
								ii->replaceAllUsesWith(ConstantInt::get(L->getContext(), zero));

								BasicBlock::iterator tmp = ii;
								++ii;
								tmp->eraseFromParent();
								ops->algbraicIdent++;
								instchanged = true; funcchanged = true;
								continue;
							} else {
								errs() << "flag5" << "\n";
							}
							*/

						} else if (op == Instruction::Mul) {
							if (ConstantInt *LC = dyn_cast<ConstantInt>(L)) {
								if (LC->getValue() == zero) {
									ii->replaceAllUsesWith(ConstantInt::get(LC->getContext(), zero));
									BasicBlock::iterator tmp = ii;
									++ii;
									tmp->eraseFromParent();
									ops->algbraicIdent++;
									instchanged = true; funcchanged = true;
									continue;
								} else if (LC->getValue() == one) {
									ii->replaceAllUsesWith(R);
									BasicBlock::iterator tmp = ii;
									++ii;
									tmp->eraseFromParent();
									ops->algbraicIdent++;
									instchanged = true; funcchanged = true;
									continue;
								}
							} else if (ConstantInt *RC = dyn_cast<ConstantInt>(R)) {
								if (RC->getValue() == zero) {
									ii->replaceAllUsesWith(ConstantInt::get(RC->getContext(), zero));
									BasicBlock::iterator tmp = ii;
									++ii;
									tmp->eraseFromParent();
									ops->algbraicIdent++;
									instchanged = true; funcchanged = true;
									continue;
								} else if (RC->getValue() == one) {
									ii->replaceAllUsesWith(L);
									BasicBlock::iterator tmp = ii;
									++ii;
									tmp->eraseFromParent();
									ops->algbraicIdent++;
									instchanged = true; funcchanged = true;
									continue;
								}
							}
						} else if (op == Instruction::UDiv || op == Instruction::SDiv) {
							//.............we need to judge whether R is 0.....
							//if R is a variable, can we just add a assert R != 0??????????????????????
							//............................assert..............................
							
							//if (dyn_cast<Instruction>(L)->isIdenticalTo(dyn_cast<Instruction>(R))) {
							//...........
							if (L == R) {//...........and L != 0
							//if (dyn_cast<Instruction>(L)->isIdenticalTo(dyn_cast<Instruction>(R))) {
							//L is not a const........if (cast<ConstantInt>(L)->isIdenticalTo(cast<ConstantInt>(R))) {
								ii->replaceAllUsesWith(ConstantInt::get(L->getContext(), one));
								BasicBlock::iterator tmp = ii;
								++ii;
								tmp->eraseFromParent();
								ops->algbraicIdent++;
								instchanged = true; funcchanged = true;
								continue;
							} else if (ConstantInt *LC = dyn_cast<ConstantInt>(L)) {
								if (LC->getValue() == zero) {
									ii->replaceAllUsesWith(ConstantInt::get(LC->getContext(), zero));
									BasicBlock::iterator tmp = ii;
									++ii;
									tmp->eraseFromParent();
									ops->algbraicIdent++;
									instchanged = true; funcchanged = true;
									continue;
								}	
							} else if (ConstantInt *RC = dyn_cast<ConstantInt>(R)) {
								if (RC->getValue() == one) {
									ii->replaceAllUsesWith(L);
									BasicBlock::iterator tmp = ii;
									++ii;
									tmp->eraseFromParent();
									ops->algbraicIdent++;
									instchanged = true; funcchanged = true;
									continue;
								}
							}
						} else {

						}

						if (ii->getNumOperands() == 2 && isa<Constant>(L) && isa<Constant>(R)) {
							Value *result = calcOpRes(op, cast<ConstantInt>(L), cast<ConstantInt>(R));
							ii->replaceAllUsesWith(result);
							BasicBlock::iterator tmp = ii;
							++ii;
							tmp->eraseFromParent();
							ops->constantFold++;
							instchanged = true; funcchanged = true;
							continue;
						}

						//Strength Reductions
						if (op == Instruction::Mul) {
							if (ConstantInt *LC = dyn_cast<ConstantInt>(L)) {
								APInt mulval = LC->getValue();
								if (mulval.isPowerOf2()) {
									unsigned lshift = mulval.logBase2();
									BinaryOperator *newInst = BinaryOperator::Create(Instruction::Shl, R, ConstantInt::get(L->getType(), lshift));// isSigned is set to false
									//ii->getParent()->getInstList().insertafter(ii, newInst);
									//insert before ii
									ii->getParent()->getInstList().insert(ii, newInst);
									ii->replaceAllUsesWith(newInst);
									BasicBlock::iterator tmp = ii;
									++ii;
									//delete the original ii
									tmp->eraseFromParent();
									ops->strengthReduce++;
									instchanged = true; funcchanged = true;
									continue;
								} 
							} else if (ConstantInt *RC = dyn_cast<ConstantInt>(R)) {
								APInt mulval = RC->getValue();
								if (mulval.isPowerOf2()) {
									unsigned lshift = mulval.logBase2();
									BinaryOperator *newInst = BinaryOperator::Create(Instruction::Shl, L, ConstantInt::get(R->getType(), lshift));// isSigned is set to false
									//ii->getParent()->getInstList().insertafter(ii, newInst);
									//insert before ii
									ii->getParent()->getInstList().insert(ii, newInst);
									ii->replaceAllUsesWith(newInst);
									BasicBlock::iterator tmp = ii;
									++ii;
									//delete the original ii
									tmp->eraseFromParent();
									ops->strengthReduce++;
									instchanged = true; funcchanged = true;
									continue;
								}
							}
						} else if (op == Instruction::SDiv || op == Instruction::UDiv) {
							if (ConstantInt *LC = dyn_cast<ConstantInt>(L)) {
								APInt divval = LC->getValue();
								if (divval.isPowerOf2()) {
									unsigned rshift = divval.logBase2();
									BinaryOperator *newInst;
									if (op == Instruction::SDiv) {
										newInst = BinaryOperator::Create(Instruction::AShr, R, ConstantInt::get(L->getType(), rshift));// isSigned is set to false
									} else {
										newInst = BinaryOperator::Create(Instruction::LShr, R, ConstantInt::get(L->getType(), rshift));// isSigned is set to false
									}
									//ii->getParent()->getInstList().insertafter(ii, newInst);
									//insert before ii
									ii->getParent()->getInstList().insert(ii, newInst);
									ii->replaceAllUsesWith(newInst);
									BasicBlock::iterator tmp = ii;
									++ii;
									//delete the original ii
									tmp->eraseFromParent();
									ops->strengthReduce++;
									instchanged = true; funcchanged = true;
									continue;
								} 
							} else if (ConstantInt *RC = dyn_cast<ConstantInt>(R)) {
								APInt divval = RC->getValue();
								if (divval.isPowerOf2()) {
									unsigned rshift = divval.logBase2();
									BinaryOperator *newInst;
									if (op == Instruction::SDiv) {
										newInst = BinaryOperator::Create(Instruction::AShr, L, ConstantInt::get(R->getType(), rshift));// isSigned is set to false
									} else {
										newInst = BinaryOperator::Create(Instruction::LShr, L, ConstantInt::get(R->getType(), rshift));// isSigned is set to false
									}
									//ii->getParent()->getInstList().insertafter(ii, newInst);
									//insert before ii
									ii->getParent()->getInstList().insert(ii, newInst);
									ii->replaceAllUsesWith(newInst);
									BasicBlock::iterator tmp = ii;
									++ii;
									//delete the original ii
									tmp->eraseFromParent();
									ops->strengthReduce++;
									instchanged = true; funcchanged = true;
									continue;
								}
							}
						}


						if (!instchanged) {
							++ii;
						} else {
							//funcchanged = true; 
						}

					}


				}

				errs() << "Summary of Optimizations\n";
				errs() << "Algebraic Identities: " << ops->algbraicIdent << "\n";
				errs() << "Constant Folding: " << ops->constantFold << "\n";
				errs() << "Strength Reduction: " << ops->strengthReduce << "\n";
				return funcchanged;
			}
				
			ConstantInt *calcOpRes(unsigned op, ConstantInt *L, ConstantInt *R) {
				if (op == Instruction::Add) {
					return ConstantInt::get(L->getContext(), L->getValue() + R->getValue());
					//return ConstantInt::get(cast<IntegerType>(ii->getType()), L->getValue() + R->getValue());
					//return cast<ConstantInt>(ConstantInt::get(L->getType(), L->getValue() + R->getValue()));
				} else if (op == Instruction::Sub) {
					return ConstantInt::get(L->getContext(), L->getValue() - R->getValue());
				} else if (op == Instruction::Mul) {
					return ConstantInt::get(L->getContext(), L->getValue() * R->getValue());
				} else if (op == Instruction::UDiv) {
					return ConstantInt::get(L->getContext(), L->getValue().udiv(R->getValue()));
				} else if (op == Instruction::SDiv) {
					return ConstantInt::get(L->getContext(), L->getValue().sdiv(R->getValue()));
				} else {
					errs() << "error....";
				}
			}

			// We don't modify the program, so we preserve all analyses
			virtual void getAnalysisUsage(AnalysisUsage &AU) const {
				AU.setPreservesAll();
			}
	};

	// LLVM uses the address of this static member to identify the pass, so the
	// initialization value is unimportant.
	char LocalOpts::ID = 0;

	// Register this pass to be used by language front ends.
	// This allows this pass to be called using the command:
	//    clang -c -Xclang -load -Xclang ./FunctionInfo.so loop.c
	static void registerMyPass(const PassManagerBuilder &,
			PassManagerBase &PM) {
		PM.add(new LocalOpts());
	}
	RegisterStandardPasses
		RegisterMyPass(PassManagerBuilder::EP_EarlyAsPossible,
				registerMyPass);

	// Register the pass name to allow it to be called with opt:
	//    clang -c -emit-llvm loop.c
	//    opt -load ./FunctionInfo.so -function-info loop.bc > /dev/null
	// See http://llvm.org/releases/3.4/docs/WritingAnLLVMPass.html#running-a-pass-with-opt for more info.
	RegisterPass<LocalOpts> X("my-local-opts", "my-local-opts");

}
