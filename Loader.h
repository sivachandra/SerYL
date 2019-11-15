#ifndef LLVM_UTILS_SERYL__LOADER_H
#define LLVM_UTILS_SERYL__LOADER_H

#include "Support.h"

#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/Twine.h"
#include "llvm/Support/SourceMgr.h"

#include <memory>
#include <unordered_map>

namespace llvm {

namespace yaml {

class Node;
class Stream;

} // namespace yaml

namespace ycd {

class Class;
class Type;
class Unit;

class Loader {
  std::unique_ptr<yaml::Stream> YStream;
  std::unique_ptr<SourceMgr> SrcMgr;
  std::unordered_map<std::string, Type *> TypeMap;

  StringRef readPackageName(yaml::Node *Value, FQNameParts &Parts);

  void readImportList(yaml::Node *List, SmallVector<StringRef, 8> &Imports);

  void loadClass(yaml::Node* Value, Class *C);

  StringRef getScalarValueOrDie(yaml::Node *N, Twine Msg);

  void loadImportedFile(StringRef Filename, SMLoc IncludeLoc, Unit *U);

public:

  std::unique_ptr<Unit> load(const std::string &InputFile,
                             const std::vector<std::string> &ImportDirs);
};

} // namespace ycd
} // namespace llvm

#endif // LLVM_UTILS_SERYL__LOADER_H
