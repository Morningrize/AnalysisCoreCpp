#include <initializer_list>
#include <stdexcept>
#include <string>
#include <vector>

#include <gtest/gtest.h>

#include "InputParser.h"
#include "TemporaryDirectory.h"

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

class InputParserConfigTest : public ::testing::Test {
protected:
  AnalysisCore::InputParser parser{"Fake program description",
                                   {.require_config = true}};
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

TEST_F(InputParserTest, CustomOptionsAdded) {
  FakeArgs args{"program_name", "--verbose"};
  parser.options().add_options()("verbose,v", "Enable verbose output");
  parser.parse(args.argc(), args.argv.data());
  EXPECT_EQ(parser.result().count("verbose"), 1);
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

TEST_F(InputParserTest, ThrowsIfNoFilesProvided) {
  FakeArgs args{"program_name"};
  parser.parse(args.argc(), args.argv.data());
  EXPECT_THROW(static_cast<void>(parser.get_input_files()),
               std::invalid_argument);
}

TEST_F(InputParserTest, ReturnsFilesFromListFlag) {
  const TemporaryDirectory tmp_dir;
  const auto tmp_file =
      tmp_dir.create_file("file_list.txt", "file1.root\nfile2.root\n");
  FakeArgs args{"program_name", "--list", tmp_file.string()};
  parser.parse(args.argc(), args.argv.data());
  EXPECT_EQ(parser.get_input_files().size(), 2U);
}

TEST_F(InputParserTest, ExtensionWithoutDirectoryIsIgnored) {
  FakeArgs args{"program_name", "--files", "file1.root", "--extension",
                ".root"};
  parser.parse(args.argc(), args.argv.data());
  EXPECT_EQ(parser.get_input_files().size(), 1U);
}

TEST_F(InputParserTest, ReturnsFilesFromDirectoryFlag) {
  const TemporaryDirectory tmp_dir;
  tmp_dir.create_file("file1.root");
  tmp_dir.create_file("file2.root");
  FakeArgs args{"program_name", "--directory", tmp_dir.path().string()};
  parser.parse(args.argc(), args.argv.data());
  EXPECT_EQ(parser.get_input_files().size(), 2U);
}

TEST_F(InputParserTest, ReturnsFilesFromDirectoryMatchingExtension) {
  const TemporaryDirectory tmp_dir;
  tmp_dir.create_file("file1.root");
  tmp_dir.create_file("file2.txt");
  FakeArgs args{"program_name", "--directory", tmp_dir.path().string(),
                "--extension", ".root"};
  parser.parse(args.argc(), args.argv.data());
  EXPECT_EQ(parser.get_input_files().size(), 1U);
}

TEST_F(InputParserTest, ReturnsFilesFromVariousSources) {
  const TemporaryDirectory tmp_dir;
  tmp_dir.create_file("file1.root");
  tmp_dir.create_file("file2.root");
  const auto file_list =
      tmp_dir.create_file("file_list.txt", "file3.root\nfile4.root\n ");
  FakeArgs args{"program_name",     "--directory", tmp_dir.path().string(),
                "--extension",      ".root",       "--list",
                file_list.string(), "--files",     "file5.root"};
  parser.parse(args.argc(), args.argv.data());
  EXPECT_EQ(parser.get_input_files().size(), 5U);
}

TEST_F(InputParserTest, DeduplicatesFilesAcrossSources) {
  const TemporaryDirectory tmp_dir;
  const auto file_list = tmp_dir.create_file("file_list.txt", "file1.root\n");
  FakeArgs args{"program_name", "--list", file_list.string(), "--files",
                "file1.root"};
  parser.parse(args.argc(), args.argv.data());
  EXPECT_EQ(parser.get_input_files().size(), 1U);
}

TEST_F(InputParserTest, ConfigOptionalByDefault) {
  FakeArgs args{"program_name"};
  parser.parse(args.argc(), args.argv.data());
  EXPECT_EQ(parser.result().count("config"), 0U);
}

TEST_F(InputParserConfigTest, ThrowsForRequiredConfigMissing) {
  FakeArgs args{"program_name"};
  EXPECT_THROW(parser.parse(args.argc(), args.argv.data()),
               std::invalid_argument);
}

TEST_F(InputParserConfigTest, ThrowsForEmptyStringConfig) {
  FakeArgs args{"program_name", "--config", ""};
  EXPECT_NO_THROW(parser.parse(args.argc(), args.argv.data()));
  EXPECT_THROW(static_cast<void>(parser.get_config_file()),
               std::invalid_argument);
}

TEST_F(InputParserConfigTest, HelpRequestedMissingRequiredConfig) {
  FakeArgs args{"program_name", "--help"};
  ASSERT_NO_THROW(parser.parse(args.argc(), args.argv.data()));
  EXPECT_TRUE(parser.help_requested());
}

TEST_F(InputParserConfigTest, ParsesRequiredConfigProvided) {
  FakeArgs args{"program_name", "--config", "config.toml"};
  ASSERT_NO_THROW(parser.parse(args.argc(), args.argv.data()));
  EXPECT_EQ(parser.result()["config"].as<std::string>(), "config.toml");
}

TEST_F(InputParserConfigTest, ThrowsGetConfigFileBeforeParse) {
  EXPECT_THROW(static_cast<void>(parser.get_config_file()), std::runtime_error);
}

TEST_F(InputParserConfigTest, GetConfigReturnsConfigFile) {
  FakeArgs args{"program_name", "--config", "config.toml"};
  parser.parse(args.argc(), args.argv.data());
  EXPECT_EQ(parser.get_config_file(), "config.toml");
}
