#ifndef LLMV_UTILS_SERYL_SCOPE_H
#define LLVM_UTILS_SERYL_SCOPE_H

namespace llvm {
namespace seryl {

// Scope is any scope within a .ycd file where are new type can be defined.
// There are only two such scopes: package scope and class scope.
class Scope {
  Scope *ParentScope;

public:
  explicit Scope(Scope *P) : ParentScope(P) {}

  virtual bool lookupType(llvm::StringRef TypeName, Type &T) const = 0;
  virtual const std::string &getFullyQualifiedName() const = 0;
  virtual const std::string &getName() const = 0;

  const Scope *getParentScope() const {
    return ParentScope;
  }  
};

} // namespace seryl
} // namespace llvma

#endif // LLVM_UTILS_SERYL_SCOPE_H
