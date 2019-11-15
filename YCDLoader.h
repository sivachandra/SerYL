#ifndef LLVM_UTILS_SERYL_YCD_LOADER_H
#define LLVM_UTILS_SERYL_YCD_LOADER_H

#include "YCDUnit.h"

#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/Twine.h"
#include "llvm/Support/SourceMgr.h"

#include <memory>

namespace llvm {

namespace yaml {

class Node;
class Stream;

} // namespace yaml

class YCDClass;

class YCDLoader {
  std::unique_ptr<yaml::Stream> YStream;
  std::unique_ptr<SourceMgr> SrcMgr;

  void readPackageName(yaml::Node *Value, YCDName &PkgName);

  void readImportList(yaml::Node *List, SmallVector<StringRef, 8> &Imports);

  void loadClass(yaml::Node* Value, YCDClass *C);

  StringRef getScalarValueOrDie(yaml::Node *N, Twine Msg);

public:

  std::unique_ptr<YCDUnit> load(const std::string &InputFile,
                                const std::vector<std::string> &ImportDirs);
};

} // namespace llvm

#endif // LLVM_UTILS_SERYL_YCD_LOADER_H
