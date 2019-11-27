#include "Unit.h"

#include "Support.h"
#include "Type.h"

#include "llvm/ADT/Twine.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/YAMLParser.h"

#include <cstdlib>
#include <memory>
#include <unordered_set>

static const char KW_Import[] = "__import__";
static const char KW_Package[] = "__package__";

static llvm::StringRef getScalarValueOrDie(llvm::yaml::Node *N,
                                           llvm::Twine Msg,
                                           llvm::seryl::YStreamErrReporter &ER) {
  if (N->getType() != llvm::yaml::Node::NK_Scalar)
    ER.printFatalError(N, Msg);
  return llvm::dyn_cast<llvm::yaml::ScalarNode>(N)->getRawValue();
}

namespace llvm {
namespace seryl {

std::unique_ptr<Unit> Unit::read(
    const std::string &Filename,
    const std::vector<std::string> &IncludeDirs) {
  llvm::SourceMgr SrcMgr;
  SrcMgr.setIncludeDirs(IncludeDirs);

  auto Buffer = MemoryBuffer::getFile(Filename);
  if (!Buffer) {
    llvm::errs() << "Unable to open " << Filename << '\n';
    std::exit(1);
  }
  MemoryBufferRef BufferRef(*Buffer.get());
  SrcMgr.AddNewSourceBuffer(std::move(Buffer.get()),
                            SMLoc::getFromPointer(nullptr));

  // This set will hold top-level names. When adding new class or enum, we will
  // error out on duplicate top-level names.
  std::unordered_set<std::string> TopLevelNames;

  llvm::yaml::Stream YStream(BufferRef, SrcMgr);
  YStreamErrReporter ER(YStream);

  for (auto IT = YStream.begin(); IT != YStream.end(); ++IT) {
    yaml::Node *TopLevelNode = IT->getRoot();
    if (TopLevelNode->getType() != yaml::Node::NK_Mapping) {
      ER.printFatalError(TopLevelNode,
                         "Top level structure is not a mapping.");
    }

    std::unique_ptr<Unit> U;
    StringRef PkgName;
    yaml::MappingNode *Map = llvm::dyn_cast<yaml::MappingNode>(TopLevelNode);
    for (llvm::yaml::KeyValueNode &N : *Map) {
      llvm::yaml::Node *KeyNode = N.getKey();
      llvm::StringRef KeyStr = getScalarValueOrDie(
          KeyNode, "Top-level key is not a string value.", ER);
      llvm::yaml::Node *ValueNode = N.getValue();

      if (!U) {
        if (KeyStr != KW_Package) {
          ER.printFatalError(
              KeyNode, llvm::StringRef("First key should be ") + KW_Package);
        }

        PkgName = getScalarValueOrDie(
            ValueNode, "Package name should be a string.", ER);
        FQNameParts PkgNameParts;
        if (!isFullyQualifiedName(PkgName, PkgNameParts))
          ER.printFatalError(ValueNode, "Invalid package name");
        U = std::make_unique<Unit>(Filename, PkgName);
        continue;
      }

      // TODO: Handle imports

      if (TopLevelNames.find(KeyStr) != TopLevelNames.end()) {
        ER.printFatalError(
            KeyNode, "A top-level entity with name " + KeyStr + " aleady exists.");
      }

      if (Enum::isEnumHeader(KeyStr)) {
        llvm::StringRef EnumName = Enum::getEnumName(KeyStr);
        if (!isValidIdentifier(EnumName))
          ER.printFatalError(KeyNode, "Invalid enum name");

        U->MainPackage->Enums.emplace_back(
            Enum::readEnum(EnumName, U->MainPackage, *ValueNode, ER));
        TopLevelNames.insert(EnumName);
      } else if (Class::isClassHeader(KeyStr)) {
        llvm::StringRef ClassName = Class::getClassName(KeyStr);
        if (!isValidIdentifier(ClassName))
          ER.printFatalError(KeyNode, "Invalid enum name");

        U->MainPackage->Classes.emplace_back(
            Class::readClass(ClassName, U->MainPackage, *ValueNode, ER));
        TopLevelNames.insert(ClassName);
      } else {
        ER.printFatalError(KeyNode, "Invalid top-level key.");
      }
    }
    return U; // We only ever have one document in the main yaml stream.
  }

  return nullptr;
}

bool Unit::lookupType(llvm::StringRef TypeName, Type &T) const {
  for (auto &P : Packages) {
    if (TypeName.startswith(P.first)) {
      return P.second->lookupType(TypeName.drop_front(P.first.size()), T);
    }
  }
  return false;
}

} // namespace seryl
} // namespace llvm
