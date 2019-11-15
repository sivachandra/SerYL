#ifndef LLVM_UTILS_SERYL_YCDREADER_H
#define LLVM_UTILS_SERYL_YCDREADER_H

#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"
#include "llvm/ADT/Twine.h"

#include <memory>

namespace llvm {

namespace yaml {

class Node;
class Stream;

}

class YCDClass;
class YCDUnit;

class YCDReader {
  yaml::Stream *YStream;

  StringRef readPackageName(yaml::Node *Value);

  void readImportList(yaml::Node *List, SmallVector<StringRef, 8> &Imports);

  void loadClass(yaml::Node* Value, YCDClass *C);

  void indexTopLevelNode(yaml::Node *N);

  StringRef getScalarValueOrDie(yaml::Node *N, Twine Msg);

public:

  std::unique_ptr<YCDUnit> read(yaml::Stream *YAMLStream);
};

} // namespace llvm

#endif // LLVM_UTILS_SERYL_YCDUNIT_H
