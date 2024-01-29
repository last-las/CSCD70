#include "LocalOpts.h"
#include "llvm/IR/InstrTypes.h"
#include <llvm/IR/Constants.h>

using namespace llvm;

bool isMultipleOfTwo(const Constant *Constant) {
  if (const ConstantInt *ConstInt = dyn_cast<ConstantInt>(Constant)) {
    auto SExtValue = ConstInt->getSExtValue();
    return (SExtValue > 0) && (SExtValue & (SExtValue - 1)) == 0;
  }
  return false;
}

uint32_t getHighestBit(uint64_t Value) {
  auto R = 0;
  while (Value >>= 1) {
    R++;
  }

  return R;
}

// If is strength reducible, return
// (var operand, constant operand, the new operation).
// std::optional<std::tuple<Value *, Value *, Instruction::BinaryOps>>
std::optional<std::tuple<Value *, Value *, Instruction::BinaryOps>>
strengthReductionInner(BinaryOperator *BinaryInst) {
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
      case Instruction::Mul:
        if (isMultipleOfTwo(ConstOperand)) {
          return std::make_tuple(TheOtherOperand, Operand->get(),
                                 Instruction::Shl);
        }
        break;
      case Instruction::SDiv:
        if (!IsFirst && isMultipleOfTwo(ConstOperand)) {
          return std::make_tuple(TheOtherOperand, Operand->get(),
                                 Instruction::AShr);
        }
        break;
      default:
        break;
      }
    }
  }

  return std::nullopt;
}

PreservedAnalyses StrengthReductionPass::run([[maybe_unused]] Function &F,
                                             FunctionAnalysisManager &) {

  /// @todo(CSCD70) Please complete this method.
  std::vector<std::pair<llvm::Instruction *, llvm::Instruction *>>
      InstsToReplace;

  for (auto BB = F.begin(); BB != F.end(); ++BB) {
    for (auto Inst = BB->begin(); Inst != BB->end(); ++Inst) {
      if (BinaryOperator *BinaryInst = dyn_cast<BinaryOperator>(Inst)) {
        if (auto Ret = strengthReductionInner(BinaryInst); Ret.has_value()) {
          // parse the returned triple
          auto TripleResult = Ret.value();
          Value *TheVar = std::get<0>(TripleResult);
          ConstantInt *TheConst =
              dyn_cast<ConstantInt>(std::get<1>(TripleResult));
          Instruction::BinaryOps NewOperation = std::get<2>(TripleResult);

          // convert the const to the highest bit representation
          auto HighestBit = getHighestBit(TheConst->getZExtValue());
          ConstantInt *TheNewConst = ConstantInt::get(
              IntegerType::getInt32Ty(F.getContext()), HighestBit);

          // create the new inst and record it
          BinaryOperator *TheNewBinaryInst =
              BinaryOperator::Create(NewOperation, TheVar, TheNewConst);
          InstsToReplace.push_back(
              std::make_pair(BinaryInst, TheNewBinaryInst));
        }
      }
    }
  }

  for (const auto &InstToReplace : InstsToReplace) {
    auto *OldInst = InstToReplace.first;
    auto *NewInst = InstToReplace.second;
    OldInst->replaceAllUsesWith(NewInst);
    NewInst->insertBefore(OldInst);
    OldInst->eraseFromParent();
  }

  return PreservedAnalyses::none();
}
