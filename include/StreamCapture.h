#ifndef STREAM_CAPTURE_H
#define STREAM_CAPTURE_H

#include <ostream>
#include <sstream>
#include <streambuf>
#include <string>

namespace AnalysisCore {

class StreamRedirect {
public:
  explicit StreamRedirect(std::ostream &stream, std::streambuf *new_buffer)
      : original_stream_(stream), old_buffer_(stream.rdbuf(new_buffer)) {}
  ~StreamRedirect() { original_stream_.rdbuf(old_buffer_); }
  StreamRedirect(const StreamRedirect &) = delete;
  StreamRedirect(StreamRedirect &&) = delete;
  StreamRedirect &operator=(const StreamRedirect &) = delete;
  StreamRedirect &operator=(StreamRedirect &&) = delete;

private:
  std::ostream &original_stream_;
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