#ifndef MEDIA_FFMPEG_CONTAINER_H_
#define MEDIA_FFMPEG_CONTAINER_H_
#include "ffmpeg/ffmpeg.h"

#include "base/macros.h"
#include "base/status.h"

#include <string>

namespace media {

class Container {
 public:
  virtual ~Container() {}

  virtual base::Status Open() = 0;
  virtual base::Status Close() = 0;
};

class InputContainer : public Container {
 public:
  struct Options {
    Options() : start_time(0) {}   
    ~Options() {}

    static Options& Default() { 
      static Options default_options;
      return default_options;
    }

    int64_t start_time;
    std::string format;
  };

  InputContainer(const std::string& file_name,
                 const Options& options);

  virtual ~InputContainer() override;

  virtual base::Status Open() override;
  virtual base::Status Close() override;
  virtual base::Status Seek(int64_t start_time, int64_t seek_timestamp=0); 

  unsigned int nb_streams() const {
    DCHECK(input_context_);
    return input_context_->nb_streams;
  }

  int64_t start_time() const {
    DCHECK(input_context_);
    return input_context_->start_time;
  }

  int64_t duration() const {
    DCHECK(input_context_);
    return input_context_->duration;
  }

  int64_t bit_rate() const {
    DCHECK(input_context_);
    return input_context_->bit_rate;
  }

 private:
  std::string file_name_;
  Options options_;

  AVFormatContext* input_context_;
  AVInputFormat* file_iformat_;   
};

} // namespace container
#endif // MEDIA_FFMPEG_CONTAINER_H_
