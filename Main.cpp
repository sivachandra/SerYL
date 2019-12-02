#include "Generators.h"
#include "Support.h"
#include "Unit.h"

#include "llvm/ADT/StringRef.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/raw_ostream.h"

#include <string>

static llvm::cl::opt<std::string> OutputDir(
    "dst",
    llvm::cl::desc("The full path to the output directory. "
                   "Defaults to current directory."),
    llvm::cl::value_desc("path"),
    llvm::cl::Required);

static llvm::cl::list<std::string> ImportPaths(
    "I",
    llvm::cl::desc("Paths in which files to be imported will be looked up."),
    llvm::cl::value_desc("path"));

static llvm::cl::opt<std::string> InputFile(
    llvm::cl::Positional,
    llvm::cl::desc("<YCD file>"),
    llvm::cl::Required);

static int SerYLMain() {
  llvm::StringRef InputFileRef(InputFile);

  if (!InputFileRef.endswith(llvm::seryl::YCDSuffix)) {
    llvm::errs() << "Input files should have " << llvm::seryl::YCDSuffix
                 << " suffix.\n";
    return 1;
  }

  auto U = llvm::seryl::Unit::read(InputFile, ImportPaths);
  GenCpp(U.get(), OutputDir);

  return 0;
}

int main(int argc, char* argv[]) {
  llvm::cl::ParseCommandLineOptions(argc, argv);
  return SerYLMain();
}
