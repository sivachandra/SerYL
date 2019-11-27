#include "Unit.h"

#include "Loader.h"
#include "Support.h"
#include "Type.h"

#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/YAMLParser.h"

#include <cstdlib>

namespace llvm {
namespace ycd {

Unit::Unit(const std::string &InFile, StringRef &PkgName)
    : InputFile(InFile), PackageName(PkgName) {
  Type::addBuiltinTypes(Types);
}

std::unique_ptr<Unit> Unit::read(
    const std::string &Filename,
    const std::vector<std::string> &IncludeDirs) {
  Loader Loader;
  return Loader.load(Filename, IncludeDirs);
}

bool Unit::lookupType(llvm::StringRef TypeName, Type &T) {
  for (auto &P : Packages) {
    if (TypeName.starts_with(P.first)) {
      return P.second->lookupType(TypeName.drop_front(P.first.size()), T);
    }
  }
  return false;
}


} // namespace ycd
} // namespace llvm
