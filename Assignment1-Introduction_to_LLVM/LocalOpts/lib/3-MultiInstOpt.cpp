#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"
#include <llvm/IR/Constants.h>

using namespace llvm;

bool isSame(const Value *X, const Value *Y) {
  if (X == Y) {
    return true;
  }

  if (const ConstantInt *ConstIntX = dyn_cast<ConstantInt>(X)) {
    if (const ConstantInt *ConstIntY = dyn_cast<ConstantInt>(Y)) {
      return ConstIntX->getSExtValue() == ConstIntY->getSExtValue();
    }
  }

  return false;
}

std::optional<Value *> findReplacedValue(BinaryOperator *BinaryInst) {
  for (auto *Operand = BinaryInst->op_begin(); Operand != BinaryInst->op_end();
       ++Operand) {
    bool IsFirst = Operand == BinaryInst->op_begin();
    Value *TheOtherOperand;
    if (IsFirst) {
      TheOtherOperand = BinaryInst->getOperand(1);
    } else {
      TheOtherOperand = BinaryInst->getOperand(0);
    }

    if (BinaryOperator *OperandBinaryInst = dyn_cast<BinaryOperator>(Operand)) {
      switch (BinaryInst->getOpcode()) {
      case Instruction::Add:
        if (OperandBinaryInst->getOpcode() == Instruction::Sub) {
          if (isSame(TheOtherOperand, OperandBinaryInst->getOperand(1))) {
            return OperandBinaryInst->getOperand(0);
          }
        }
        break;
      case Instruction::Sub:
        if (IsFirst && OperandBinaryInst->getOpcode() == Instruction::Add) {
          if (isSame(TheOtherOperand, OperandBinaryInst->getOperand(1))) {
            return OperandBinaryInst->getOperand(0);
          }

          if (isSame(TheOtherOperand, OperandBinaryInst->getOperand(0))) {
            return OperandBinaryInst->getOperand(1);
          }
        }
        break;
      case Instruction::SDiv:
        if (IsFirst && OperandBinaryInst->getOpcode() == Instruction::Mul) {
          if (isSame(TheOtherOperand, OperandBinaryInst->getOperand(1))) {
            return OperandBinaryInst->getOperand(0);
          }

          if (isSame(TheOtherOperand, OperandBinaryInst->getOperand(0))) {
            return OperandBinaryInst->getOperand(1);
          }
        }
        break;
      default:
        break;
      }
    }
  }

  return std::nullopt;
}

PreservedAnalyses MultiInstOptPass::run([[maybe_unused]] Function &F,
                                        FunctionAnalysisManager &) {

  /// @todo(CSCD70) Please complete this method.
  std::vector<llvm::Instruction *> InstsToDelete;

  for (auto BB = F.begin(); BB != F.end(); ++BB) {
    for (auto Inst = BB->begin(); Inst != BB->end(); ++Inst) {
      if (BinaryOperator *BinaryInst = dyn_cast<BinaryOperator>(Inst)) {
        auto Result = findReplacedValue(BinaryInst);
        if (Result.has_value()) {
          BinaryInst->replaceAllUsesWith(Result.value());
          InstsToDelete.push_back(BinaryInst);
        }
      }
    }
  }

  // remove the recorded insts
  for (auto *Inst : InstsToDelete) {
    Inst->eraseFromParent();
  }

  return PreservedAnalyses::none();
}
