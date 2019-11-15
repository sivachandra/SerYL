#include "YCDReader.h"

#include "YCDClass.h"
#include "YCDUnit.h"

#include "llvm/ADT/StringExtras.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/YAMLParser.h"

namespace {

const char KW_Import[] = "__import__";
const char KW_Package[] = "__package__";

bool isValidIdentifier(llvm::StringRef Id) {
  assert(Id.size() > 0);

  for (size_t i = 0; i < Id.size(); ++i) {
    char c = Id[i];
    if (c == '_' || llvm::isAlpha(c)) {
      continue;
    }
    if (i > 0 && llvm::isDigit(c)) {
      continue;
    }
    return false;
  }

  return true;
}

bool isValidFQName(llvm::StringRef FQName, llvm::YCDName &Name) {
  llvm::SmallVector<llvm::StringRef, 8> Parts;
  FQName.split(Parts, ".");
  for (llvm::StringRef P : Parts) {
    if (!isValidIdentifier(P)) {
      return false;
    }
    Name.push_back(P);
  }
  return true;
}

} // anonymous namespace

namespace llvm {

StringRef YCDReader::getScalarValueOrDie(yaml::Node *N, Twine Msg) {
  if (N->getType() != yaml::Node::NK_Scalar) {
    YStream->printError(N, Msg);
    std::exit(1);
  }
  return dyn_cast<yaml::ScalarNode>(N)->getRawValue();
}

void YCDReader::readImportList(yaml::Node *N, SmallVector<StringRef, 8> &Imports) {
  if (N->getType() != yaml::Node::NK_Sequence) {
    YStream->printError(N, "Imports should be listed as a sequence.");
    std::exit(1);
  }

  yaml::SequenceNode *SN = dyn_cast<yaml::SequenceNode>(N);
  for (auto IT = SN->begin(); IT != SN->end(); ++IT) {
    yaml::Node &Elem = *IT;
    Imports.push_back(getScalarValueOrDie(
        &Elem, "Import list should consist of scalar values"));
  }
}

void YCDReader::loadClass(yaml::Node *Value, YCDClass *C) {
  if (Value->getType() != yaml::Node::NK_Mapping) {
    YStream->printError(Value, "Class body should be a mapping");
    std::exit(1);
  }

  yaml::MappingNode *MapValue = dyn_cast<yaml::MappingNode>(Value);
  for (auto IT = MapValue->begin(); IT != MapValue->end(); ++IT) {
    yaml::KeyValueNode &FieldNode = *IT;
    if (FieldNode.getType() != yaml::Node::NK_KeyValue) {
      YStream->printError(&FieldNode,
                          "Class field should be a key-value pair.");
      std::exit(1);
    }

    StringRef FieldName = getScalarValueOrDie(
        FieldNode.getKey(),
        "Field name should be a simple scalar string.");
    if (!isValidIdentifier(FieldName)) {
      YStream->printError(FieldNode.getKey(), "Invalid field name.");
      std::exit(1);
    }

    StringRef FieldType = getScalarValueOrDie(
        FieldNode.getValue(),
        "Field name should be a simple scalar string.");
    // TODO: Validate FieldType. The type should be type already
    // at this point.

    C->addField(FieldName, FieldType);
  }
}

void YCDReader::readPackageName(yaml::Node *Value, YCDName &PkgName) {
  StringRef FQPkgName = getScalarValueOrDie(
      Value, "Package name should be a scalar value.");
  if (!isValidFQName(FQPkgName, PkgName)) {
    YStream->printError(Value, "Invalid package name.");
    std::exit(1);
  }
}

std::unique_ptr<YCDUnit> YCDReader::read(yaml::Stream *YAMLStream) {
  YStream = YAMLStream;
  for (auto IT = YStream->begin(); IT != YStream->end(); ++IT) {
    yaml::Node *TopLevel = IT->getRoot();
    if (TopLevel->getType() != yaml::Node::NK_Mapping) {
      YAMLStream->printError(TopLevel, "Top level structure is not a mapping.");
      std::exit(1);
    }

    std::unique_ptr<YCDUnit> Unit;

    yaml::MappingNode *M = llvm::dyn_cast<yaml::MappingNode>(TopLevel);
    int KeyValueCount = 0;
    for (auto IT = M->begin(); IT != M->end(); ++IT) {
      ++KeyValueCount;
      yaml::KeyValueNode &N = *IT;
      yaml::Node *Key = N.getKey();
      yaml::Node *Value = N.getValue();

      StringRef KeyStr = getScalarValueOrDie(Key, "Expecting a scalar top-level key.");
      if (!isValidIdentifier(KeyStr)) {
        YStream->printError(Key, "Invalid top-level key name.");
        std::exit(1);
      }

      if (KeyValueCount == 1) {
        if (KeyStr != KW_Package) {
          YStream->printError(Key, StringRef("First key should be ") + KW_Package);
          std::exit(1);
        }
        YCDName PkgName;
        readPackageName(Value, PkgName);
        
        Unit.reset(new YCDUnit(PkgName));
        continue;
      }

      assert(Unit); // Unit should be created if the control reaches here.

      if (KeyStr == KW_Import) {
        SmallVector<StringRef, 8> Imports;
        readImportList(Value, Imports);
        // TODO: Loded imported files and get the imported classes.
        continue;
      }

      // KeyStr is a class name.
      YCDClass C(KeyStr);
      loadClass(Value, &C);
      Unit->Classes.emplace_back(C);
    }

    return Unit;
  }

  return nullptr;
}

} // namespace llvm
