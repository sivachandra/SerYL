// DO NOT EDIT - Generated from ../llvm/utils/SerYL/examples/simple.ycd

#ifndef _____LLVM_UTILS_SERYL_EXAMPLES_SIMPLE_H
#define _____LLVM_UTILS_SERYL_EXAMPLES_SIMPLE_H

#include <cstdint>
#include <string>
#include <vector>

namespace seryl {
namespace examples {
namespace simple {

class Foo {
  private:
  ::std::int32_t MyInt;
  public:
  ::std::int32_t get_MyInt() const {
    return MyInt;
  }
  void set_MyInt(::std::int32_t __val) {
    MyInt = __val;
  }

  private:
  ::std::string MyString;
  public:
  ::std::string &get_MyString() const {
    return MyString;
  }
  void set_MyString(const ::std::string &__str) {
    MyString = __str;
  }

};

class Bar {
  private:
  ::seryl::examples::simple::Foo MyFoo;
  public:
  const ::seryl::examples::simple::Foo &get_MyFoo() const {
    return MyFoo;
  }
  ::seryl::examples::simple::Foo &get_MyFoo() {
    return MyFoo;
  }

  private:
  ::std::vector<double> MyDoubleList;
  public:
  const double &get_MyDoubleList(size_t __elem) const {
    return MyDoubleList[__elem];
  }
  double &get_MyDoubleList(size_t __elem) {
    return MyDoubleList[__elem];
  }

};

} // namespace seryl
} // namespace examples
} // namespace simple

#endif //_____LLVM_UTILS_SERYL_EXAMPLES_SIMPLE_H
