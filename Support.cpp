#include "Support.h"

#include "llvm/ADT/StringExtras.h"

namespace llvm {
namespace seryl {

const char YCDSuffix[] = ".ycd";

bool isValidIdentifier(llvm::StringRef Id) {
  assert(Id.size() > 0);

  for (size_t i = 0; i < Id.size(); ++i) {
    char c = Id[i];
    if (c == '_' || llvm::isAlpha(c)) {
      continue;
    }
    if (i > 0 && llvm::isDigit(c)) {
      continue;
    }
    return false;
  }

  return true;
}

bool isFullyQualifiedName(llvm::StringRef FQName, FQNameParts &Parts) {
  llvm::SmallVector<llvm::StringRef, 8> RefParts;
  FQName.split(RefParts, ".");
  for (llvm::StringRef P : RefParts) {
    if (!isValidIdentifier(P)) {
      return false;
    }
    Parts.push_back(P);
  }
  return true;
}

} // namespace seryl
} // namespace llvm
