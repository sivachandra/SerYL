#ifndef LLVM_UTILS_SERYL_YCDUNIT_H
#define LLVM_UTILS_SERYL_YCDUNIT_H

#include "YCDClass.h"

#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"

#include <memory>
#include <string>
#include <vector>

namespace llvm {

typedef SmallVector<std::string, 8> YCDName;

class YCDUnit {
  friend class YCDReader;

  YCDName PackageName;
  std::vector<YCDClass> Classes;

  explicit YCDUnit(const YCDName &PkgName) : PackageName(PkgName) {}

public:
  
  static std::unique_ptr<YCDUnit> load(
      const std::string &Filename,
      const std::vector<std::string> &IncludeDirs);

  void writeHeader(llvm::raw_ostream &OS) const;
  void writeImpl(llvm::raw_ostream &OS) const;
};

} // namespace llvm

#endif // LLVM_UTILS_SERYL_YCDUNIT_H
