#ifndef LLVM_UTILS_SERYL_TYPE_H
#define LLVM_UTILS_SERYL_TYPE_H

#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/Twine.h"

#include <memory>
#include <string>
#include <vector>

namespace llvm {
namespace ycd {

class Type {
public:
  enum TypeKind {
    TK_Scalar,
    TK_Class,
    TK_List,
    TK_String
  };

private:
  std::string Name;
  std::string FullyQualifiedName;
  TypeKind Kind;
  bool DefinedInMainFile;

public:
  Type(const std::string &N, const std::string &FQName, TypeKind K,
       bool InMainFile)
      : Name(N), FullyQualifiedName(FQName), Kind(K),
        DefinedInMainFile(InMainFile) {}

  TypeKind getKind() const { return Kind; }

  /// Return true if this type is define in the main file.
  bool isDefinedInMainFile() const { return DefinedInMainFile; }

  const std::string& getFullyQualifiedName() const {
    return FullyQualifiedName;
  }

  std::string getFullyQualifiedCppName() const;

  std::string getInnerMostCppName() const;

  static void addBuiltinTypes(std::vector<std::unique_ptr<Type>> &TypeVector);

  static StringRef isListType(StringRef TypeName);
};

} // namespace ycd
} // namespace llvm

#endif // LLVM_UTILS_SERYL_TYPE_H
