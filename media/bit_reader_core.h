#ifndef MEDIA_BASE_BIT_READER_CORE_H_
#define MEDIA_BASE_BIT_READER_CORE_H_

#include <stdint.h>

#include <glog/logging.h>

#include "base/macros.h"

namespace media {

class BitReaderCore {
 public:
  class ByteStreamProvider {
   public:
    ByteStreamProvider();
    virtual ~ByteStreamProvider();
    virtual int GetBytes(int max_n, const uint8_t** array) = 0;
  };

  explicit BitReaderCore(ByteStreamProvider* byte_stream_provider);
  ~BitReaderCore();

  bool ReadBits(int num_bits, bool* out) {
    DCHECK_EQ(num_bits, 1);
    return ReadFlag(out);
  }

  template<typename T> bool ReadBits(int num_bits, T* out) {
    DCHECK_LE(num_bits, static_cast<int>(sizeof(T) * 8));
    uint64_t temp;
    bool ret = ReadBitsInternal(num_bits, &temp);
    *out = static_cast<T>(temp);
    return ret;
  }

  bool ReadFlag(bool* flag);

  int PeekBitsMsbAligned(int num_bits, uint64_t* out);
  bool SkipBits(int num_bits);
  int bits_read() const;

 private:
  bool SkipBitsSmall(int num_bits);
  bool ReadBitsInternal(int num_bits, uint64_t* out);
  bool Refill(int min_nbits);
  void RefillCurrentRegister();
  ByteStreamProvider* const byte_stream_provider_;
  int bits_read_;
  int nbits_;
  uint64_t reg_;
  int nbits_next_;
  uint64_t reg_next_;

  DISALLOW_COPY_AND_ASSIGN(BitReaderCore);
};

}  // namespace media

#endif  // MEDIA_BASE_BIT_READER_CORE_H_
