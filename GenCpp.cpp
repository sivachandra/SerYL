#include "Class.h"
#include "Enum.h"
#include "Generators.h"
#include "Support.h"
#include "Unit.h"

#include "llvm/ADT/StringExtras.h"
#include "llvm/Support/raw_ostream.h"

#include <cstdlib>
#include <string>
#include <system_error>

static const char Indent[] = "  ";
static const char ScalarArg[] = "__val";
static const char StringArg[] = "__str";
static const char ClassArg[] = "__obj";
static const char ListArg[] = "__elem";

namespace llvm {
namespace seryl {

static std::string getFullyQualifiedCppName(const std::string &FQName) {
  if (FQName == "string") {
    return "::std::string";
  } else if (FQName == "double") {
    return "double";
  } else if (FQName == "int" || FQName == "int32") {
    return "::std::int32_t";
  } else if (FQName == "int8") {
    return "::std::int8_t";
  } else if (FQName == "int16") {
    return "::std::int16_t";
  } else if (FQName == "int64") {
    return "::std::int64_t";
  } else if (FQName == "bool") {
    return "bool";
  }

  FQNameParts Parts;
  bool isFQ = isFullyQualifiedName(llvm::StringRef(FQName), Parts);
  assert(isFQ && "Cannot get a fully qualified C++ name from an invalid name.");
  return std::string("::") + llvm::join(Parts, "::");
}

static void writeField(const Field &F, const llvm::Twine &I,
                       llvm::raw_ostream &OS) {
  bool IsOneof = F.isOneof();
  OS << I << "private:\n";
  for (auto &P : F) {
    if (IsOneof) {
      ; // TODO: Emit an enum for the oneof field.
    }
    std::string FQCppName =
        getFullyQualifiedCppName(P.second.getFullyQualifiedName());
    if (P.second.getKind() == Type::TK_List) {
      FQCppName = std::string("::std::vector<") + FQCppName + ">";
    }
    OS << I << FQCppName << " " << P.first << ";\n";
  }

  OS << I << "public:\n";
  for (auto &P : F) {
    const Type &T = P.second;
    const std::string &Name = P.first;
    std::string FQCppName = getFullyQualifiedCppName(T.getFullyQualifiedName());

    Type::TypeKind K = T.getKind();

    if (Type::isScalarType(T)) {
      OS << I << FQCppName << " get_" << Name << "() const {\n";
      OS << I + Indent << "return " << Name << ";\n";
      OS << I << "}\n";

      OS << I << "void "
         << "set_" << Name << "(" << FQCppName << " " << ScalarArg << ") {\n";
      OS << I + Indent << Name << " = " << ScalarArg << ";\n";
      OS << I << "}\n";
    } else if (K == Type::TK_String) {
      OS << I << FQCppName << " &get_" << Name << "() const {\n";
      OS << I + Indent << "return " << Name << ";\n";
      OS << I << "}\n";

      OS << I << "void "
         << "set_" << Name << "(const " << FQCppName << " &" << StringArg
         << ") {\n";
      OS << I + Indent << Name << " = " << StringArg << ";\n";
      OS << I << "}\n";
    } else if (K == Type::TK_Class) {
      OS << I << "const " << FQCppName << " &get_" << Name << "() const {\n";
      OS << I + Indent << "return " << Name << ";\n";
      OS << I << "}\n";

      OS << I << FQCppName << " &get_" << Name << "() {\n";
      OS << I + Indent << "return " << Name << ";\n";
      OS << I << "}\n";
    } else if (K == Type::TK_List) {
      OS << I << "const " << FQCppName << " &get_" << Name << "(size_t "
         << ListArg << ") const {\n";
      OS << I + Indent << "return " << Name << "[" << ListArg << "];\n";
      OS << I << "}\n";

      OS << I << FQCppName << " &get_" << Name << "(size_t " << ListArg
         << ") {\n";
      OS << I + Indent << "return " << Name << "[" << ListArg << "];\n";
      OS << I << "}\n";
    } else {
      assert(false && "Unexpected type kind of a field type.");
    }
  }
}

static void writeEnum(const Enum &E, const llvm::Twine &I, llvm::raw_ostream &OS) {
  OS << I << "enum " << E.getName() << " {\n";

  auto ItemEnd = E.end();
  for (auto ItemIterator = E.begin(); ItemIterator != ItemEnd; ++ItemIterator) {
    auto &Item = *ItemIterator;
    OS << I + Indent << Item.getName();
    if (Item.hasValue())
      OS << " = " << Item.value();
    OS << ",\n";
  }

  OS << I << "};\n";
}

static void writeClass(const Class &C, const llvm::Twine &I, llvm::raw_ostream &OS) {
  OS << I << "class " << C.getName() << " {\n";

  auto EnumEnd = C.end<Enum>();
  for (auto Iter = C.begin<Enum>(); Iter != EnumEnd; ++Iter) {
    OS << I << "public:\n";
    auto &NestedEnum = *Iter;
    writeEnum(*NestedEnum, I + Indent, OS);
    OS << '\n';
  }

  auto ClassEnd = C.end<Class>();
  for (auto Iter = C.begin<Class>(); Iter != ClassEnd; ++Iter) {
    OS << I << "public:\n";
    auto &NestedClass = *Iter;
    writeClass(*NestedClass, I + Indent, OS);
    OS << '\n';
  }

  auto FieldEnd = C.end<Field>();
  for (auto Iter = C.begin<Field>(); Iter != FieldEnd; ++Iter) {
    auto &F = *Iter;
    writeField(*F, I + Indent, OS);
    OS << '\n';
  }

  OS << I << "};\n";
} 

void GenCpp(const Unit *U, const std::string &OutDir) {
  llvm::StringRef SuffixRef(llvm::seryl::YCDSuffix);
  std::string HdrName = OutDir + "/" + 
      std::string(llvm::StringRef(U->getFilename()).drop_back(SuffixRef.size())) +
      ".h";

  std::error_code EC;
  llvm::raw_fd_ostream OS(HdrName, EC);
  if (EC) {
    llvm::errs() << "Error opening " << HdrName << " to write: "
                 << EC.message() << '\n';
    std::exit(1);
  }

  OS << "// DO NOT EDIT - Generated from " << U->getFilename() << '\n';
  OS << '\n';

  std::string HdrGuard = HdrName;
  for (char &C : HdrGuard) {
    if (C == '\\' || C == '/' || C == '.')
      C = '_';
  }
  HdrGuard = llvm::StringRef(HdrGuard).upper();

  OS << "#ifndef " << HdrGuard << '\n';
  OS << "#define " << HdrGuard << '\n';
  OS << '\n';
  OS << "#include <cstdint>\n";
  OS << "#include <string>\n";
  OS << "#include <vector>\n";

  OS << '\n';

  const Package *MainPackage = U->getMainPackage();

  FQNameParts PkgNameParts;
  bool IsFQ =
      isFullyQualifiedName(MainPackage->getFullyQualifiedName(), PkgNameParts);
  assert(IsFQ && "Package name is not fully qualified.");

  for (auto &P : PkgNameParts) {
    OS << "namespace " << P << " {\n";
  }

  OS << '\n';

  auto EnumIterator = MainPackage->begin<Enum>();
  auto EnumEnd = MainPackage->end<Enum>();
  for (; EnumIterator != EnumEnd; ++EnumIterator) {
    auto &E = *EnumIterator;
    if (!E->isComplete()) continue;

    writeEnum(*E, "", OS);
    OS << '\n';
  }

  auto ClassIterator = MainPackage->begin<Class>();
  auto ClassEnd = MainPackage->end<Class>();
  for (; ClassIterator != ClassEnd; ++ClassIterator) {
    auto &C = *ClassIterator;
    if (!C->isComplete()) continue;

    writeClass(*C, "", OS);
    OS << '\n';
  }

  for (auto &P : PkgNameParts) {
    OS << "} // namespace " << P << '\n';
  }
  OS << '\n';

  OS << "#endif //" << HdrGuard << '\n';
}

} // namespace seryl
} // namespace llvm
