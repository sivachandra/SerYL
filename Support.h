#ifndef LLVM_UTILS_SERYL_SUPPORT_H
#define LLVM_UTILS_SERYL_SUPPORT_H

#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/Support/SourceMgr.h"

#include <cstdlib>

namespace llvm {
namespace seryl {

typedef SmallVector<std::string, 8> FQNameParts;

/// Return true if `Id` is a valid seryl identifier.
bool isValidIdentifier(llvm::StringRef Id);

/// Return true if `FQName` is a valid fully qualified name.
/// `Parts` will be populated with the parts of `FQName`.
bool isFullyQualifiedName(llvm::StringRef FQName, FQNameParts &Parts);

extern const char YCDSuffix[];

class YStreamErrReporter {
  llvm::yaml::Stream &YStream;

public:
  explicit YStreamErrReporter(llvm::yaml::Stream &YS) : YStream(YS) {}

  void printFatalError(yaml::Node *N, const Twine &Msg) {
    YStream.printError(N, Msg);
    std::exit(1);
  }
};

} // namespace seryl
} // namespace llvm

#endif // LLVM_UTILS_SERYL_SUPPORT_H
