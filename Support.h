#ifndef LLVM_UTILS_SERYL_SUPPORT_H
#define LLVM_UTILS_SERYL_SUPPORT_H

#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"

namespace llvm {

typedef SmallVector<std::string, 8> YCDName;

/// Return true if `Id` is a valid YCD identifier.
bool isValidYCDIdentifier(llvm::StringRef Id);

/// Return true if `FQName` is a valid fully qualified name.
/// `Name` will be populated with the parts of `FQName`.
bool isFullyQualifiedYCDName(llvm::StringRef FQName, llvm::YCDName &Name);

} // namespace llvm

#endif // LLVM_UTILS_SERYL_SUPPORT_H
