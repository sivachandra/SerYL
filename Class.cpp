#include "Class.h"

namespace llvm {
namespace ycd {

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
  OS << "};\n";
}

} // namespace ycd
} // namespace llvm
