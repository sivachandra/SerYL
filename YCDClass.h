#ifndef LLVM_UTILS_SERYL_YCDCLASS_H
#define LLVM_UTILS_SERYL_YCDCLASS_H

#include "llvm/ADT/StringRef.h"

#include <string>
#include <vector>

namespace llvm {

enum YCDFieldKind {
  YCD_Bool,
  YCD_Int8,
  YCD_Int16,
  YCD_Int32,
  YCD_Int64,
  YCD_Double,
  YCD_String,
  YCD_List,
  YCD_Class
};

class YCDField {
  std::string Name;
  std::string TypeName;
  YCDFieldKind TypeKind;

public:
  YCDField(const std::string &N, const std::string &T) : Name(N), TypeName(T) {}
};

class YCDClass {
  friend class YCDReader;

  std::string Name;
  std::vector<YCDField> Fields;

  explicit YCDClass(StringRef N) : Name(N) {}

  void addField(StringRef Name, StringRef TypeName) {
    Fields.emplace_back(YCDField(Name, TypeName));
  }

public:

  const std::string &getName() const { return Name; }

  void writeHeader(llvm::raw_ostream &OS) const;
  void writeImpl(llvm::raw_ostream &OS) const;
};

} // namespace llvm

#endif // LLVM_UTILS_SERYL_YCDCLASS_H
