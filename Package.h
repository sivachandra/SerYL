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
  std::vector<std::unique_ptr<Class>> Classes;
  std::vector<std::unique_ptr<Enum>> Enums;

public:

  Package(const std::string &N, Scope *PS) : Scope(PS), Name(N) {}

  const std::string &getName() const override { return Name; }
  const std::string &getFullyQualifiedName() const override { return Name; }

  bool lookupType(llvm::StringRef TypeName, Type &T) const override;

  template <typename T>
  using iterator = typename std::vector<std::unique_ptr<T>>::iterator;

  template <typename T>
  using const_iterator = typename std::vector<std::unique_ptr<T>>::const_iterator;

  template <typename T>
  iterator<T> begin(); 

  template <typename T>
  const_iterator<T> begin() const;

  template <typename T>
  iterator<T> end(); 

  template <typename T>
  const_iterator<T> end() const;
};

} // namespace seryl
} // namespace llvm

#endif // LLVM_UTILS_SERYL_PACKAGE_H
