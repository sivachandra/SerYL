#ifndef LLVM_UTILS_SERYL_UNIT_H
#define LLVM_UTILS_SERYL_UNIT_H

#include "Support.h"
#include "Class.h"
#include "Type.h"

#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"

#include <memory>
#include <string>
#include <vector>

namespace llvm {
namespace ycd {

class Unit {
  friend class Loader;

  std::string InputFile;
  std::string PackageName;

  // List of classes declared in the main .ycd file.
  std::vector<Class> Classes;

  // List of types in the main .ycd file as well as in the imported files.
  std::vector<std::unique_ptr<Type>> Types;

  // List of imports
  std::vector<std::string> ImportedFiles;

  explicit Unit(const std::string &InFile, StringRef &PkgName);

public:
  
  static std::unique_ptr<Unit> load(
      const std::string &InputFile,
      const std::vector<std::string> &IncludeDirs);

  int writeCpp(const std::string &Basename) const;
};

} // namespace ycd
} // namespace llvm

#endif // LLVM_UTILS_SERYL_UNIT_H
