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

class Unit {
  std::string InputFile;
  std::string PackageName;

  // List of toplevel classes declared in the main .ycd file.
  std::vector<Class> Classes;

  // List of toplevel enums declared in the main .ycd file.
  std::vector<Enum> Enums;

  // List of types in the main .ycd file as well as in the imported files.
  std::vector<std::unique_ptr<Type>> Types;

  // List of imports
  std::vector<std::string> ImportedFiles;

  explicit Unit(const std::string &InFile, StringRef &PkgName);

public:
  
  static std::unique_ptr<Unit> read(
      const std::string &InputFile,
      const std::vector<std::string> &IncludeDirs);

  std::string &getPackageName() const {
    return PackageName;
  }

  std::string &getFilename() const {
    return InputFile;
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
