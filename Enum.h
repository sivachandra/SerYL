#ifndef LLVM_UTILS_SERYL_ENUM_H
#define LLMV_UTILS_SERYL_ENUM_H

#include "Support.h"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace llvm {
namespace seryl {

/// Represents a YCD enum definition.
/// An example of an enums is as follows:
///
/// enum(MyEnum):
///   - Item1
///   - Item2(100)
///   - Item3
///
/// The items, like Item1, Item2 etc above, are listed as a YAML sequence.
/// The values of the items are optional and should be 32-bit integer values
/// listed in parenthesis after the item name. For example, Item2 in the
/// example has a value of 100.
class Enum {
public:

  /// Represents an individual item of an enum definition.
  class Item {
    friend class Enum;

    std::string Name;
    bool HasValue;
    std::int32_t Value;

    Item(const std::string &N, bool HasVal, std::int32_t V = 0)
        : Name(N), HasValue(HasVal), Value(V) {
    }

    Item(const Item&) = delete;

  public:
    const std::string &getName() const {
      return Name;
    }

    bool hasValue() const {
      return HasValue;
    }

    std::int32_t value() const {
      return Value;
    }
  };

private:
  friend class Unit;

  std::string FullyQualifiedName;
  std::vector<Item> Items;

  Enum(const Enum&) = delete;
  explicit Enum(const std::string &FQName) : FullyQualifiedName(FQName) {}

public:

  static std::unique_ptr<Enum> readEnum(const std::string &FQName,
                                        llvm::yaml::Node &EnumBody,
                                        YStreamErrReporter &ER);

  static bool isEnumHeader(llvm::StringRef H);

  // The caller should check if the returned name is a valid identifier.
  static llvm::StringRef getEnumName(llvm::StringRef H);

  const std::string &getFullyQualifiedName() const {
    return FullyQualifiedName;
  }

  using iterator = std::Vector<Item>::iterator;
  using const_iterator = std::Vector<Item>::const_iterator;

  iterator begin() { return Items.begin(); }
  const_iterator begin() const { return Items.begin(); }

};

} // namespace seryl
} // namespace llvm

#endif // LLMV_UTILS_SERYL_ENUM_H
