#ifndef CC_FFMPEG_EXCEPTION_H_
#define CC_FFMPEG_EXCEPTION_H_

#include <exception>
#include <string>

namespace ffmpeg {

class Exception : public std::exception {
 public:
  Exception() {}
  Exception(Exception&&) noexcept {}
  Exception(const Exception&) {}
  Exception& operator=(const Exception&) {
    return *this;
  }
  Exception& operator=(Exception&&) { return *this; }
};

class FFmpegException : public Exception {
 public:
  FFmpegException() {}
  explicit FFmpegException(const std::string& message)
    : message_(message) {}

  FFmpegException(const char* message, int err) {
    message_ = std::string(message) + ":" + std::to_string(err);
  }

  ~FFmpegException() throw() override {}

  const char* what() const throw() override {
    if (message_.empty()) {
      return "Default FFmpegException";
    } else {
      return message_.c_str();
    }
  }
 protected:
  std::string message_;
};

} // namespace ffmpeg
#endif //CC_FFMPEG_EXCEPTION_H_

