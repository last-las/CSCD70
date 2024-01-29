#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"

using namespace llvm;

std::optional<Value *> isAlgebraicIdentity(BinaryOperator *BinaryInst) {
  for (auto *Operand = BinaryInst->op_begin(); Operand != BinaryInst->op_end();
       ++Operand) {
    bool IsFirst = Operand == BinaryInst->op_begin();
    Value *TheOtherOperand;
    if (IsFirst) {
      TheOtherOperand = BinaryInst->getOperand(1);
    } else {
      TheOtherOperand = BinaryInst->getOperand(0);
    }

    if (const Constant *ConstOperand = dyn_cast<Constant>(Operand)) {
      switch (BinaryInst->getOpcode()) {
      case Instruction::Add:
      case Instruction::Sub:
        if (ConstOperand->isZeroValue()) {
          return TheOtherOperand;
        }
        break;
      case Instruction::Mul:
        if (ConstOperand->isOneValue()) {
          return TheOtherOperand;
        }
        break;
      case Instruction::SDiv:
        if (!IsFirst && ConstOperand->isOneValue()) {
          return TheOtherOperand;
        }
        break;
      default:
        break;
      }
    }
  }

  return std::nullopt;
}

PreservedAnalyses AlgebraicIdentityPass::run([[maybe_unused]] Function &F,
                                             FunctionAnalysisManager &) {

  /// @todo(CSCD70) Please complete this method.
  std::vector<llvm::Instruction *> InstsToDelete;
  for (auto BB = F.begin(); BB != F.end(); ++BB) {
    for (auto Inst = BB->begin(); Inst != BB->end(); ++Inst) {
      if (BinaryOperator *BinaryInst = dyn_cast<BinaryOperator>(Inst)) {
        // determine whether current inst is algebraic identity
        auto Result = isAlgebraicIdentity(BinaryInst);
        if (!Result.has_value()) {
          continue;
        }
        Value *NewOperand = Result.value();

        // replace the use of current inst with the simplified operand
        BinaryInst->replaceAllUsesWith(NewOperand);
        assert(BinaryInst->getNumUses() == 0 &&
               "The binary inst shouldn't have any uses");

        // record the inst
        InstsToDelete.push_back(BinaryInst);
      }
    }
  }

  // remove the recorded insts
  for (auto *Inst : InstsToDelete) {
    Inst->eraseFromParent();
  }

  return PreservedAnalyses::none();
}