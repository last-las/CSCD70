#include <llvm/Passes/PassBuilder.h>
#include <llvm/Passes/PassPlugin.h>
#include <llvm/Support/raw_ostream.h>

using namespace llvm;

namespace {

class FunctionInfoPass final : public PassInfoMixin<FunctionInfoPass> {
public:
  PreservedAnalyses run([[maybe_unused]] Module &M, ModuleAnalysisManager &) {
    //    outs() << "CSCD70 Function Information Pass"
    //           << "\n";

    /// @todo(CSCD70) Please complete this method.
    for (auto F = M.begin(); F != M.end(); ++F) {
      auto Name = F->getName();
      std::string ArgSizeStr = std::to_string(F->arg_size());
      if (F->getFunctionType()->isVarArg()) {
        ArgSizeStr += "+*";
      }
      size_t DirectCallCnt = 0;
      size_t InstCnt = 0;
      for (auto BB = F->begin(); BB != F->end(); ++BB) {
        InstCnt += BB->size();
        for (auto Inst = BB->begin(); Inst != BB->end(); ++Inst) {
          if (CallInst *CI = dyn_cast<CallInst>(Inst)) {
            DirectCallCnt += 1;
          }
        }
      }
      auto BBCnt = F->size();

      outs() << "Function Name: " << Name << "\n";
      outs() << "Number of Arguments: " << ArgSizeStr << "\n";
      outs() << "Number of Calls: " << DirectCallCnt << "\n";
      outs() << "Number OF BBs: " << BBCnt << "\n";
      outs() << "Number of Instructions: " << InstCnt << "\n";
    }

    return PreservedAnalyses::all();
  }
}; // class FunctionInfoPass

} // anonymous namespace

extern "C" PassPluginLibraryInfo llvmGetPassPluginInfo() {
  return {
      .APIVersion = LLVM_PLUGIN_API_VERSION,
      .PluginName = "FunctionInfo",
      .PluginVersion = LLVM_VERSION_STRING,
      .RegisterPassBuilderCallbacks =
          [](PassBuilder &PB) {
            PB.registerPipelineParsingCallback(
                [](StringRef Name, ModulePassManager &MPM,
                   ArrayRef<PassBuilder::PipelineElement>) -> bool {
                  if (Name == "function-info") {
                    MPM.addPass(FunctionInfoPass());
                    return true;
                  }
                  return false;
                });
          } // RegisterPassBuilderCallbacks
  };        // struct PassPluginLibraryInfo
}
