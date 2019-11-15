#ifndef LLVM_UTILS_SERYL_YCDCLASS_H
#define LLVM_UTILS_SERYL_YCDCLASS_H

#include "Support.h"

#include "llvm/ADT/StringRef.h"

#include <string>
#include <vector>

namespace llvm {


class YCDField {
public:
  enum FieldKind {
    FK_Scalar,
    FK_Class,
    FK_List
  };

private:
  friend class YCDUnit;

  // Field names and simple string as they are never fully qualified.
  std::string Name;

  // Type names are fully qualified unless they are from the main file.
  std::string TypeName;

  FieldKind TypeKind;

public:
  YCDField(const std::string &N, const std::string &T) : Name(N), TypeName(T) {}
};

class YCDClass {
  friend class YCDLoader;
  friend class YCDUnit;

  std::string Name;
  std::vector<YCDField> Fields;

  explicit YCDClass(StringRef N) : Name(N) {}

  void addField(StringRef Name, StringRef TypeName) {
    Fields.emplace_back(YCDField(Name, TypeName));
  }

public:

  void writeCppDefinition(llvm::raw_ostream &OS) const;
};

} // namespace llvm

#endif // LLVM_UTILS_SERYL_YCDCLASS_H
