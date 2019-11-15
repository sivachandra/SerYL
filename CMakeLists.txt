add_llvm_utility(
  seryl
  Main.cpp
  YCDClass.h
  YCDReader.cpp
  YCDReader.h
  YCDUnit.cpp
  YCDUnit.h
)

target_link_libraries(seryl PRIVATE LLVMSupport)
