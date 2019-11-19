#ifndef LLVM_UTILS_SERYL_CLASS_H
#define LLVM_UTILS_SERYL_CLASS_H

#include "Support.h"
#include "Type.h"

#include "llvm/ADT/StringRef.h"

#include <string>
#include <vector>

namespace llvm {
namespace ycd {

class Field {
private:
  friend class Class;

  std::string Name;
  const Type *FieldType;

public:
  Field(const std::string &N, const Type *T) : Name(N), FieldType(T) {}

  const std::string &getName() const { return Name; }

  const Type *getType() const { return FieldType; }
};

class Class {
  friend class Loader;
  friend class Unit;

  std::string Name;
  std::vector<Field> Fields;

  explicit Class(StringRef N) : Name(N) {}

  void addField(StringRef Name, const Type *Type) {
    Fields.emplace_back(Field(Name, Type));
  }

  void emitFieldDumper(llvm::raw_ostream &OS, const std::string &indent);
public:

  void writeCppDefinition(llvm::raw_ostream &OS) const;
};

} // namespace ycd
} // namespace llvm

#endif // LLVM_UTILS_SERYL_CLASS_H
