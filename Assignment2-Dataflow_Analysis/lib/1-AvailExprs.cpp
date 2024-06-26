#include "DFA.h"

using namespace llvm;

AnalysisKey AvailExprs::Key;

bool AvailExprs::transferFunc(const Instruction &Inst, const DomainVal_t &IDV,
                              DomainVal_t &ODV) {

  /// @todo(CSCD70) Please complete this method.
  // todo: seems like we can optimize it
  ODV = IDV;

  if (const llvm::BinaryOperator *BinaryOp =
          dyn_cast<llvm::BinaryOperator>(&Inst)) {
    auto CurExpr = dfa::Expression(*BinaryOp);

    for (const auto &[EachExpr, DomainId] : DomainIdMap) {
      // kill expressions of which operator has been reassigned
      if (EachExpr.contain(BinaryOp)) {
        ODV[DomainId] = dfa::Bool{.Value = false};
      }

      // generate the current expression
      if (EachExpr == CurExpr) {
        ODV[DomainId] = dfa::Bool{.Value = true};
      }
    }
  }

  for (uint64_t I = 0; I < IDV.size(); I++) {
    if (IDV[I].Value != ODV[I].Value) {
      return true;
    }
  }

  return false;
}
