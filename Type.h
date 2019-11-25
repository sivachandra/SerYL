#ifndef LLVM_UTILS_SERYL_TYPE_H
#define LLVM_UTILS_SERYL_TYPE_H

#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/Twine.h"

#include <memory>
#include <string>
#include <vector>

namespace llvm {
namespace seryl {

class Type {
public:
  enum TypeKind {
    TK_Scalar,
    TK_Class,
    TK_List,
    TK_String,
    TK_Enum
  };

private:
  const std::string FullyQualifiedName;
  const TypeKind Kind;

public:
  Type(const std::string &FQName, TypeKind K)
      : FullyQualifiedName(FQName), Kind(K) {}

  TypeKind getKind() const { return Kind; }

  void setKind(TypeKind K) { Kind = K; }

  const std::string& getFullyQualifiedName() const {
    return FullyQualifiedName;
  }

  void setFullyQualifiedName(std::string &N) { FullyQualifiedName = N; }

  static void addBuiltinTypes(std::vector<std::unique_ptr<Type>> &TypeVector);

  static bool isListType(StringRef TypeName);
};

} // namespace seryl
} // namespace llvm

#endif // LLVM_UTILS_SERYL_TYPE_H
