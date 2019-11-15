#ifndef LLVM_UTILS_SERYL_SUPPORT_H
#define LLVM_UTILS_SERYL_SUPPORT_H

#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"

namespace llvm {
namespace ycd {

typedef SmallVector<std::string, 8> FQNameParts;

/// Return true if `Id` is a valid YCD identifier.
bool isValidIdentifier(llvm::StringRef Id);

/// Return true if `FQName` is a valid fully qualified name.
/// `Parts` will be populated with the parts of `FQName`.
bool isFullyQualifiedName(llvm::StringRef FQName, FQNameParts &Parts);

extern const char YCDSuffix[];

} // namespace ycd
} // namespace llvm

#endif // LLVM_UTILS_SERYL_SUPPORT_H
