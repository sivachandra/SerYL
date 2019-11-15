#include "YCDUnit.h"

#include "YCDReader.h"

#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/raw_ostream.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/YAMLParser.h"

#include <cstdlib>

namespace llvm {

std::unique_ptr<YCDUnit> YCDUnit::load(
    const std::string &Filename,
    const std::vector<std::string> &IncludeDirs) {
  SourceMgr SM;
  SM.setIncludeDirs(IncludeDirs);

  auto Buffer = MemoryBuffer::getFile(Filename);
  if (!Buffer) {
    errs() << "Unable to open " << Filename << "\n";
    std::exit(1);
  }
  MemoryBufferRef BufferRef(*Buffer.get());
  unsigned MainFileID = SM.AddNewSourceBuffer(std::move(Buffer.get()),
                                              SMLoc::getFromPointer(nullptr));

  yaml::Stream YStream(BufferRef, SM);
  YCDReader Reader;
  return Reader.read(&YStream);
}

} // namespace llvm
