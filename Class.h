#ifndef LLVM_UTILS_SERYL_CLASS_H
#define LLVM_UTILS_SERYL_CLASS_H

#include "Field.h"
#include "Scope.h"
#include "Support.h"
#include "Type.h"

#include "llvm/ADT/StringRef.h"
#include "llvm/Support/YAMLParser.h"

#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

namespace llvm {
namespace seryl {

class Enum;

class Class : public Scope {

  std::string Name;
  std::string FullyQualifiedName;

  std::vector<std::unique_ptr<Field>> Fields;
  std::vector<std::unique_ptr<Class>> NestedClasses;
  std::vector<std::unique_ptr<Enum>> NestedEnums;
  bool Complete;

  void emitFieldDumper(llvm::raw_ostream &OS, const std::string &indent);

  Class(const Class &) = delete;

public:

  Class(StringRef N, Scope *PS) : Scope(PS), Name(N), Complete(false) {
    assert(PS != nullptr &&
           "A class definition should always have a parent scope.");
    FullyQualifiedName = PS->getFullyQualifiedName() + "." + Name;
  }

  static std::unique_ptr<Class> readClass(llvm::StringRef N,
                                          Scope *ParScope,
                                          llvm::yaml::Node &Body,
                                          YStreamErrReporter &ER);

  bool lookupType(llvm::StringRef TypeName, Type &T) const override;

  const std::string &getName() const override { return Name; }

  bool isComplete() const { return Complete; }

  static bool isClassHeader(llvm::StringRef H);

  // The caller should check if the returned name is a valid identifier.
  static llvm::StringRef getClassName(llvm::StringRef H);


  const std::string &getFullyQualifiedName() const override {
    return FullyQualifiedName;
  }

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

#endif // LLVM_UTILS_SERYL_CLASS_H
