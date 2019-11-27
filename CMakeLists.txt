add_llvm_utility(
  seryl
  Main.cpp
  Class.cpp
  Class.h
  Enum.cpp
  Enum.h
  Field.cpp
  Field.h
  Generators.h
  Package.cpp
  Package.h
  Scope.h
  Support.cpp
  Support.h
  Type.cpp
  Type.h
  Unit.cpp
  Unit.h

  GenCpp.cpp
)

target_link_libraries(seryl PRIVATE LLVMSupport)
