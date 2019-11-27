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
    TK_Bool,
    TK_Class,
    TK_Double,
    TK_Enum,
    TK_Int,
    TK_Int8,
    TK_Int16,
    TK_Int32,
    TK_Int64,
    TK_List,
    TK_String
  };

private:
  std::string FullyQualifiedName;
  TypeKind Kind;

public:
  Type() {}

  Type(const std::string &FQName, TypeKind K)
      : FullyQualifiedName(FQName), Kind(K) {}

  TypeKind getKind() const { return Kind; }

  void setKind(TypeKind K) { Kind = K; }

  const std::string& getFullyQualifiedName() const {
    return FullyQualifiedName;
  }

  void setFullyQualifiedName(const std::string &N) { FullyQualifiedName = N; }

  static void addBuiltinTypes(std::vector<std::unique_ptr<Type>> &TypeVector);

  static bool isListType(llvm::StringRef TypeName, llvm::StringRef &ElemType);

  static bool isBuiltinType(llvm::StringRef TypeName, Type &T);

  static bool isScalarType(const Type &T);
};

} // namespace seryl
} // namespace llvm

#endif // LLVM_UTILS_SERYL_TYPE_H
