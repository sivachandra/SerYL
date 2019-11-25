#ifndef LLVM_UTILS_SERYL_FIELD_H
#define LLVM_UTILS_SERYL_FIELD_H

#include <string>
#include <unordered_map>

namespace llvm {
namespace seryl {

class Field {
private:
  friend class Class;

  typedef std::unordered_map<std::string, Type> NameTypeMap;
  // We keep a map to handle __oneof__ fields.
  NameTypeMap FieldMap;

  NameTypeMap(const NameTypeMap &) = delete;

  Field() {}

  Field(const Field &) = delete;

public:
  typedef NameTypeMap::iterator iterator;
  typedef NameTypeMap::const_iterator const_iterator;

  static bool isOneofField();

  iterator begin() {
    return FieldMap.begin();
  }

  const_iterator begin() const {
    FieldMap.begin();
  }

  iterator end() {
    FieldMap.end();
  }

  const_iterator end() const {
    FieldMap.end();
  }

  bool isOneof() const { return NameTypeMap.size() > 1; }

};

} // namespace seryl
} // namespace llvm

#endif // LLVM_UTILS_SERYL_FIELD_H
