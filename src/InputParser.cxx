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

    InputParser::InputParser(std::string description, InputParserRequiredFlags flags) 
        : description_(std::move(description)),
          flags_(flags),
          options_(std::make_unique<cxxopts::Options>("", description_)) {
            options_->add_options()
                ("f,files", "Input files, comma separated", cxxopts::value<std::vector<std::string>>())
                ("l,list", "List of file names stored in a text file", cxxopts::value<std::string>())
                ("d,directory", "Directory holding input files", cxxopts::value<std::string>())
                ("e,extension","Filter files in directory by extension, used in conjuction with the --d option",cxxopts::value<std::string>())
                ("c,config","Provide a TOML config file", cxxopts::value<std::string>())
                ("h,help", "Print usage");
          }
    
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
        result_ = std::make_unique<cxxopts::ParseResult>(options_->parse(argc, argv));
        if (flags_.require_config && 
            result_->count("help") == 0 &&
            result_->count("config") == 0){
                throw std::invalid_argument("--config is required");
            }         
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

    std::string InputParser::get_config_file() const {
        if (!result_) {
            throw std::runtime_error("parse() must be called first");
        }
        std::string config_file;
        if (result_->count("config") != 0U) {
            config_file = (*result_)["config"].as<std::string>();
            if (config_file.empty()) {
                throw std::invalid_argument("No config file provided");
            }
        }
        return config_file;
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
      if (unique_file_names.empty()) {
        throw std::invalid_argument("No input files provided");
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