#ifndef INPUTPARSER_H
#define INPUTPARSER_H

#include <memory>
#include <string>
#include <vector>

#include "cxxopts.hpp"

namespace AnalysisCore {

struct InputParserRequiredFlags {
  bool require_config = false;
};

class InputParser {
public:
  explicit InputParser(std::string description,
                       InputParserRequiredFlags flags = {});
  ~InputParser();

  InputParser(const InputParser &) = delete;
  InputParser &operator=(const InputParser &) = delete;
  InputParser(InputParser &&) = delete;
  InputParser &operator=(InputParser &&) = delete;

  void parse(int argc, char **argv);

  [[nodiscard]] cxxopts::Options &options();

  [[nodiscard]] std::vector<std::string> get_input_files() const;

  [[nodiscard]] bool help_requested() const;

  [[nodiscard]] std::string help_text() const;

  [[nodiscard]] const cxxopts::ParseResult &result() const;

  [[nodiscard]] std::string get_config_file() const;

private:
  std::string description_;
  std::unique_ptr<cxxopts::Options> options_;
  std::unique_ptr<cxxopts::ParseResult> result_;
  InputParserRequiredFlags flags_;
};

} // namespace AnalysisCore

#endif