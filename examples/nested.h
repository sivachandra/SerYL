// DO NOT EDIT - Generated from ../llvm/utils/SerYL/examples/nested.ycd

#ifndef _____LLVM_UTILS_SERYL_EXAMPLES_NESTED_H
#define _____LLVM_UTILS_SERYL_EXAMPLES_NESTED_H

#include <cstdint>
#include <string>
#include <vector>

namespace seryl {
namespace examples {
namespace nested {

class MyClass {
public:
  enum NestedEnum {
    A = 101,
    B,
    C,
  };

public:
  class NestedClass {
    private:
    ::std::int32_t IntField;
    public:
    ::std::int32_t get_IntField() const {
      return IntField;
    }
    void set_IntField(::std::int32_t __val) {
      IntField = __val;
    }

    private:
    ::seryl::examples::nested::MyClass::NestedEnum EnumField;
    public:
    ::seryl::examples::nested::MyClass::NestedEnum get_EnumField() const {
      return EnumField;
    }
    void set_EnumField(::seryl::examples::nested::MyClass::NestedEnum __val) {
      EnumField = __val;
    }

  };

  private:
  ::std::string StringField;
  public:
  ::std::string &get_StringField() const {
    return StringField;
  }
  void set_StringField(const ::std::string &__str) {
    StringField = __str;
  }

  private:
  double DoubleField;
  public:
  double get_DoubleField() const {
    return DoubleField;
  }
  void set_DoubleField(double __val) {
    DoubleField = __val;
  }

};

} // namespace seryl
} // namespace examples
} // namespace nested

#endif //_____LLVM_UTILS_SERYL_EXAMPLES_NESTED_H
