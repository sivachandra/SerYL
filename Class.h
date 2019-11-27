#ifndef LLVM_UTILS_SERYL_CLASS_H
#define LLVM_UTILS_SERYL_CLASS_H

#include "Field.h"
#include "Scope.h"
#include "Support.h"
#include "Type.h"

#include "llvm/ADT/StringRef.h"

#include <memory>
#include <string>
#include <unordered_set>
#include <vector>

namespace llvm {
namespace seryl {

class Class : public Scope {

  std::string Name;
  std::string FullyQualifiedName;

  std::vector<std::unique_ptr<Field>> FieldVector;
  std::vector<std::unique_ptr<Class>> NestedClasses;
  std::vector<std::unique_ptr<Enum>> NestedEnums;

  // Map of types defined within the class to the kind of the type. These
  // consist of the nested  classes and enums defined within this class.
  // The keys of this map are local names of the types.
  std::unordered_map<std::string, Type::TypeKind> TypeNameMap;

  explicit Class(StringRef N, Scope *PS) : Scope(PS), Name(N) {
    if (Scope != nullptr) {
      FullyQualifiedName = Scope->getFullyQualifiedName() + "." + Name;
  }

  void emitFieldDumper(llvm::raw_ostream &OS, const std::string &indent);

  Class(const Class &) = delete;

public:

  static std::unique_ptr<Class> readClass(llvm::StringRef &Name,
                                          Scope *ParScope,
                                          llvm::yaml::Node &N,
                                          YStreamErrReporter &ER);

  bool lookupType(llvm::StringRef TypeName, Type &T) const override;

  const std::string &getName() const { return Name; }

  static bool isClassHeader(llvm::StringRef H);

  // The caller should check if the returned name is a valid identifier.
  static llvm::StringRef getClassName(llvm::StringRef H);


  const std::string &getFullyQualifiedName() const override {
    return FullyQualifiedName;
  }

  const Type *getType(llvm::StringRef TypeName) override;
  
  template <typename T>
  using iterator = std::vector<std::unique_ptr<Class>>::iterator;

  template <typename T>
  using const_iterator = std::vector<std::unique_ptr<Class>>::const_iterator;

  std::string &getName() const { return Name; }

  template <typename T>
  iterator<T> begin(); 

  template <typename T>
  const_iterator<T> begin() const;

  template <typename T>
  iterator<T> end(); 

  template <typename T>
  const_iterator<T> end() const;
};

template <>
Class::iterator<Field> Class::begin<Field>() {
  return Classes.begin();
}

template <>
Class::const_iterator<Field> Class::begin<Field>() const {
  return Classes.begin();
}

template <>
Class::iterator<Class> Class::begin<Class>() {
  return Classes.begin();
}

template <>
Class::const_iterator<Class> Class::begin<Class>() const {
  return Classes.begin();
}

template <>
Class::iterator<Enum> Class::begin<Enum>() {
  return Enums.begin();
}

template <>
Class::const_iterator<Enum> Class::begin<Enum>() const {
  return Enums.begin();
}

} // namespace seryl
} // namespace llvm

#endif // LLVM_UTILS_SERYL_CLASS_H
