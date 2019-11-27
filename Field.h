#ifndef LLVM_UTILS_SERYL_FIELD_H
#define LLVM_UTILS_SERYL_FIELD_H

#include "Type.h"

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

  Field(const Field &) = delete;

public:
  Field() {}

  typedef NameTypeMap::iterator iterator;
  typedef NameTypeMap::const_iterator const_iterator;

  static bool isOneofField();

  iterator begin() {
    return FieldMap.begin();
  }

  const_iterator begin() const {
    return FieldMap.begin();
  }

  iterator end() {
    return FieldMap.end();
  }

  const_iterator end() const {
    return FieldMap.end();
  }

  bool isOneof() const { return FieldMap.size() > 1; }

  static bool isOneofField(llvm::StringRef FieldStr) {
    return false;
  }

};

} // namespace seryl
} // namespace llvm

#endif // LLVM_UTILS_SERYL_FIELD_H
