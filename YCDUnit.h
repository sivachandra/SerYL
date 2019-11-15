#ifndef LLVM_UTILS_SERYL_YCDUNIT_H
#define LLVM_UTILS_SERYL_YCDUNIT_H

#include "Support.h"
#include "YCDClass.h"

#include "llvm/ADT/SmallVector.h"
#include "llvm/ADT/StringRef.h"

#include <memory>
#include <string>
#include <vector>

namespace llvm {

class YCDUnit {
  friend class YCDLoader;

  std::string InputFile;
  YCDName PackageName;
  std::vector<YCDClass> Classes;

  explicit YCDUnit(const std::string &InFile,
                   const YCDName &PkgName)
      : InputFile(InFile), PackageName(PkgName) {}

public:
  
  static std::unique_ptr<YCDUnit> load(
      const std::string &InputFile,
      const std::vector<std::string> &IncludeDirs);

  int writeCpp(const std::string &Basename) const;
};

} // namespace llvm

#endif // LLVM_UTILS_SERYL_YCDUNIT_H
