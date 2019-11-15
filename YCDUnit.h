#ifndef LLVM_UTILS_SERYL_YCDUNIT_H
#define LLVM_UTILS_SERYL_YCDUNIT_H

#include "YCDClass.h"

#include "llvm/ADT/StringRef.h"

#include <memory>
#include <string>
#include <vector>

namespace llvm {

class YCDUnit {
  friend class YCDReader;

  std::string PackageName;
  std::vector<YCDClass> Classes;

  explicit YCDUnit(llvm::StringRef PkgName) : PackageName(PkgName) {}

public:
  
  static std::unique_ptr<YCDUnit> load(
      const std::string &Filename,
      const std::vector<std::string> &IncludeDirs);

  void writeHeader(llvm::raw_ostream &OS) const;
  void writeImpl(llvm::raw_ostream &OS) const;
};

} // namespace llvm

#endif // LLVM_UTILS_SERYL_YCDUNIT_H
