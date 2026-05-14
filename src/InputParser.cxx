#include "InputParser.h"

#include <memory>
#include <string>
#include <utility>
#include <vector>
#include <unordered_set>
#include <stdexcept>

#include "cxxopts.hpp"

#include "io_utils.h"

namespace AnalysisCore{

    InputParser::InputParser(std::string description) 
        : description_(std::move(description)),
          options_(std::make_unique<cxxopts::Options>("", description_)) {}
    
    InputParser::~InputParser() = default;

    cxxopts::Options& InputParser::options() {
        if (result_) {
            throw std::runtime_error("Cannot modify options after parse()");
        } 
        return *options_;
    }

    void InputParser::parse(int argc, char **argv) {
        if (result_) {
            throw std::runtime_error("parse() has already been called");
        }
        options_ = std::make_unique<cxxopts::Options>(*argv, description_);
        options_->add_options()
            ("f,files", "Input files separated by spaces", cxxopts::value<std::vector<std::string>>())
            ("l,list", "List of file names stored in a text file", cxxopts::value<std::string>())
            ("d,directory", "Directory holding input files", cxxopts::value<std::string>())
            ("e,extension","Filter files in directory by extension, used in conjuction with the --d option",cxxopts::value<std::string>())
            ("h,help",      "Print usage");

        result_ = std::make_unique<cxxopts::ParseResult>(options_->parse(argc, argv));
    }

    bool InputParser::help_requested() const {
        if (!result_) {
            throw std::runtime_error("parse() must be called first");
        }
        return result_->count("help") != 0U;
    }

    std::string InputParser::help_text() const {
        if (!result_) {
            throw std::runtime_error("parse() must be called first");
        }
        return options_->help();
    }

    std::vector<std::string> InputParser::get_input_files() const {
        if (!result_) {
            throw std::runtime_error("parse() must be called first");
        }
      std::vector<std::string> file_names;
      if (result_->count("files") != 0U) {
        const auto& files = (*result_)["files"].as<std::vector<std::string>>();
        file_names.insert(file_names.end(), files.begin(), files.end());
      }
      if (result_->count("list") != 0U) {
        const auto& list_file = (*result_)["list"].as<std::string>();
        const auto& files_from_list = parse_lines(list_file);
        file_names.insert(file_names.end(), files_from_list.begin(),
                        files_from_list.end());
      }
      if (result_->count("directory") != 0U) {
        const auto& dir_name = (*result_)["directory"].as<std::string>();
        std::string extension;
        if (result_->count("extension") != 0U ){
            extension = (*result_)["extension"].as<std::string>();
        }
        const auto& files_from_dir = list_directory(dir_name, extension);
        file_names.insert(file_names.end(), files_from_dir.begin(),
                          files_from_dir.end());
      }
      std::vector<std::string> unique_file_names;
      std::unordered_set<std::string> seen;
      for (const auto& file_name : file_names){
        if (seen.insert(file_name).second){
            unique_file_names.push_back(file_name);
        }
      }

    return unique_file_names;
    }

    const cxxopts::ParseResult& InputParser::result() const {
        if (!result_) {
            throw std::runtime_error("parse() must be called first");
        }
        return *result_;
    }
}