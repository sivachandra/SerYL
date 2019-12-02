#include "Enum.h"

#include <cstdlib>
#include <unordered_set>

static const char EnumPrefix[] = "enum(";
static size_t EnumPrefixSize = llvm::StringRef(EnumPrefix).size();
static const char EnumSuffix[] = ")";
static size_t EnumSuffixSize = llvm::StringRef(EnumSuffix).size();

namespace llvm {
namespace seryl {

std::unique_ptr<Enum> Enum::readEnum(llvm::StringRef Name,
                                     Scope *PS,
                                     llvm::yaml::Node &EnumBody,
                                     YStreamErrReporter &ER) {
  if (EnumBody.getType() != llvm::yaml::Node::NK_Sequence) {
    ER.printFatalError(&EnumBody, "Enum body should be a sequence.");
  }

  llvm::yaml::SequenceNode *EnumBodyAsSequence =
      llvm::dyn_cast<llvm::yaml::SequenceNode>(&EnumBody);

  auto E = std::make_unique<Enum>(Name, PS);

  // This set will hold the item names. When adding an item, we will
  // error out on duplicate names.
  std::unordered_set<std::string> NameSet;

  for (llvm::yaml::Node &N : *EnumBodyAsSequence) {
    if (N.getType() != llvm::yaml::Node::NK_Scalar) {
      ER.printFatalError(&N, "Enum item should be a scalar.");
    }

    llvm::yaml::ScalarNode *ItemNode =
        llvm::dyn_cast<llvm::yaml::ScalarNode>(&N);
    assert(ItemNode && "Enum node could not be cast to a ScalarNode");

    StringRef ItemStr = ItemNode->getRawValue();
    StringRef ItemName;
    bool HasVal;
    std::int32_t Val;
    if (ItemStr.endswith(")")) {
      auto Pair = ItemStr.split("(");
      if (Pair.second.size() == 0) {
        ER.printFatalError(ItemNode, "Invalid item syntax.");
      }

      HasVal = true;
      ItemName = Pair.first.trim();
      StringRef ValStr = Pair.second.drop_back(1);
      if (ValStr.getAsInteger(0, Val)) {
        ER.printFatalError(ItemNode,
                           "Item value is not a valid 32-bit integer.");
      }
    } else {
      ItemName = ItemStr;
      HasVal = false;
      Val = 0;
    }

    if (!isValidIdentifier(ItemName)) {
      ER.printFatalError(ItemNode, "Invalid name for enum item.");
    }
    if (NameSet.find(ItemName) != NameSet.end()) {
      ER.printFatalError(ItemNode, "Duplicate item name.");
    }

    NameSet.insert(ItemName);
    E->Items.emplace_back(ItemName, HasVal, Val);
  }

  E->Complete = true;
  return E;
}

bool Enum::isEnumHeader(llvm::StringRef H) {
  return H.startswith(EnumPrefix) and H.endswith(EnumSuffix);
}

llvm::StringRef Enum::getEnumName(llvm::StringRef H) {
  assert(Enum::isEnumHeader(H) &&
         "Trying to get enum name from an invalid enum header");
  return H.drop_front(EnumPrefixSize).drop_back(EnumSuffixSize);
}

} // namespace seryl 
} // namespace llvm
