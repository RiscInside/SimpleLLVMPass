#include <fstream>
#include <llvm/IR/Function.h>
#include <llvm/IR/Module.h>
#include <llvm/Pass.h>
#include <llvm/Support/raw_ostream.h>
#include <map>
#include <sstream>
#include <string>
#include <unistd.h>

namespace {

struct CountPass : public llvm::FunctionPass {
  size_t functionsCount = 0;
  size_t basicBlocksCount = 0;

  // Used for gnuplot yrange
  size_t maximumBlocksWithSameInstructionCount = 0;

  std::map<size_t, size_t> instructionCountsHistogram;

  static char ID;
  CountPass() : llvm::FunctionPass(ID) {}

  bool runOnFunction(llvm::Function &function) override {
    functionsCount++;
    // Iterate over all basic blocks
    for (auto &basicBlock : function) {
      basicBlocksCount++;
      size_t instructionsCount = 0;
      // Iterate over all instructions
      for (auto &ins : basicBlock) {
        instructionsCount++;
      }
      // Update histogram data
      if (instructionCountsHistogram.find(instructionsCount) !=
          instructionCountsHistogram.end()) {
        instructionCountsHistogram[instructionsCount]++;

        if (instructionCountsHistogram[instructionsCount] >
            maximumBlocksWithSameInstructionCount) {
          maximumBlocksWithSameInstructionCount =
              instructionCountsHistogram[instructionsCount];
        }

      } else {
        instructionCountsHistogram[instructionsCount] = 1;
      }
    }
    return false;
  }

  virtual bool doFinalization(llvm::Module &module) {
    llvm::errs() << "The program has a total of " << functionsCount
                 << " functions and " << basicBlocksCount << " basic blocks.\n";
    llvm::errs() << "Basic blocks by instructions count statistics:\n";
    for (auto const &x : instructionCountsHistogram) {
      llvm::errs() << x.first << ": " << x.second << "\n";
    }

    // Start gnuplot process
    FILE *gnuplotProcess = popen("gnuplot", "w");
    if (gnuplotProcess == nullptr) {
      llvm::errs() << "Failed to spawn gnuplot process, exiting...";
      return false;
    }
    const std::string outputFilename = module.getSourceFileName() + ".pdf";
    llvm::errs() << "Gnuplot presence detected, rendering histogram to "
                 << outputFilename << "\n";

    // Save histogram data to temporary file
    char tmpFilename[] = "/tmp/SimplePassDataXXXXXX";
    int fd = mkstemp(tmpFilename);
    if (fd < 0) {
      llvm::errs() << "Failed to create temporarory file, exiting...";
    }
    close(fd);
    std::ofstream tmpFile(tmpFilename);
    if (!tmpFile.is_open()) {
      llvm::errs() << "Failed to open temporarory file, exiting...";
    }
    for (auto const &x : instructionCountsHistogram) {
      tmpFile << x.first << " " << x.second << "\n";
    }
    tmpFile.close();

    // Create a string with gnuplot source using string stream
    std::stringstream sourceStream;

    // gnuplot code for the Histogram
    sourceStream << "set terminal pdf\n";
    sourceStream << "set output \'" << outputFilename << "\'\n";
    sourceStream << "set style data histograms\n";
    sourceStream << "set style fill solid\n";
    sourceStream << "set yrange [0:"
                 << maximumBlocksWithSameInstructionCount + 1 << "]\n";
    sourceStream << "plot \'" << tmpFilename
                 << "\' using 2:xtic(1) title \'Basic blocks by instruction "
                    "count\' linecolor \'black\'\n";

    // Send code over to gnuplot
    std::string source = sourceStream.str();
    if (fputs(source.c_str(), gnuplotProcess) < 0) {
      llvm::errs() << "Failed to send script to gnuplot, exiting...\n";
    } else {
      llvm::errs() << "Successfully rendered output to " << outputFilename
                   << ", exiting...\n";
    }
    pclose(gnuplotProcess);
    return false;
  }
};

char CountPass::ID = 0;

static llvm::RegisterPass<CountPass>
    X("count", "Count functions, basic blocks and instructions", false, false);

}; // namespace
