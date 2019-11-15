#include "YCDUnit.h"

#include "llvm/Support/CommandLine.h"

#include <string>

namespace {

llvm::cl::opt<std::string> DstDir(
    "dst",
    llvm::cl::desc("The directory in which to write the output. "
                   "Defaults to current directory."),
    llvm::cl::value_desc("<path>"));
llvm::cl::list<std::string> ImportPaths(
    "I",
    llvm::cl::desc("Paths in which files to be imported will be looked up."),
    llvm::cl::value_desc("<path>"));
llvm::cl::opt<std::string> InputFile(
    llvm::cl::Positional,
    llvm::cl::desc("The input *.ycd file."),
    llvm::cl::value_desc("<input file>"),
    llvm::cl::Required);

} // anonymous namespace

namespace llvm {

int SerYlMain() {
  YCDUnit::load(InputFile, ImportPaths);
  return 0;
}

} // namespace llvm

int main(int argc, char* argv[]) {
  llvm::cl::ParseCommandLineOptions(argc, argv);
  return llvm::SerYlMain();
}
