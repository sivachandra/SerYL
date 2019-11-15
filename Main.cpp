#include "YCDUnit.h"

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

const char YCDSuffix[] = ".ycd";

} // anonymous namespace

namespace llvm {

static int SerYLMain() {
  StringRef InputFileRef(InputFile);

  if (!InputFileRef.endswith(YCDSuffix)) {
    errs() << "Input files should have " << YCDSuffix << " suffix.\n";
    return 1;
  }

  auto Unit = YCDUnit::load(InputFile, ImportPaths);

  StringRef SuffixRef(YCDSuffix);
  std::string Basename = OutputDir + "/" + 
      std::string(InputFileRef.drop_back(SuffixRef.size()));
  return Unit->writeCpp(Basename);
}

} // namespace llvm

int main(int argc, char* argv[]) {
  llvm::cl::ParseCommandLineOptions(argc, argv);
  return llvm::SerYLMain();
}
