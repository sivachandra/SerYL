#include "Class.h"
#include "Enum.h"
#include "Package.h"
#include "Type.h"

namespace llvm {
namespace seryl {

bool Package::lookupType(llvm::StringRef TypeName, Type &T) const {
  FQNameParts Parts;
  bool IsFQ = isFullyQualifiedName(TypeName, Parts);

  for (auto &C : Classes) {
    const std::string &ClassName = C->getName();
    if (ClassName == TypeName) {
      T.setFullyQualifiedName(C->getFullyQualifiedName());
      T.setKind(Type::TK_Class);
      return true;
    }
    if (IsFQ && ClassName == Parts[0]) {
      Type NT;
      llvm::ArrayRef<std::string> PartsRef(Parts);
      PartsRef = PartsRef.drop_front();
      std::string InnerName;
      for (const std::string &P : PartsRef) {
        InnerName += P;
      }
      if (C->lookupType(InnerName, NT)) {
        return true;
      }
    }
  }

  for (auto &E : Enums) {
    if (E->getName() == TypeName) {
      T.setFullyQualifiedName(E->getFullyQualifiedName());
      T.setKind(Type::TK_Enum);
      return true;
    }
  }

  return getParentScope()->lookupType(TypeName, T);
}

template <>
Class::iterator<Class> Package::begin<Class>() {
  return Classes.begin();
}

template <>
Class::const_iterator<Class> Package::begin<Class>() const {
  return Classes.begin();
}

template <>
Class::const_iterator<Class> Package::end<Class>() const {
  return Classes.end();
}

template <>
Class::iterator<Enum> Package::begin<Enum>() {
  return Enums.begin();
}

template <>
Class::const_iterator<Enum> Package::begin<Enum>() const {
  return Enums.begin();
}

template <>
Class::const_iterator<Enum> Package::end<Enum>() const {
  return Enums.end();
}

} // namespace seryl
} // namespace llvm
