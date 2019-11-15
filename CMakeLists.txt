add_llvm_utility(
  seryl
  Main.cpp
  Class.cpp
  Class.h
  Loader.cpp
  Loader.h
  Type.cpp
  Type.h
  Unit.cpp
  Unit.h
  Support.cpp
  Support.h
)

target_link_libraries(seryl PRIVATE LLVMSupport)
