#ifndef LLVM_UTILS_SERYL_UNIT_H
#define LLVM_UTILS_SERYL_UNIT_H

#include "Class.h"
#include "Support.h"
#include "Type.h"

#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"

#include <memory>
#include <string>
#include <vector>

namespace llvm {
namespace ycd {

class Unit : public Scope {
  std::string InputFile;
  std::string MainPackageName;

  std::unordered_map<std::string, std::unique_ptr<Package>> Packages;

  // List of toplevel classes declared in the main .ycd file.
  std::vector<Class> Classes;

  // List of toplevel enums declared in the main .ycd file.
  std::vector<Enum> Enums;

  // List of types in the main .ycd file as well as in the imported files.
  std::vector<std::unique_ptr<Type>> Types;

  // List of imports
  std::vector<std::string> ImportedFiles;

  explicit Unit(const std::string &InFile, StringRef &PkgName)
      : Scope(nullptr), InputFile(InFile), MainPackageName(PkgName) {}

public:
  
  static std::unique_ptr<Unit> read(
      const std::string &InputFile,
      const std::vector<std::string> &IncludeDirs);

  std::string &getMainPackageName() const {
    return MainPackageName;
  }

  std::string &getFilename() const {
    return InputFile;
  }

  bool lookupType(llvm::StringRef TypeName, Type &T) const override;

  const std::string &getName() const override {
    return InputFile;
  }

  const std::string &getFullyQualifiedName() const override {
    return MainPackageName;
  }
};

template <>
Unit::iterator<Class> Unit::begin<Class>() {
  return Classes.begin();
}

template <>
Unit::const_iterator<Class> Unit::begin<Class>() const {
  return Classes.begin();
}

template <>
Unit::iterator<Enum> Unit::begin<Enum>() {
  return Enums.begin();
}

template <>
Unit::const_iterator<Enum> Unit::begin<Enum>() const {
  return Enums.begin();
}

} // namespace ycd
} // namespace llvm

#endif // LLVM_UTILS_SERYL_UNIT_H
