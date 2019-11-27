#ifndef LLVM_UTILS_SERYL_PACKAGE_H
#define LLVM_UTILS_SERYL_PACKAGE_H

#include "Scope.h"

#include <memory>
#include <string>

namespace llvm {
namespace seryl {

class Class;
class Enum;
class Type;

class Package : public Scope {
  friend class Unit;

  std::string Name;
  std::unique_ptr<Class> Classes;
  std::unique_ptr<Enum> Enum;

  Package(const std::string &N, Scope *PS) : Scope(PS), Name(N) {}

public:

  const std::string &getName() const override { return Name; }
  const std::string &getFullyQualifiedName() const override { return Name; }

  bool lookupType(llvm::StringRef TypeName, Type &T) const override;
  
};

} // namespace seryl
} // namespace llvm

#endif // LLVM_UTILS_SERYL_PACKAGE_H
