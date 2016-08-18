#ifndef MEDIA_INPUT_STREAM_H_
#define MEDIA_INPUT_STREAM_H_
#include <stdint.h>

#include "base/callback.h"
#include "base/macros.h"
#include "base/time.h"

namespace media {

class InputStream {
 public:
  typedef base::Callback<void(int64_t, int64_t)> StatusCallback;
  typedef base::Callback<void(int)> ReadCB;

  enum { kReadError = -1 };

  InputStream();
  virtual ~InputStream();

  virtual void Read(int64_t position,
                    int size,
                    uint8_t* data,
                    const InputStream::ReadCB& read_cb) = 0;

  virtual void Stop() = 0;
  virtual bool GetSize(int64_t* size_out) = 0;
  virtual bool IsStreaming() = 0;
  virtual void SetBitrate(int bitrate) = 0;

 private:
  DISALLOW_COPY_AND_ASSIGN(InputStream);
};

} // namespace media
#endif // MEDIA_INPUT_STREAM_H_
