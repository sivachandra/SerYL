#ifndef LLVM_UTILS_SERYL_UNIT_H
#define LLVM_UTILS_SERYL_UNIT_H

#include "Class.h"
#include "Enum.h"
#include "Package.h"
#include "Scope.h"
#include "Support.h"
#include "Type.h"

#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"

#include <memory>
#include <string>
#include <vector>

namespace llvm {
namespace seryl {

class Unit : public Scope {
  std::string InputFile;
  std::string MainPackageName;
  Package *MainPackage;

  std::unordered_map<std::string, std::unique_ptr<Package>> Packages;

  // List of imports
  std::vector<std::string> ImportedFiles;

public:
  
  explicit Unit(const std::string &InFile, StringRef &PkgName)
      : Scope(nullptr), InputFile(InFile), MainPackageName(PkgName) {
    auto Pair = Packages.emplace(PkgName, std::make_unique<Package>(PkgName, this));
    MainPackage = Pair.first->second.get();
  }

  static std::unique_ptr<Unit> read(
      const std::string &InputFile,
      const std::vector<std::string> &IncludeDirs);

  const std::string &getMainPackageName() const {
    return MainPackageName;
  }

  const Package *getMainPackage() const {
    return MainPackage;
  }

  const std::string &getFilename() const {
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

} // namespace seryl
} // namespace llvm

#endif // LLVM_UTILS_SERYL_UNIT_H
