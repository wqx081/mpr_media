#ifndef BASE_FILES_MEMORY_MAPPED_FILE_H_
#define BASE_FILES_MEMORY_MAPPED_FILE_H_

#include <stddef.h>
#include <stdint.h>

#include "base/file.h"
#include "base/macros.h"

namespace base {

class FilePath;

class MemoryMappedFile {
 public:
  enum Access {
    READ_ONLY,
    READ_WRITE,
    READ_WRITE_EXTEND,
  };

  // The default constructor sets all members to invalid/null values.
  MemoryMappedFile();
  ~MemoryMappedFile();

  // Used to hold information about a region [offset + size] of a file.
  struct Region {
    static const Region kWholeFile;

    bool operator==(const Region& other) const;
    bool operator!=(const Region& other) const;

    // Start of the region (measured in bytes from the beginning of the file).
    int64_t offset;

    // Length of the region in bytes.
    int64_t size;
  };

  bool Initialize(const FilePath& file_name, Access access);
  bool Initialize(const FilePath& file_name) {
    return Initialize(file_name, READ_ONLY);
  }

  bool Initialize(File file, Access access);
  bool Initialize(File file) {
    return Initialize(std::move(file), READ_ONLY);
  }

  bool Initialize(File file, const Region& region, Access access);
  bool Initialize(File file, const Region& region) {
    return Initialize(std::move(file), region, READ_ONLY);
  }

  const uint8_t* data() const { return data_; }
  uint8_t* data() { return data_; }
  size_t length() const { return length_; }

  // Is file_ a valid file handle that points to an open, memory mapped file?
  bool IsValid() const;

 private:
  // Given the arbitrarily aligned memory region [start, size], returns the
  // boundaries of the region aligned to the granularity specified by the OS,
  // (a page on Linux, ~32k on Windows) as follows:
  // - |aligned_start| is page aligned and <= |start|.
  // - |aligned_size| is a multiple of the VM granularity and >= |size|.
  // - |offset| is the displacement of |start| w.r.t |aligned_start|.
  static void CalculateVMAlignedBoundaries(int64_t start,
                                           int64_t size,
                                           int64_t* aligned_start,
                                           int64_t* aligned_size,
                                           int32_t* offset);

  // Map the file to memory, set data_ to that memory address. Return true on
  // success, false on any kind of failure. This is a helper for Initialize().
  bool MapFileRegionToMemory(const Region& region, Access access);

  // Closes all open handles.
  void CloseHandles();

  File file_;
  uint8_t* data_;
  size_t length_;

  DISALLOW_COPY_AND_ASSIGN(MemoryMappedFile);
};

}  // namespace base

#endif  // BASE_FILES_MEMORY_MAPPED_FILE_H_
