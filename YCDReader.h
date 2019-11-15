#ifndef LLVM_UTILS_SERYL_YCDREADER_H
#define LLVM_UTILS_SERYL_YCDREADER_H

#include "YCDUnit.h"

#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/Twine.h"

#include <memory>

namespace llvm {

namespace yaml {

class Node;
class Stream;

} // namespace yaml

class YCDClass;

class YCDReader {
  yaml::Stream *YStream;

  void readPackageName(yaml::Node *Value, YCDName &PkgName);

  void readImportList(yaml::Node *List, SmallVector<StringRef, 8> &Imports);

  void loadClass(yaml::Node* Value, YCDClass *C);

  StringRef getScalarValueOrDie(yaml::Node *N, Twine Msg);

public:

  std::unique_ptr<YCDUnit> read(yaml::Stream *YAMLStream);
};

} // namespace llvm

#endif // LLVM_UTILS_SERYL_YCDREADER_H
