#ifndef TESTS_TEMPORARYDIRECTORY_H
#define TESTS_TEMPORARYDIRECTORY_H

#include <cerrno>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <string>
#include <system_error>

#include <gtest/gtest.h>

class TemporaryDirectory {
public:
  TemporaryDirectory() {
    auto pattern =
        (std::filesystem::temp_directory_path() / "analysiscore_test_XXXXXX")
            .string();
    if (::mkdtemp(pattern.data()) == nullptr) {
      throw std::system_error(errno, std::generic_category(),
                              "Could not create temporary directory");
    }
    path_ = pattern;
  }

  ~TemporaryDirectory() {
    std::error_code error;
    std::filesystem::remove_all(path_, error);
    if (error) {
      ADD_FAILURE() << "Could not remove temporary directory " << path_ << ": "
                    << error.message();
    }
  }

  TemporaryDirectory(const TemporaryDirectory &) = delete;
  TemporaryDirectory &operator=(const TemporaryDirectory &) = delete;
  TemporaryDirectory(TemporaryDirectory &&) = delete;
  TemporaryDirectory &operator=(TemporaryDirectory &&) = delete;

  [[nodiscard]] const std::filesystem::path &path() const { return path_; }
  // NOLINTNEXTLINE(modernize-use-nodiscard)
  std::filesystem::path create_file(const std::string &name,
                                    const std::string &contents = "") const {
    const auto file_path = path_ / name;
    std::ofstream file;
    file.exceptions(std::ios::failbit | std::ios::badbit);
    file.open(file_path);
    file << contents;
    file.close();
    return file_path;
  }

private:
  std::filesystem::path path_;
};

#endif
