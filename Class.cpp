#include "Class.h"

static const char ClassPrefix[] = "class(";
static size_t ClassPrefixSize = llvm::StringRef(ClassPrefix).size();
static const char ClassSuffix[] = ")";
static size_t ClassSuffixSize = llvm::StringRef(ClassSuffix).size();

namespace llvm {
namespace seryl {

std::unique_ptr<Class> Class::readClass(llvm::StringRef FQName,
                                        Scope *ParScope,
                                        llvm::yaml::Node &ClassBody,
                                        YStreamErrReporter &ER) {
  if (ClassBody.getType != llvm::yaml::Node::NK_MappingNode) {
    ER.printFatalError(&ClassBody, "Class body should be a mapping.");
  }

  llvm::yaml::MappingNode *ClassBodyAsMapping =
      llvm::dyn_cast<llvm::yaml::MappingNode>(&ClassBody);

  // This set will hold member names. When adding a member, we will error
  // out on duplicate member names.
  std::unordered_set<std::string> MemberNames;

  auto C = std::make_unique<Class>(FQName);

  for (llvm::yaml::KeyValueNode &KVNode : *ClassBodyAsMapping) {
    llvm::yaml::Node *Key = KVNode.getKey();
    llvm::yaml::Node *Value = KVNode.getValue();

    if (!Key->getType() != llvm::yaml::Node::NK_Scalar)
      ER.printFatalError(Key, "Class body keys should be scalars.");

    llvm::StringRef KeyStr =
        llvm::dyn_cast<llvm::yaml::ScalarNode>(Key)->getValue();
    if (MemberNames.find(KeyStr) != MemberNames.end()) {
      ER.printFatalError(
          Key, "Class member with name " + KeyStr + " aleady exists.");
    }

    if (Enum::isEnumHeader(KeyStr)) {
      llvm::StringRef EnumName = Enum::getEnumName(KeyStr);
      if (!isValidIdentifier(EnumName))
        ER.printFatalError(Key, "Invalid enum name");

      std::string FQEnumName = C->FullyQualifiedName + std::string(EnumName);
      C->NestedEnums.emplace_back(Enum::readEnum(FQEnumName, *Value, ER));
      C->TypeNameMap.emplace(EnumName, Type::TK_Enum);
    } else if (Class::isClassHeader(KeyStr)) {
      llvm::StringRef ClassName = Class::getClassName(KeyStr);
      if (!isValidIdentifier(ClassName))
        ER.printFatalError(Key, "Invalid class name");

      std::string FQClassName = C->FullyQualifiedName + std::string(ClassName);
      C->NestedClasses.emplace_back(Class::readClass(FQClassName, *Value, ER));
      C->TypeNameMap.emplace(ClassName, Type::TK_Class);
    } else if (Field::isOneofField(KeyStr)) {
      ; // TODO: Parse and setup oneof fields.
    } else {
      // This is a normal field of the kind:
      //   <field name> : <field type>
      if (!isValidIdentifier(KeyStr)
        ER.printFatalError(Key, "Invalid field name.");

      if (Value->getType() != llvm::yaml::Node::NK_Scalar)
        ER.printFatalError(Value, "Field type should be a simple string value.");

      llvm::StringRef TypeName =
        llvm::dyn_cast<llvm::yaml::ScalarNode>(Value)->getValue();

      llvm::StrinRef ElemTypeName;
      Type T;
      if (Type::isListType(ValStr, ElemTypeName)) {
        if (!Type::isBuiltinType(ElemTypeName, T) {
          if (!lookupType(ElemTypeName, T))
            ER.printFatalError(Value, "Unknown field type.");
        }
        T.setKind(Type::TK_List);
      } else {
        if (!isValidIdentifier(TypeName))
          ER.printFatalError(Value, "Invalid field type.");
        if (!Type::isBuiltinType(ElemTypeName, T) {
          if (!lookupType(TypeName, T))
            ER.printFatalError(Value, "Unknown field type.");
        }
      }
      Field F = std::make_unique<Field>();
      F.NameTypeMap.insert(KeyStr, T);
      FieldVector.emplace_back(std::move(F));
    }
  }

  return C;
}

bool Class::isClassHeader(llvm::StringRef H) {
  return H.starts_with(ClassPrefix) and H.ends_with(ClassSuffix);
}

llvm::StringRef Class::getClassName(llvm::StringRef H) {
  assert(Class::isClassHeader(H) &&
         "Trying to get class name from an invalid clas header");
  return H.drop_front(ClassPrefixSize).drop_back(ClassSuffixSize);
}

const Type *Class:getType(llvm::StringRef TypeName) {
  auto IT = TypeMap.find(TypeName);
  if (IT == TypeMap.end()) {
    return nullptr;
  }
  return IT->second;
}

bool Class::lookupType(llvm::StringRef TypeName, Type &T) {
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

/*
void Class::writeCppDefinition(raw_ostream &OS) const {
  OS << "class " << Name << "{\n";
  printf("Writing class %s\n", Name.data());
  for (auto &F : Fields) {
    const Type *T = F.FieldType;
    auto TypeKind = T->getKind();
    std::string TypeName = T->getFullyQualifiedCppName();
    if (TypeKind == Type::TK_Scalar) {
      OS << "public:\n";
      OS << "  " << TypeName << " get_" << F.Name << "() const {\n";
      OS << "    return _" << F.Name << ";\n";
      OS << "  }\n";
      OS << "  void set_" << F.Name << "(" << TypeName << " " << F.Name << ") {\n";
      OS << "    _" << F.Name << " = " << F.Name << ";\n";
      OS << "  }\n";
      OS << "private:\n";
      OS << "  " << TypeName << " " << "_" << F.Name << ";\n";
    } else {
      OS << "public:\n";
      OS << "  const " << TypeName << " &get_" << F.Name << "() const {\n";
      OS << "    return _" << F.Name << ";\n";
      OS << "  }\n";
      OS << "  " << TypeName << " &get_" << F.Name << "() {\n";
      OS << "    return _" << F.Name << ";\n";
      OS << "  }\n";
      OS << "private:\n";
      OS << "  " << TypeName << " " << "_" << F.Name << ";\n";
    }
  }

  OS << "public:\n";
  OS << "  void dump(llvm::raw_ostream &OS) {\n";
  for (auto &F : Fields) {
    auto TypeKind = F.FieldType->getKind();
    if (TypeKind == Type::TK_Scalar || TypeKind == Type::TK_String) {
      OS << "    OS << \"" << F.Name << ": \" << " << "_" << F.Name << " << \"\\n\";" << "\n";
    } else if (TypeKind == Type::TK_List) {
      OS << "    OS << \"" << F.Name << ":\" << \"\\n\"" << "\n";
      OS << "    for (auto &Val : _" << F.Name << ") {\n";
      OS << "      OS << \"-\" << Val << \"\\n\"" <<  "\n";
      OS << "    }\n";
    }
    
  }
  OS << "  }\n";
  OS << "};\n";
}
*/
} // namespace seryl
} // namespace llvm
