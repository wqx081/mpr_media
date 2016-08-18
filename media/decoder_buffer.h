#ifndef MEDIA_DECODER_BUFFER_H_
#define MEDIA_DECODER_BUFFER_H_

#include <stddef.h>
#include <stdint.h>

#include <memory>
#include <string>
#include <utility>

#include "base/macros.h"
#include "base/aligned_memory.h"
#include "base/ref_counted.h"
#include "base/time.h"
#include "media/timestamp_constants.h"


namespace media {

class DecoderBuffer
    : public base::RefCountedThreadSafe<DecoderBuffer> {
 public:

  enum {
    kPaddingSize = 32,
    kAlignmentSize = 32
  };

  explicit DecoderBuffer(size_t size);

  static scoped_ref_ptr<DecoderBuffer> CopyFrom(const uint8_t* data,
                                                size_t size);
  static scoped_ref_ptr<DecoderBuffer> CopyFrom(const uint8_t* data,
                                                size_t size,
                                                const uint8_t* side_data,
                                                size_t side_data_size);  
  static scoped_ref_ptr<DecoderBuffer> CreateEOSBuffer();

  base::TimeDelta timestamp() const {
    DCHECK(!end_of_stream());
    return timestamp_;
  }  

  virtual void set_timestamp(base::TimeDelta timestamp);
  
  base::TimeDelta duration() const {
    DCHECK(!end_of_stream());
    return duration_;
  }

  void set_duration(base::TimeDelta duration) {
    DCHECK(!end_of_stream());
    DCHECK(duration == kNoTimestamp ||
                      (duration >= base::TimeDelta() && duration != kInfiniteDuration))
        << duration.InSecondsF();
    duration_ = duration;
  }

  const uint8_t* data() const {
    DCHECK(!end_of_stream());
    return data_.get();
  }

  uint8_t* writable_data() const {
    DCHECK(!end_of_stream());
    return data_.get();
  }

  size_t data_size() const {
    DCHECK(!end_of_stream());
    return size_;
  }

  const uint8_t* side_data() const {
    DCHECK(!end_of_stream());
    return side_data_.get();
  }

  size_t side_data_size() const {
    DCHECK(!end_of_stream());
    return side_data_size_;
  }

  typedef std::pair<base::TimeDelta, base::TimeDelta> DiscardPadding;
  const DiscardPadding& discard_padding() const {
    DCHECK(!end_of_stream());
    return discard_padding_;
  }

  void set_discard_padding(const DiscardPadding& discard_padding) {
    DCHECK(!end_of_stream());
    discard_padding_ = discard_padding;
  }

  bool end_of_stream() const {
    return data_ == NULL;
  }

  base::TimeDelta splice_timestamp() const {
    DCHECK(!end_of_stream());
    return splice_timestamp_;
  } 
  
  void set_splice_timestamp(base::TimeDelta splice_timestamp) {
    DCHECK(!end_of_stream());
    splice_timestamp_ = splice_timestamp;
  } 
  
  bool is_key_frame() const {
    DCHECK(!end_of_stream());
    return is_key_frame_;
  } 
  
  void set_is_key_frame(bool is_key_frame) {
    DCHECK(!end_of_stream());
    is_key_frame_ = is_key_frame;
  } 
  
  std::string AsHumanReadableString();
  
  void CopySideDataFrom(const uint8_t* side_data, size_t side_data_size);
  


 protected:
  friend class base::RefCountedThreadSafe<DecoderBuffer>;
  
  DecoderBuffer(const uint8_t* data,
                size_t size,
                const uint8_t* side_data,
                size_t side_data_size);
  virtual ~DecoderBuffer();

 private:
  base::TimeDelta timestamp_;
  base::TimeDelta duration_;

  size_t size_;
  std::unique_ptr<uint8_t, base::AlignedFreeDeleter> data_;
  size_t side_data_size_;
  std::unique_ptr<uint8_t, base::AlignedFreeDeleter> side_data_;
  //DECRYPT
  DiscardPadding discard_padding_;
  base::TimeDelta splice_timestamp_;
  bool is_key_frame_;

  void Initialize();

  DISALLOW_COPY_AND_ASSIGN(DecoderBuffer);
};

} // namespace media
#endif // MEDIA_DECODER_BUFFER_H_
