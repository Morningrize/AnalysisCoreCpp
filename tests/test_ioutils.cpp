#include <filesystem>
#include <stdexcept>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "TemporaryDirectory.h"
#include "io_utils.h"

class IoUtilsTest : public ::testing::Test {
protected:
  TemporaryDirectory tmp_dir;
};

// ----------------------
// trim
// ----------------------

TEST(TrimStringTest, TrimsLeadingTrailingWhitespace) {
  const std::string test_string = "\t I need a trim ";
  EXPECT_EQ(AnalysisCore::trim(test_string), "I need a trim");
}

TEST(TrimStringTest, WhitespaceOnlyTrimsToEmpty) {
  const std::string test_string = "\t \n \t";
  EXPECT_TRUE(AnalysisCore::trim(test_string).empty());
}

TEST(TrimStringTest, TrimEmptyString) {
  const std::string test_string;
  EXPECT_TRUE(AnalysisCore::trim(test_string).empty());
}

// ----------------------
// parse_lines
// ----------------------

TEST_F(IoUtilsTest, ThrowsForMissingFile) {
  EXPECT_THROW(
      AnalysisCore::parse_lines((tmp_dir.path() / "missing.txt").string()),
      std::runtime_error);
}

TEST_F(IoUtilsTest, ReturnsEmptyForEmptyFile) {
  const auto tmp_file = tmp_dir.create_file("test_parse_lines.txt");
  EXPECT_TRUE(AnalysisCore::parse_lines(tmp_file.string()).empty());
}

TEST_F(IoUtilsTest, ReturnsLinesFromFile) {
  const auto tmp_file =
      tmp_dir.create_file("test_parse_lines.txt", "line1\nline2\nline3\n");
  const auto lines = AnalysisCore::parse_lines(tmp_file.string());
  EXPECT_EQ(lines.size(), 3U);
  EXPECT_EQ(lines[0], "line1");
  EXPECT_EQ(lines[2], "line3");
}

TEST_F(IoUtilsTest, SkipsEmptyLines) {
  const auto tmp_file =
      tmp_dir.create_file("test_parse_lines.txt", "line1\n\nline2\n");
  const auto lines = AnalysisCore::parse_lines(tmp_file.string());
  EXPECT_EQ(lines.size(), 2U);
  EXPECT_EQ(lines[1], "line2");
}

TEST_F(IoUtilsTest, TrimsLeadingTrailingWhitespace) {
  const auto tmp_file =
      tmp_dir.create_file("test_parse_lines.txt", "\tline1\n\nline2 \n \n");
  const auto lines = AnalysisCore::parse_lines(tmp_file.string());
  EXPECT_EQ(lines.size(), 2U);
  EXPECT_EQ(lines[0], "line1");
  EXPECT_EQ(lines[1], "line2");
}

// ----------------------
// list_directory
// ----------------------

TEST_F(IoUtilsTest, ThrowsForMissingDirectory) {
  EXPECT_THROW(
      AnalysisCore::list_directory((tmp_dir.path() / "missing").string()),
      std::runtime_error);
}

TEST_F(IoUtilsTest, ReturnsEmptyForEmptyDirectory) {
  EXPECT_TRUE(AnalysisCore::list_directory(tmp_dir.path().string()).empty());
}

TEST_F(IoUtilsTest, ReturnsEmptyWhenNoMatchingExtension) {
  tmp_dir.create_file("file1.txt");
  EXPECT_TRUE(
      AnalysisCore::list_directory(tmp_dir.path().string(), ".root").empty());
}

TEST_F(IoUtilsTest, ListsAllExtensionUnspecified) {
  tmp_dir.create_file("file1.txt");
  tmp_dir.create_file("file2.root");

  const auto files = AnalysisCore::list_directory(tmp_dir.path().string());
  EXPECT_EQ(files.size(), 2U);
}

TEST_F(IoUtilsTest, FiltersFilesByExtension) {
  tmp_dir.create_file("file1.txt");
  const auto root_file = tmp_dir.create_file("file2.root");

  const auto files =
      AnalysisCore::list_directory(tmp_dir.path().string(), ".root");
  EXPECT_EQ(files, std::vector<std::string>{root_file.string()});
}