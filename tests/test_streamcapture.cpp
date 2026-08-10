#include <iostream>
#include <sstream>
#include <streambuf>
#include <type_traits>

#include <gtest/gtest.h>

#include "StreamCapture.h"

// ---- StreamRedirect -------------------------------------------------------

class StreamRedirectTest : public ::testing::Test {
protected:
  void SetUp() override { original_buf_ = std::clog.rdbuf(); }
  void TearDown() override { std::clog.rdbuf(original_buf_); }
  std::streambuf *original_buf_ = nullptr; // NOLINT
};

TEST_F(StreamRedirectTest, RedirectsOutput) {
  const std::ostringstream streamed_string;
  const AnalysisCore::StreamRedirect redirect(std::clog,
                                              streamed_string.rdbuf());
  std::clog << "redirected";
  EXPECT_EQ(streamed_string.str(), "redirected");
}

TEST_F(StreamRedirectTest, RestoresPreviousBuffer) {
  const std::ostringstream outer_stream;
  std::clog.rdbuf(outer_stream.rdbuf());
  {
    const std::ostringstream inner_stream;
    const AnalysisCore::StreamRedirect redirect(std::clog,
                                                inner_stream.rdbuf());
    std::clog << "captured";
    EXPECT_EQ(inner_stream.str(), "captured");
  }
  std::clog << "outer";
  EXPECT_EQ(outer_stream.str(), "outer");
}

TEST(StreamRedirectTypeTraitsTest, IsNotCopyableOrMovable) {
  EXPECT_FALSE(std::is_copy_constructible_v<AnalysisCore::StreamRedirect>);
  EXPECT_FALSE(std::is_copy_assignable_v<AnalysisCore::StreamRedirect>);
  EXPECT_FALSE(std::is_move_constructible_v<AnalysisCore::StreamRedirect>);
  EXPECT_FALSE(std::is_move_assignable_v<AnalysisCore::StreamRedirect>);
}

// ---- StreamCapture --------------------------------------------------------

TEST(StreamCaptureTest, CapturesOutput) {
  std::ostringstream string_stream;
  const AnalysisCore::StreamCapture capture(string_stream);
  string_stream << "hello";
  string_stream << ' ';
  string_stream << "world";
  EXPECT_EQ(capture.str(), "hello world");
}

TEST(StreamCaptureTest, StartsEmpty) {
  std::ostringstream string_stream;
  const AnalysisCore::StreamCapture capture(string_stream);
  EXPECT_TRUE(capture.str().empty());
}

TEST(StreamCaptureTest, RestoresOriginalBufferOnDestruction) {
  std::ostringstream string_stream;
  {
    const AnalysisCore::StreamCapture capture(string_stream);
    string_stream << "captured";
  }
  string_stream << " restored";
  EXPECT_EQ(string_stream.str(), " restored");
}

TEST(StreamCaptureTypeTraitsTest, IsNotCopyableOrMovable) {
  EXPECT_FALSE(std::is_copy_constructible_v<AnalysisCore::StreamCapture>);
  EXPECT_FALSE(std::is_copy_assignable_v<AnalysisCore::StreamCapture>);
  EXPECT_FALSE(std::is_move_constructible_v<AnalysisCore::StreamCapture>);
  EXPECT_FALSE(std::is_move_assignable_v<AnalysisCore::StreamCapture>);
}