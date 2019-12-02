#include "Class.h"

#include "Enum.h"

static const char ClassPrefix[] = "class(";
static size_t ClassPrefixSize = llvm::StringRef(ClassPrefix).size();
static const char ClassSuffix[] = ")";
static size_t ClassSuffixSize = llvm::StringRef(ClassSuffix).size();

namespace llvm {
namespace seryl {

std::unique_ptr<Class> Class::readClass(llvm::StringRef ClassName,
                                        Scope *ParScope,
                                        llvm::yaml::Node &ClassBody,
                                        YStreamErrReporter &ER) {
  if (ClassBody.getType() != llvm::yaml::Node::NK_Mapping) {
    ER.printFatalError(&ClassBody, "Class body should be a mapping.");
  }

  llvm::yaml::MappingNode *ClassBodyAsMapping =
      llvm::dyn_cast<llvm::yaml::MappingNode>(&ClassBody);

  // This set will hold member names. When adding a member, we will error
  // out on duplicate member names.
  std::unordered_set<std::string> MemberNames;

  auto C = std::make_unique<Class>(ClassName, ParScope);

  for (llvm::yaml::KeyValueNode &KVNode : *ClassBodyAsMapping) {
    llvm::yaml::Node *Key = KVNode.getKey();
    llvm::yaml::Node *Value = KVNode.getValue();

    if (Key->getType() != llvm::yaml::Node::NK_Scalar)
      ER.printFatalError(Key, "Class body keys should be scalars.");

    llvm::StringRef KeyStr =
        llvm::dyn_cast<llvm::yaml::ScalarNode>(Key)->getRawValue();
    if (MemberNames.find(KeyStr) != MemberNames.end()) {
      ER.printFatalError(
          Key, "Class member with name " + KeyStr + " aleady exists.");
    }

    if (Enum::isEnumHeader(KeyStr)) {
      llvm::StringRef EnumName = Enum::getEnumName(KeyStr);
      if (!isValidIdentifier(EnumName))
        ER.printFatalError(Key, "Invalid enum name");

      C->NestedEnums.emplace_back(Enum::readEnum(EnumName, C.get(), *Value, ER));
      MemberNames.insert(EnumName);
    } else if (Class::isClassHeader(KeyStr)) {
      llvm::StringRef NestedClassName = Class::getClassName(KeyStr);
      if (!isValidIdentifier(NestedClassName))
        ER.printFatalError(Key, "Invalid class name");

      C->NestedClasses.emplace_back(
          Class::readClass(NestedClassName, C.get(), *Value, ER));
      MemberNames.insert(NestedClassName);
    } else if (Field::isOneofField(KeyStr)) {
      ; // TODO: Parse and setup oneof fields.
    } else {
      // This is a normal field of the kind:
      //   <field name> : <field type>
      if (!isValidIdentifier(KeyStr))
        ER.printFatalError(Key, "Invalid field name.");

      if (Value->getType() != llvm::yaml::Node::NK_Scalar)
        ER.printFatalError(Value, "Field type should be a simple string value.");

      llvm::StringRef TypeName =
        llvm::dyn_cast<llvm::yaml::ScalarNode>(Value)->getRawValue();

      llvm::StringRef ElemTypeName;
      Type T;
      if (Type::isListType(TypeName, ElemTypeName)) {
        if (!Type::isBuiltinType(ElemTypeName, T)) {
          if (!C->lookupType(ElemTypeName, T))
            ER.printFatalError(Value, "Unknown field type.");
        }
        T.setKind(Type::TK_List);
      } else {
        if (!isValidIdentifier(TypeName))
          ER.printFatalError(Value, "Invalid field type.");
        if (!Type::isBuiltinType(TypeName, T)) {
          if (!C->lookupType(TypeName, T))
            ER.printFatalError(Value, "Unknown field type.");
        }
      }
      auto F = std::make_unique<Field>();
      F->FieldMap.insert({KeyStr, T});
      C->Fields.emplace_back(std::move(F));
    }
  }

  C->Complete = true;
  return C;
}

bool Class::isClassHeader(llvm::StringRef H) {
  return H.startswith(ClassPrefix) and H.endswith(ClassSuffix);
}

llvm::StringRef Class::getClassName(llvm::StringRef H) {
  assert(Class::isClassHeader(H) &&
         "Trying to get class name from an invalid clas header");
  return H.drop_front(ClassPrefixSize).drop_back(ClassSuffixSize);
}

bool Class::lookupType(llvm::StringRef TypeName, Type &T) const {
  FQNameParts Parts;
  bool IsFQ = isFullyQualifiedName(TypeName, Parts);

  // Within this class, the types are classes and enums.
  for (auto &NC : NestedClasses) {
    const std::string &NCName = NC->getName();
    if (NCName == TypeName) {
      T.setFullyQualifiedName(NC->getFullyQualifiedName());
      T.setKind(Type::TK_Class);
      return true;
    }
    if (IsFQ && NCName == Parts[0]) {
      Type NT;
      llvm::ArrayRef<std::string> PartsRef(Parts);
      PartsRef = PartsRef.drop_front();
      std::string InnerName;
      for (const std::string &P : PartsRef) {
        InnerName += P;
      }
      if (NC->lookupType(InnerName, NT)) {
        return true;
      }
    }
  }

  for (auto &NE : NestedEnums) {
    if (NE->getName() == TypeName) {
      T.setFullyQualifiedName(NE->getFullyQualifiedName());
      T.setKind(Type::TK_Enum);
      return true;
    }
  }

  return getParentScope()->lookupType(TypeName, T);
}

template <>
Class::iterator<Field> Class::begin<Field>() {
  return Fields.begin();
}

template <>
Class::const_iterator<Field> Class::begin<Field>() const {
  return Fields.begin();
}

template <> Class::iterator<Field> Class::end<Field>() { return Fields.end(); }

template <> Class::const_iterator<Field> Class::end<Field>() const {
  return Fields.end();
}

template <>
Class::iterator<Class> Class::begin<Class>() {
  return NestedClasses.begin();
}

template <>
Class::const_iterator<Class> Class::begin<Class>() const {
  return NestedClasses.begin();
}

template <>
Class::iterator<Class> Class::end<Class>() {
  return NestedClasses.end();
}

template <>
Class::const_iterator<Class> Class::end<Class>() const {
  return NestedClasses.end();
}

template <>
Class::iterator<Enum> Class::begin<Enum>() {
  return NestedEnums.begin();
}

template <>
Class::const_iterator<Enum> Class::begin<Enum>() const {
  return NestedEnums.begin();
}

template <>
Class::iterator<Enum> Class::end<Enum>() {
  return NestedEnums.end();
}

template <>
Class::const_iterator<Enum> Class::end<Enum>() const {
  return NestedEnums.end();
}

} // namespace seryl
} // namespace llvm
