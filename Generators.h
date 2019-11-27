#ifndef LLVM_UTILS_SERYL_GENERATOR_H
#define LLVM_UTILS_SERYL_GENERATOR_H

#include <string>

namespace llvm {
namespace seryl {

class Unit;

void GenCpp(const Unit *U, const std::string &OutDir);

} // namespace seryl
} // namespace llvm

#endif // LLVM_UTILS_SERYL_GENERATOR_H
