#pragma once // NOLINT(llvm-header-guard)

#include <llvm/IR/InstVisitor.h>
#include <llvm/IR/Instruction.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Value.h>
#include <llvm/Support/raw_ostream.h>

#include "Base.h"
#include "Utility.h"

namespace dfa {

struct Expression final : DomainBase<Expression> {
  const unsigned Opcode;
  const llvm::Value *const LHS = nullptr, *const RHS = nullptr;
  Expression(const llvm::BinaryOperator &BinaryOp)
      : Opcode(BinaryOp.getOpcode()), LHS(BinaryOp.getOperand(0)),
        RHS(BinaryOp.getOperand(1)) {}
  Expression(const unsigned Opcode, const llvm::Value *const LHS,
             const llvm::Value *const RHS)
      : Opcode(Opcode), LHS(LHS), RHS(RHS) {}

  bool operator==(const Expression &Other) const final {

    /// @todo(CSCD70) Please complete this method.
    if (this->Opcode == Other.Opcode) {
      if (this->LHS == Other.LHS && this->RHS == Other.RHS) {
        return true;
      }

      if (llvm::Instruction::isCommutative(this->Opcode) &&
          this->LHS == Other.RHS && this->RHS == Other.LHS) {
        return true;
      }
    }

    return false;
  }

  bool contain(const llvm::Value *const Val) const final {

    /// @todo(CSCD70) Please complete this method.
    if (this->LHS == Val || this->RHS == Val) {
      return true;
    }

    return false;
  }
  Expression replaceValueWith(const llvm::Value *const SrcVal,
                              const llvm::Value *const DstVal) const final {

    /// @todo(CSCD70) Please complete this method.
    bool ContainValue = false;
    const llvm::Value *NewLHS = this->LHS;
    const llvm::Value *NewRHS = this->RHS;
    if (this->LHS == SrcVal) {
      ContainValue = true;
      NewLHS = DstVal;
    }
    if (this->RHS == SrcVal) {
      ContainValue = true;
      NewRHS = DstVal;
    }
    if (ContainValue) {
      return Expression(this->Opcode, NewLHS, NewRHS);
    }

    return *this;
  }

  using DomainBase<Expression>::DomainIdMap_t;
  using DomainBase<Expression>::DomainVector_t;

  struct Initializer : public llvm::InstVisitor<Initializer> {
    DomainIdMap_t &DomainIdMap;
    DomainVector_t &DomainVector;
    explicit Initializer(DomainIdMap_t &DomainIdMap,
                         DomainVector_t &DomainVector)
        : DomainIdMap(DomainIdMap), DomainVector(DomainVector) {}
    void visitBinaryOperator(llvm::BinaryOperator &);
  };
};

} // namespace dfa

llvm::raw_ostream &operator<<(llvm::raw_ostream &, const dfa::Expression &);

namespace std {

template <> struct hash<::dfa::Expression> {
  size_t operator()(const dfa::Expression &Expr) const {
    size_t HashVal = 0;

    /// @todo(CSCD70) Please complete this method.
    std::hash<const llvm::Value *> Hasher;

    hashCombine(&HashVal, Expr.Opcode, Hasher(Expr.LHS) ^ Hasher(Expr.RHS));

    return HashVal;
  }
};

} // namespace std
