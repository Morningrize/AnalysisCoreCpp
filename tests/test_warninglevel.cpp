#include <iostream>
#include <stdexcept>
#include <string>

#include <gtest/gtest.h>

#include "StreamCapture.h"
#include "WarningLevel.h"

class WarningLevelTest : public ::testing::Test {
protected:
  void TearDown() override {
    AnalysisCore::set_warning_level(AnalysisCore::WarningLevel::WARN);
  }
};

TEST_F(WarningLevelTest, DefaultWarningLevel) {
  EXPECT_EQ(AnalysisCore::get_warning_level(),
            AnalysisCore::WarningLevel::WARN);
}

TEST_F(WarningLevelTest, SetAndGet) {
  AnalysisCore::set_warning_level(AnalysisCore::WarningLevel::SILENT);
  EXPECT_EQ(AnalysisCore::get_warning_level(),
            AnalysisCore::WarningLevel::SILENT);

  AnalysisCore::set_warning_level(AnalysisCore::WarningLevel::WARN);
  EXPECT_EQ(AnalysisCore::get_warning_level(),
            AnalysisCore::WarningLevel::WARN);

  AnalysisCore::set_warning_level(AnalysisCore::WarningLevel::ERROR);
  EXPECT_EQ(AnalysisCore::get_warning_level(),
            AnalysisCore::WarningLevel::ERROR);
}

TEST_F(WarningLevelTest, HandleWarningSilent) {
  AnalysisCore::set_warning_level(AnalysisCore::WarningLevel::SILENT);
  const AnalysisCore::StreamCapture test_stream(std::clog);
  EXPECT_NO_THROW(
      AnalysisCore::handle_warning("This message should not appear"));
  EXPECT_TRUE(test_stream.str().empty());
}

TEST_F(WarningLevelTest, HandleWarningWarn) {
  AnalysisCore::set_warning_level(AnalysisCore::WarningLevel::WARN);
  const AnalysisCore::StreamCapture test_stream(std::clog);
  EXPECT_NO_THROW(AnalysisCore::handle_warning("Warning message"));
  EXPECT_NE(test_stream.str().find("Warning message"), std::string::npos);
}

TEST_F(WarningLevelTest, HandleWarningError) {
  AnalysisCore::set_warning_level(AnalysisCore::WarningLevel::ERROR);
  EXPECT_THROW(AnalysisCore::handle_warning("Test error"), std::runtime_error);
}