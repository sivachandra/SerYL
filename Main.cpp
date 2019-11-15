#include "Support.h"
#include "Unit.h"

#include "llvm/ADT/StringRef.h"
#include "llvm/Support/CommandLine.h"
#include "llvm/Support/raw_ostream.h"

#include <string>

namespace {

llvm::cl::opt<std::string> OutputDir(
    "dst",
    llvm::cl::desc("The full path to the output directory. "
                   "Defaults to current directory."),
    llvm::cl::value_desc("path"),
    llvm::cl::Required);

llvm::cl::list<std::string> ImportPaths(
    "I",
    llvm::cl::desc("Paths in which files to be imported will be looked up."),
    llvm::cl::value_desc("path"));

llvm::cl::opt<std::string> InputFile(
    llvm::cl::Positional,
    llvm::cl::desc("<YCD file>"),
    llvm::cl::Required);

int SerYLMain() {
  llvm::StringRef InputFileRef(InputFile);

  if (!InputFileRef.endswith(llvm::ycd::YCDSuffix)) {
    llvm::errs() << "Input files should have " << llvm::ycd::YCDSuffix
                 << " suffix.\n";
    return 1;
  }

  auto U = llvm::ycd::Unit::load(InputFile, ImportPaths);

  llvm::StringRef SuffixRef(llvm::ycd::YCDSuffix);
  std::string Basename = OutputDir + "/" + 
      std::string(InputFileRef.drop_back(SuffixRef.size()));
  return U->writeCpp(Basename);
}

} // anonymous namespace

int main(int argc, char* argv[]) {
  llvm::cl::ParseCommandLineOptions(argc, argv);
  return SerYLMain();
}
