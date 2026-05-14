#include <iostream>
#include <sstream>
#include <stdexcept>
#include <streambuf>
#include <string>

#include <gtest/gtest.h>

#include "WarningLevel.h"

class WarningLevelTest : public ::testing::Test {
protected:
  void SetUp() override {
    AnalysisCore::set_warning_level(AnalysisCore::WarningLevel::WARN);
    old_clog_buf = std::clog.rdbuf(test_stream.rdbuf());
  }

  void TearDown() override {
    std::clog.rdbuf(old_clog_buf);
    AnalysisCore::set_warning_level(AnalysisCore::WarningLevel::WARN);
    test_stream.str("");
    test_stream.clear();
  }
  std::ostringstream test_stream;         // NOLINT
  std::streambuf *old_clog_buf = nullptr; // NOLINT
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
  EXPECT_NO_THROW(
      AnalysisCore::handle_warning("This message should not appear"));
  EXPECT_TRUE(test_stream.str().empty());
}

TEST_F(WarningLevelTest, HandleWarningWarn) {
  AnalysisCore::set_warning_level(AnalysisCore::WarningLevel::WARN);
  EXPECT_NO_THROW(AnalysisCore::handle_warning("Warning message"));
  EXPECT_NE(test_stream.str().find("Warning message"), std::string::npos);
}

TEST_F(WarningLevelTest, HandleWarningError) {
  AnalysisCore::set_warning_level(AnalysisCore::WarningLevel::ERROR);
  EXPECT_THROW(AnalysisCore::handle_warning("Test error"), std::runtime_error);
}