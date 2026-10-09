#ifndef STREAM_CAPTURE_H
#define STREAM_CAPTURE_H

#include <ostream>
#include <sstream>
#include <streambuf>
#include <string>

namespace AnalysisCore {

class StreamRedirect {
public:
  explicit StreamRedirect(std::ostream &origin,
                          std::streambuf *destination_buffer)
      : origin_(origin), old_buffer_(origin.rdbuf(destination_buffer)) {}
  explicit StreamRedirect(std::ostream &origin, std::ostream &destination)
      : StreamRedirect(origin, destination.rdbuf()) {}
  ~StreamRedirect() { origin_.rdbuf(old_buffer_); }
  StreamRedirect(const StreamRedirect &) = delete;
  StreamRedirect(StreamRedirect &&) = delete;
  StreamRedirect &operator=(const StreamRedirect &) = delete;
  StreamRedirect &operator=(StreamRedirect &&) = delete;

private:
  std::ostream &origin_;
  std::streambuf *old_buffer_;
};

class StreamCapture {
public:
  explicit StreamCapture(std::ostream &stream)
      : redirect_(stream, captured_.rdbuf()) {}
  ~StreamCapture() = default;
  StreamCapture(const StreamCapture &) = delete;
  StreamCapture &operator=(const StreamCapture &) = delete;
  StreamCapture(StreamCapture &&) = delete;
  StreamCapture &operator=(StreamCapture &&) = delete;
  std::string str() const { return captured_.str(); }

private:
  std::ostringstream captured_;
  StreamRedirect redirect_;
};

} // namespace AnalysisCore

#endif