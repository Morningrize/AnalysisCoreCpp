
#include <filesystem>
#include <fstream>
#include <initializer_list>
#include <stdexcept>
#include <string>
#include <sys/stat.h>
#include <vector>

#include <gtest/gtest.h>

#include "InputParser.h"

namespace {

struct FakeArgs {
  std::vector<std::string> args;
  std::vector<char *> argv;
  FakeArgs(std::initializer_list<std::string> string_list) : args(string_list) {
    for (auto &string : args) {
      argv.push_back(string.data());
    }
  }
  [[nodiscard]] int argc() const { return static_cast<int>(argv.size()); }
};

class InputParserTest : public ::testing::Test {
protected:
  AnalysisCore::InputParser parser{"Fake program description"};
};

class InputParserDirectoryTest : public ::testing::Test {
protected:
  AnalysisCore::InputParser parser{"Fake program description"}; // NOLINT
  void SetUp() override {
    tmp_dir = std::filesystem::temp_directory_path() / "test_list_directory";
    std::filesystem::create_directory(tmp_dir);
  }
  void TearDown() override { std::filesystem::remove_all(tmp_dir); }
  std::filesystem::path tmp_dir; // NOLINT
};

} // namespace

TEST_F(InputParserTest, OptionsAccessibleBeforeParse) {
  EXPECT_NO_THROW(static_cast<void>(parser.options()));
}

TEST_F(InputParserTest, ThrowsOptionsAfterParse) {
  FakeArgs args{"program_name"};
  parser.parse(args.argc(), args.argv.data());
  EXPECT_THROW(static_cast<void>(parser.options()), std::runtime_error);
}

TEST_F(InputParserTest, ThrowsOnDoubleParse) {
  FakeArgs args{"program_name"};
  parser.parse(args.argc(), args.argv.data());
  EXPECT_THROW(parser.parse(args.argc(), args.argv.data()), std::runtime_error);
}

TEST_F(InputParserTest, ThrowsHelpRequestedBeforeParse) {
  EXPECT_THROW(static_cast<void>(parser.help_requested()), std::runtime_error);
  EXPECT_THROW(static_cast<void>(parser.help_text()), std::runtime_error);
}

TEST_F(InputParserTest, ThrowsGetInputFilesBeforeParse) {
  EXPECT_THROW(static_cast<void>(parser.get_input_files()), std::runtime_error);
}

TEST_F(InputParserTest, ThrowsResultBeforeParse) {
  EXPECT_THROW(static_cast<void>(parser.result()), std::runtime_error);
}

TEST_F(InputParserTest, ResultAccessibleAfterParse) {
  FakeArgs args{"program_name"};
  parser.parse(args.argc(), args.argv.data());
  EXPECT_NO_THROW(static_cast<void>(parser.result()));
}

TEST_F(InputParserTest, HelpRequestedReturnsFalseWithoutHelpFlag) {
  FakeArgs args{"program_name"};
  parser.parse(args.argc(), args.argv.data());
  EXPECT_FALSE(parser.help_requested());
}

TEST_F(InputParserTest, HelpRequestedReturnsTrueWithHelpFlag) {
  FakeArgs args{"program_name", "--help"};
  parser.parse(args.argc(), args.argv.data());
  EXPECT_TRUE(parser.help_requested());
}

TEST_F(InputParserTest, HelpRequestedText) {
  FakeArgs args{"program_name", "--help"};
  parser.parse(args.argc(), args.argv.data());
  EXPECT_NE(parser.help_text().find("Fake program description"),
            std::string::npos);
}

TEST_F(InputParserTest, ReturnsFilesFromFilesFlag) {
  FakeArgs args{"program_name", "--files", "file1.root,file2.root"};
  parser.parse(args.argc(), args.argv.data());
  EXPECT_EQ(parser.get_input_files().size(), 2U);
}

TEST_F(InputParserTest, DeduplicatesFiles) {
  FakeArgs args{"program_name", "--files", "file1.root,file1.root"};
  parser.parse(args.argc(), args.argv.data());
  EXPECT_EQ(parser.get_input_files().size(), 1U);
}

TEST_F(InputParserTest, ReturnsEmptyIfNoFilesProvided) {
  FakeArgs args{"program_name"};
  parser.parse(args.argc(), args.argv.data());
  EXPECT_TRUE(parser.get_input_files().empty());
}

TEST_F(InputParserTest, ReturnsFilesFromListFlag) {
  const std::filesystem::path tmp_file =
      std::filesystem::temp_directory_path() / "file_list.txt";
  std::ofstream file_stream(tmp_file);
  file_stream << "file1.root\nfile2.root\n";
  file_stream.close();
  FakeArgs args{"program_name", "--list", tmp_file.string()};
  parser.parse(args.argc(), args.argv.data());
  EXPECT_EQ(parser.get_input_files().size(), 2U);
  std::filesystem::remove(tmp_file);
}

TEST_F(InputParserTest, ExtensionWithoutDirectoryIsIgnored) {
  FakeArgs args{"program_name", "--files", "file1.root", "--extension",
                ".root"};
  parser.parse(args.argc(), args.argv.data());
  EXPECT_EQ(parser.get_input_files().size(), 1U);
}

TEST_F(InputParserDirectoryTest, ReturnsFilesFromDirectoryFlag) {
  std::ofstream(tmp_dir / "file1.root").close();
  std::ofstream(tmp_dir / "file2.root").close();
  FakeArgs args{"program_name", "--directory", tmp_dir.string()};
  parser.parse(args.argc(), args.argv.data());
  EXPECT_EQ(parser.get_input_files().size(), 2U);
}

TEST_F(InputParserDirectoryTest, ReturnsFilesFromDirectoryMatchingExtension) {
  std::ofstream(tmp_dir / "file1.root").close();
  std::ofstream(tmp_dir / "file2.txt").close();
  FakeArgs args{"program_name", "--directory", tmp_dir.string(), "--extension",
                ".root"};
  parser.parse(args.argc(), args.argv.data());
  EXPECT_EQ(parser.get_input_files().size(), 1U);
}

TEST_F(InputParserDirectoryTest, ReturnsFilesFromVariousSources) {
  std::ofstream(tmp_dir / "file1.root").close();
  std::ofstream(tmp_dir / "file2.root").close();
  std::ofstream file_list(tmp_dir / "file_list.txt");
  file_list << "file3.root\nfile4.root\n ";
  file_list.close();
  FakeArgs args{"program_name",
                "--directory",
                tmp_dir.string(),
                "--extension",
                ".root",
                "--list",
                (tmp_dir / "file_list.txt").string(),
                "--files",
                "file5.root"};
  parser.parse(args.argc(), args.argv.data());
  EXPECT_EQ(parser.get_input_files().size(), 5U);
}

TEST_F(InputParserDirectoryTest, DeduplicatesFilesAcrossSources) {
  std::ofstream file_list(tmp_dir / "file_list.txt");
  file_list << "file1.root\n";
  file_list.close();
  FakeArgs args{"program_name", "--list", (tmp_dir / "file_list.txt").string(),
                "--files", "file1.root"};
  parser.parse(args.argc(), args.argv.data());
  EXPECT_EQ(parser.get_input_files().size(), 1U);
}
