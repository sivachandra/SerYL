#include "Support.h"

#include "llvm/ADT/StringExtras.h"

namespace llvm {

bool isValidYCDIdentifier(llvm::StringRef Id) {
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

bool isFullyQualifiedYCDName(llvm::StringRef FQName, llvm::YCDName &Name) {
  llvm::SmallVector<llvm::StringRef, 8> Parts;
  FQName.split(Parts, ".");
  for (llvm::StringRef P : Parts) {
    if (!isValidYCDIdentifier(P)) {
      return false;
    }
    Name.push_back(P);
  }
  return true;
}

} // namespace llvm
