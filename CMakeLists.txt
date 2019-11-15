add_llvm_utility(
  seryl
  Main.cpp
  Support.cpp
  Support.h
  YCDClass.cpp
  YCDClass.h
  YCDLoader.cpp
  YCDLoader.h
  YCDUnit.cpp
  YCDUnit.h
)

target_link_libraries(seryl PRIVATE LLVMSupport)
