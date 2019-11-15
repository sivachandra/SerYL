#include "Loader.h"

#include "Support.h"
#include "Class.h"
#include "Unit.h"

#include "llvm/ADT/StringExtras.h"
#include "llvm/Support/Casting.h"
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/YAMLParser.h"

namespace {

const char KW_Import[] = "__import__";
const char KW_Package[] = "__package__";

} // anonymous namespace

namespace llvm {
namespace ycd {

StringRef Loader::getScalarValueOrDie(yaml::Node *N, Twine Msg) {
  if (N->getType() != yaml::Node::NK_Scalar) {
    YStream->printError(N, Msg);
    std::exit(1);
  }
  return dyn_cast<yaml::ScalarNode>(N)->getRawValue();
}

void Loader::readImportList(yaml::Node *N, SmallVector<StringRef, 8> &Imports) {
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

void Loader::loadImportedFile(StringRef Filename, SMLoc IncludeLoc, Unit *U) {
  std::string FullPath;
  unsigned BufferID = SrcMgr->AddIncludeFile(Filename, IncludeLoc, FullPath);
  const MemoryBuffer *Buffer = SrcMgr->getMemoryBuffer(BufferID);
  MemoryBufferRef BufferRef(*Buffer);

  U->ImportedFiles.push_back(Filename);
  yaml::Stream ImportedYStream(BufferRef, *SrcMgr.get());
  for (auto IT = ImportedYStream.begin(); IT != ImportedYStream.end(); ++IT) {
    yaml::Node *TopLevel = IT->getRoot();
    if (TopLevel->getType() != yaml::Node::NK_Mapping) {
      ImportedYStream.printError(
          TopLevel, std::string("Top level structure in imported file ") +
              Filename + " is not a mapping.");
      std::exit(1);
    }

    StringRef PkgName;
    yaml::MappingNode *M = llvm::dyn_cast<yaml::MappingNode>(TopLevel);
    int KeyValueCount = 0;
    for (auto IT = M->begin(); IT != M->end(); ++IT) {
      ++KeyValueCount;
      yaml::KeyValueNode &N = *IT;
      yaml::Node *Key = N.getKey();
      yaml::Node *Value = N.getValue();

      StringRef KeyStr = getScalarValueOrDie(
          Key, "Top-level key is not a scalar.");
      if (!isValidIdentifier(KeyStr)) {
        ImportedYStream.printError(Key, "Invalid top-level key name.");
        std::exit(1);
      }
      if (KeyValueCount == 1) {
        if (KeyStr != KW_Package) {
          ImportedYStream.printError(
              Key, StringRef("First key should be ") + KW_Package);
          std::exit(1);
        }
        FQNameParts PkgNameParts;
        PkgName = readPackageName(Value, PkgNameParts);
        
        continue;
      }

      assert(PkgName.size() > 0 && "Package name is empty");
      if (KeyStr == KW_Import) {
        continue;
      }

      std::string FQClassName = std::string(PkgName) + "." + std::string(KeyStr);
      printf("Adding type %s\n", FQClassName.data());
      U->Types.emplace_back(std::make_unique<Type>(KeyStr, FQClassName, Type::TK_Class, false));
      Type *ClassType = U->Types.back().get();
      TypeMap[FQClassName] = ClassType;
    }
    return;
  }
}

void Loader::loadClass(yaml::Node *Value, Class *C) {
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

    StringRef FieldTypeName = getScalarValueOrDie(
        FieldNode.getValue(),
        "Field name should be a simple scalar string.");
    StringRef BaseTypeName = Type::isListType(FieldTypeName);
    std::string ListTypeName;
    if (BaseTypeName.size() > 0) {
      ListTypeName = "list<";
      ListTypeName += std::string(BaseTypeName);
      ListTypeName += ">";
      FieldTypeName = ListTypeName;
    }
    auto MI = TypeMap.find(FieldTypeName);
    if (MI == TypeMap.end()) {
      YStream->printError(
          &FieldNode, "Unknown field type " + FieldTypeName + ".");
      std::exit(1);
    }

    auto NameTypePair = *MI;
    C->addField(FieldName, NameTypePair.second);
  }
}

StringRef Loader::readPackageName(yaml::Node *Value, FQNameParts &Parts) {
  StringRef FQPkgName = getScalarValueOrDie(
      Value, "Package name should be a scalar value.");
  if (!isFullyQualifiedName(FQPkgName, Parts)) {
    YStream->printError(Value, "Invalid package name.");
    std::exit(1);
  }
  return FQPkgName;
}

std::unique_ptr<Unit> Loader::load(
    const std::string &InputFile,
    const std::vector<std::string> &ImportDirs) {
  SrcMgr.reset(new SourceMgr);
  SrcMgr->setIncludeDirs(ImportDirs);
  TypeMap.clear();

  auto Buffer = MemoryBuffer::getFile(InputFile);
  if (!Buffer) {
    errs() << "Unable to open " << InputFile << "\n";
    std::exit(1);
  }
  MemoryBufferRef BufferRef(*Buffer.get());
  SrcMgr->AddNewSourceBuffer(std::move(Buffer.get()),
                            SMLoc::getFromPointer(nullptr));
  YStream.reset(new yaml::Stream(BufferRef, *SrcMgr));

  for (auto IT = YStream->begin(); IT != YStream->end(); ++IT) {
    yaml::Node *TopLevel = IT->getRoot();
    if (TopLevel->getType() != yaml::Node::NK_Mapping) {
      YStream->printError(TopLevel, "Top level structure is not a mapping.");
      std::exit(1);
    }

    std::unique_ptr<Unit> U;
    StringRef PkgName;

    yaml::MappingNode *M = llvm::dyn_cast<yaml::MappingNode>(TopLevel);
    int KeyValueCount = 0;
    for (auto IT = M->begin(); IT != M->end(); ++IT) {
      ++KeyValueCount;
      yaml::KeyValueNode &N = *IT;
      yaml::Node *Key = N.getKey();
      yaml::Node *Value = N.getValue();

      StringRef KeyStr = getScalarValueOrDie(
          Key, "Top-level key is not a scalar.");
      if (!isValidIdentifier(KeyStr)) {
        YStream->printError(Key, "Invalid top-level key name.");
        std::exit(1);
      }

      if (KeyValueCount == 1) {
        if (KeyStr != KW_Package) {
          YStream->printError(
              Key, StringRef("First key should be ") + KW_Package);
          std::exit(1);
        }
        FQNameParts PkgNameParts;
        PkgName = readPackageName(Value, PkgNameParts);
        
        U.reset(new Unit(InputFile, PkgName));
        for (auto &T : U->Types) {
          TypeMap[T->getFullyQualifiedName()] = T.get();
        }
        continue;
      }

      assert(U && "A Unit has not been created.");

      if (KeyStr == KW_Import) {
        SmallVector<StringRef, 8> Imports;
        readImportList(Value, Imports);
        for (StringRef ImportFilename : Imports) {
          if (!ImportFilename.endswith(YCDSuffix)) {
            YStream->printError(
                Value, "Imported file should have a '.ycd' suffix.");
          }
          loadImportedFile(ImportFilename,
                           SMLoc::getFromPointer(ImportFilename.data()),
                           U.get());
        }
        continue;
      }

      // KeyStr is a class name.
      auto &ClassVector = U->Classes;
      ClassVector.emplace_back(Class(KeyStr));
      loadClass(Value, &ClassVector.back());
      std::string FQClassName = std::string(PkgName) + "." + std::string(KeyStr);
      U->Types.emplace_back(std::make_unique<Type>(KeyStr, FQClassName, Type::TK_Class, true));
      Type *ClassType = U->Types.back().get();
      TypeMap[FQClassName] = ClassType;
      TypeMap[std::string(KeyStr)] = ClassType;
      printf("Addied type for %s\n", FQClassName.data());
    }

    return U;
  }

  return nullptr;
}

} // namespace ycd
} // namespace llvm
