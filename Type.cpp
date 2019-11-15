#include "Type.h"
#include "Support.h"

#include "llvm/ADT/StringExtras.h"

namespace llvm {
namespace ycd {

std::string Type::getFullyQualifiedCppName() const {
  if (Kind == TK_Scalar) {
    return Name;
  } else if (Kind == TK_String) {
    return "std::string";
  }

  StringRef FQNameRef = FullyQualifiedName;
  if (Kind == TK_List)
    FQNameRef = FQNameRef.drop_front(5).drop_back(1);

  FQNameParts Parts;
  bool B = isFullyQualifiedName(FQNameRef, Parts);
  assert(B && "The fully qualifed name of Type is not fully qualifed.");

  std::string FQName = join(Parts.begin(), Parts.end(), "::");
  if (Kind == TK_List) {
    FQName = std::string("std::vector<") + FQName + ">";
  }
  return FQName;
}

std::string Type::getInnerMostCppName() const {
    switch (Kind) {
    case TK_Scalar:
      return Name;
    case TK_String:
      return "std::string";
    case TK_Class:
      return Name;
    case TK_List:
      return std::string("std::vector<") + Name + ">";
    default:
      return "";
    }
}

void Type::addBuiltinTypes(std::vector<std::unique_ptr<Type>> &TypeVector) {
  TypeVector.emplace_back(std::make_unique<Type>("int8", "int8", TK_Scalar, false));
  TypeVector.emplace_back(std::make_unique<Type>("list<int8>", "list<int8>", TK_List, false));
  TypeVector.emplace_back(std::make_unique<Type>("int16", "int16", TK_Scalar, false));
  TypeVector.emplace_back(std::make_unique<Type>("list<int16>", "list<int16>", TK_List, false));
  TypeVector.emplace_back(std::make_unique<Type>("int32", "int32", TK_Scalar, false));
  TypeVector.emplace_back(std::make_unique<Type>("list<int32>", "list<int32>", TK_List, false));
  TypeVector.emplace_back(std::make_unique<Type>("int64", "int64", TK_Scalar, false));
  TypeVector.emplace_back(std::make_unique<Type>("list<int64>", "list<int64>", TK_List, false));
  TypeVector.emplace_back(std::make_unique<Type>("int", "int", TK_Scalar, false));
  TypeVector.emplace_back(std::make_unique<Type>("list<int>", "list<int>", TK_List, false));
  TypeVector.emplace_back(std::make_unique<Type>("double", "double", TK_Scalar, false));
  TypeVector.emplace_back(std::make_unique<Type>("list<double>", "list<double>", TK_List, false));
  TypeVector.emplace_back(std::make_unique<Type>("string", "string", TK_String, false));
  TypeVector.emplace_back(std::make_unique<Type>("list<string>", "list<string>", TK_List, false));
}

StringRef Type::isListType(StringRef TypeName) {
  StringRef R = TypeName.trim(' ');
  if (!(R.startswith("list") && R.endswith(">"))) {
    return "";
  }

  R = R.drop_front(4).drop_back(1).trim();
  if (!R.startswith("<")) {
    return "";
  }

  R = R.drop_front(1);
  if (isValidIdentifier(R)) {
    return R;
  }

  return "";
}

} // namespace ycd
} // namespace llvm
