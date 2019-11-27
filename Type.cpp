#include "Type.h"
#include "Support.h"

#include "llvm/ADT/StringExtras.h"

static const char BoolType[] = "bool";
static const char IntType[] = "int";
static const char Int8Type[] = "int8";
static const char Int16Type[] = "int16";
static const char Int32Type[] = "int32";
static const char Int64Type[] = "int64";
static const char DoubleType[] = "double";
static const char StringType[] = "string";

namespace llvm {
namespace seryl {

bool Type::isListType(llvm::StringRef TypeName, llvm::StringRef &ElemType) {
  llvm::StringRef R = TypeName.trim(' ');
  if (!(R.startswith("list") && R.endswith(">"))) {
    return false;
  }

  R = R.drop_front(4).drop_back(1).trim();
  if (!R.startswith("<")) {
    return false;
  }

  R = R.drop_front(1);
  if (isValidIdentifier(R)) {
    ElemType = R;
    return true;
  }

  return false;
}

bool Type::isBuiltinType(llvm::StringRef TypeName, Type &T) {
  if (TypeName == IntType) {
    T.FullyQualifiedName = IntType;
    T.Kind = TK_Int;
    return true;
  } else if (TypeName == Int8Type) {
    T.FullyQualifiedName = Int8Type;
    T.Kind = TK_Int8;
    return true;
  } else if (TypeName == Int16Type) {
    T.FullyQualifiedName = Int16Type;
    T.Kind = TK_Int16;
    return true;
  } else if (TypeName == Int32Type) {
    T.FullyQualifiedName = Int32Type;
    T.Kind = TK_Int32;
    return true;
  } else if (TypeName == Int64Type) {
    T.FullyQualifiedName = Int64Type;
    T.Kind = TK_Int64;
    return true;
  } else if (TypeName == StringType) {
    T.FullyQualifiedName = StringType;
    T.Kind = TK_String;
    return true;
  } else if (TypeName == DoubleType) {
    T.FullyQualifiedName = DoubleType;
    T.Kind = TK_Double;
    return true;
  } else if (TypeName == BoolType) {
    T.FullyQualifiedName = BoolType;
    T.Kind = TK_Bool;
    return true;
  } else {
    return false;
  }
}

bool Type::isScalarType(const Type &T) {
  const TypeKind K = T.getKind();
  switch (K) {
  case TK_Bool:
  case TK_Double:
  case TK_Int:
  case TK_Int8:
  case TK_Int16:
  case TK_Int32:
  case TK_Int64:
  case TK_Enum:
    return true;
  default:
    return false;
  }
}

} // namespace seryl
} // namespace llvm
