#ifndef MEDIA_BASE_DATA_SOURCE_H_
#define MEDIA_BASE_DATA_SOURCE_H_

#include "base/callback.h"
#include "base/ref_counted.h"
#include "base/time.h"

namespace media {

class DataSourceHost {
 public:
  // Set the total size of the media file.
  virtual void SetTotalBytes(int64_t total_bytes) = 0;

  // Notify the host that byte range [start,end] has been buffered.
  // TODO(fischman): remove this method when demuxing is push-based instead of
  // pull-based.  http://crbug.com/131444
  virtual void AddBufferedByteRange(int64_t start, int64_t end) = 0;

  // Notify the host that time range [start,end] has been buffered.
  virtual void AddBufferedTimeRange(base::TimeDelta start,
                                    base::TimeDelta end) = 0;

 protected:
  virtual ~DataSourceHost();
};

class DataSource : public base::RefCountedThreadSafe<DataSource> {
 public:
  typedef base::Callback<void(int64_t, int64_t)> StatusCallback;
  typedef base::Callback<void(int)> ReadCB;
  static const int kReadError;

  DataSource();

  virtual void set_host(DataSourceHost* host);

  // Reads |size| bytes from |position| into |data|. And when the read is done
  // or failed, |read_cb| is called with the number of bytes read or
  // kReadError in case of error.
  virtual void Read(int64_t position, int size, uint8_t* data,
                    const DataSource::ReadCB& read_cb) = 0;

  // Notifies the DataSource of a change in the current playback rate.
  virtual void SetPlaybackRate(float playback_rate);

  // Stops the DataSource. Once this is called all future Read() calls will
  // return an error.
  virtual void Stop(const base::Closure& callback) = 0;

  // Returns true and the file size, false if the file size could not be
  // retrieved.
  virtual bool GetSize(int64_t* size_out) = 0;

  // Returns true if we are performing streaming. In this case seeking is
  // not possible.
  virtual bool IsStreaming() = 0;

  // Notify the DataSource of the bitrate of the media.
  // Values of |bitrate| <= 0 are invalid and should be ignored.
  virtual void SetBitrate(int bitrate) = 0;

 protected:
  // Only allow scoped_refptr<> to delete DataSource.
  friend class base::RefCountedThreadSafe<DataSource>;
  virtual ~DataSource();

  DataSourceHost* host();

 private:
  DataSourceHost* host_;

  DISALLOW_COPY_AND_ASSIGN(DataSource);
};

}  // namespace media

#endif  // MEDIA_BASE_DATA_SOURCE_H_
