#include "Class.h"
#include "Enum.h"
#include "Generators.h"
#include "Support.h"
#include "Unit.h"

#include "llvm/Support/raw_ostream.h"

#include <cstdlib>
#include <system_error>

static const char[] Indent = "  ";

namespace llvm {
namespace seryl {

static void writeEnum(const Enum &E, const llvm::Twine &I, llvm::raw_ostream &OS) {
  OS << I << "enum " << E.getName() << " {\n";

  auto ItemEnd = E.end();
  for (auto ItemIterator = E.begin(); ItemIterator != ItemEnd; ++ItemIterator) {
    auto &Item = *ItemIterator;
    OS << I + Indent << Item.getName();
    if (Item.hasValue())
      OS << " = " << Item.value();
    OS << ",\n";
  }

  OS << I << "};\n";
}

static void writeClass(const Class &C, const llvm::Twine &I, llvm::raw_ostream &OS) {
  OS << I << "class " << C.getName() << " {\n";

  auto EnumEnd = C.end<Enum>();
  for (auto EnumIterator = C.begin<Enum>(); EnumIterator != EnumEnd; ++EnumIterator) {
    auto &E = *EnumIterator;
    writeEnum(*E, I + Indent, OS);
  }

  auto ClassEnd = C.end<Class>();
  for (auto ClassIterator = C.begin<Class>(); ClassIterator != ClassEnd; ++ClassIterator) {
    OS << "public:\n";
    auto &E = *EnumIterator;
    writeEnum(*E, I + Indent, OS);
  }

  OS << I << "};\n";
} 

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
    if (!E->isComplete()) continue;

    writeEnum(*E, "", OS);
  }

  auto ClassIterator = MainPackage->begin<Class>();
  auto ClassEnd = MainPackage->end<Class>();
  for (; ClassIterator != ClassEnd; ++ClassIterator) {
    auto &C = *ClassIterator;
    if (!C->isComplete()) continue;

    writeClass(*C, "", OS);
  }
}

} // namespace seryl
} // namespace llvm
