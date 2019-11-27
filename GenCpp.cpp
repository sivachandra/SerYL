#include "Class.h"
#include "Enum.h"
#include "Generators.h"
#include "Support.h"
#include "Unit.h"

#include "llvm/Support/raw_ostream.h"

#include <cstdlib>
#include <system_error>

namespace llvm {
namespace seryl {

void GenCpp(const Unit *U, const std::string &OutDir) {
  llvm::StringRef SuffixRef(llvm::seryl::YCDSuffix);
  std::string HdrName = OutDir + "/" + 
      std::string(llvm::StringRef(U->getFilename()).drop_back(SuffixRef.size())) +
      ".h";

  std::error_code EC;
  llvm::raw_fd_ostream OS(HdrName, EC);
  if (EC) {
    llvm::errs() << "Error opening " << HdrName << " to write: "
                 << EC.message() << '\n';
    std::exit(1);
  }

  const Package *MainPackage = U->getMainPackage();

  auto EnumIterator = MainPackage->begin<Enum>();
  auto EnumEnd = MainPackage->end<Enum>();
  for (; EnumIterator != EnumEnd; ++EnumIterator) {
    auto &E = *EnumIterator;
    OS << "enum " << E->getName() << " {\n";
    auto ItemIterator = E->begin();
    auto ItemEnd = E->end();
    for (; ItemIterator != ItemEnd; ++ItemIterator) {
      auto &Item = *ItemIterator;
      OS << "  " << Item.getName();
      if (Item.hasValue())
        OS << " = " << Item.value();
      OS << ",\n";
    }
    OS << "};\n";
  }

  auto ClassIterator = MainPackage->begin<Class>();
  auto ClassEnd = MainPackage->end<Class>();
  for (; ClassIterator != ClassEnd; ++ClassIterator) {
    auto &C = *ClassIterator;
    OS << "class " << C->getName() << " {\n";
    OS << "};\n";
  }
}

} // namespace seryl
} // namespace llvm
