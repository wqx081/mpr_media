// A specialized buffer for interfacing with audio / video decoders.
//
// Specifically ensures that data is aligned and padded as necessary by the
// underlying decoding framework.  On desktop platforms this means memory is
// allocated using FFmpeg with particular alignment and padding requirements.
//
// Also includes decoder specific functionality for decryption.

#ifndef MEDIA_BASE_DECODER_BUFFER_H_
#define MEDIA_BASE_DECODER_BUFFER_H_

#include "base/ref_counted.h"
#include "media/buffers.h"
//#include "media/decrypt_config.h"

namespace media {

class DecoderBuffer : public Buffer {
 public:
  enum {
    kPaddingSize = 16,
    kAlignmentSize = 32
  };

  // Allocates buffer of size |buffer_size| >= 0.  Buffer will be padded and
  // aligned as necessary.
  explicit DecoderBuffer(int buffer_size);

  // Create a DecoderBuffer whose |data_| is copied from |data|.  Buffer will be
  // padded and aligned as necessary.  |data| must not be NULL and |size| >= 0.
  static scoped_ref_ptr<DecoderBuffer> CopyFrom(const uint8_t* data, int size);

  // Create a DecoderBuffer indicating we've reached end of stream.  GetData()
  // and GetWritableData() will return NULL and GetDataSize() will return 0.
  static scoped_ref_ptr<DecoderBuffer> CreateEOSBuffer();

  // Buffer implementation.
  virtual const uint8_t* GetData() const override;
  virtual int GetDataSize() const override;

  // Returns a read-write pointer to the buffer data.
  virtual uint8_t* GetWritableData();

  //virtual const DecryptConfig* GetDecryptConfig() const;
  //virtual void SetDecryptConfig(scoped_ptr<DecryptConfig> decrypt_config);

 protected:
  // Allocates a buffer of size |size| >= 0 and copies |data| into it.  Buffer
  // will be padded and aligned as necessary.  If |data| is NULL then |data_| is
  // set to NULL and |buffer_size_| to 0.
  DecoderBuffer(const uint8_t* data, int size);
  virtual ~DecoderBuffer();

 private:
  int buffer_size_;
  uint8_t* data_;
  //scoped_ref_ptr<DecryptConfig> decrypt_config_;

  // Constructor helper method for memory allocations.
  void Initialize();

  DISALLOW_COPY_AND_ASSIGN(DecoderBuffer);
};

}  // namespace media

#endif  // MEDIA_BASE_DECODER_BUFFER_H_
