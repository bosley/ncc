/*
MIT License

Copyright (c) 2023 bosley

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
*/

#ifndef ARGER_HPP
#define ARGER_HPP

#include <functional>
#include <iostream>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

namespace args {

//! \brief An argument parser
class arger_c {
public:
	//! \brief Add an argument
	//! \param name The name of the argument
	//! \param description The description of the argument
	//! \param default_value The default value of the argument
	//! \param required Whether the argument is required (default: false)
  void add_arg(const std::string name, const std::string description,
               const std::string default_value, const bool required = false) {
    insert_arg(arg_c{name, description, default_value,
                     (required) ? std::optional<bool>(false) : std::nullopt,
                     false});
  }

	//! \brief Add a boolean flag argument
	//! \param name The name of the argument
	//! \param description The description of the argument
	//! \param default_value The default value of the argument (default: false)
  void add_flag(const std::string name, const std::string description,
                const bool default_value, const bool required = false) {
    insert_arg(arg_c{name, description, std::to_string(default_value),
                     (required) ? std::optional<bool>(false) : std::nullopt,
                     true});
  }

	//! \brief Parse the arguments
	//! \param argc The number of arguments
	//! \param argv The arguments
	//! \return Whether the arguments were parsed successfully
	//! \note This function will print an error message if the arguments were not
	//! parsed successfully
  bool parse(int argc, char **argv) {
    std::vector<std::string> arguments(argv, argv + argc);
    program_name_ = arguments[0];
    for (std::size_t i = 1; i < arguments.size(); ++i) {
      for (auto &&da : declared_args_) {
        if (da.name == arguments[i]) {
          if (!((da.is_flag) ? handle_flag(da)
                             : handle_arg(da, arguments, i))) {
            return false;
          }
        }
      }
    }
    return true;
  }

	bool validate_required_args() {
		for (auto &&da : declared_args_) {
			if (da.found.has_value() && !da.found.value()) {
				std::cerr << "Error: Argument " << da.name << " is required"
					<< std::endl;
				return false;
			}
		}
		return true;
	}

	//! \brief Get the name of the program
	//! \return The name of the program
  const char *get_program_name() { return program_name_.c_str(); }

	//! \brief Print the help message
	//! \note This function will print the help message to stdout
  void print_help() {
    std::cout << "Usage: " << program_name_ << " [options]" << std::endl;
    std::cout << "Options:" << std::endl;
    for (auto &&da : declared_args_) {
      std::cout << "  " << da.name << " " << da.description << " "
                << ((da.found.has_value()) ? "<required>" : "<optional>")
                << "\n" << std::endl;
    }
  }

	//! \brief Get the value of an argument
	//! \tparam T The type of the argument
	//! \param name The name of the argument
	//! \return The value of the argument
	template<typename T>
	std::optional<T> get_arg (const std::string name) {
		if (parsed_args_.find(name) == parsed_args_.end()) {
			return std::nullopt;
		}
    std::stringstream convert(parsed_args_[name]);
    T value;
    convert >> value;
    return value;
	}

	//! \brief dump the parsed arguments
	//! \note This function will print the parsed arguments to stdout
  void dump() {
    std::cout << "Dumping parsed args:" << std::endl;
    for (auto &&da : parsed_args_) {
      std::cout << da.first << " = " << da.second << std::endl;
    }
  }

private:
  struct arg_c {
    std::string name;
    std::string description;
		std::string value;
    std::optional<bool> found{std::nullopt};
    bool is_flag{false};
  };

  std::string program_name_;
  std::vector<arg_c> declared_args_;
  std::unordered_map<std::string, std::string> parsed_args_;

  inline void insert_arg(arg_c arg) {
		declared_args_.push_back(arg);

		// Add as the default
		parsed_args_[arg.name] = arg.value;
	}

  inline bool handle_flag(arg_c &da) {
    parsed_args_[da.name] = "1";
    conditionally_mark_found(da);
    return true;
  }

  inline bool handle_arg(arg_c &da, const std::vector<std::string> &arguments,
                         std::size_t &index) {
    if (index + 1 >= arguments.size()) {
      std::cerr << "Error: Argument " << da.name << " requires a value"
                << std::endl;
      return false;
    }

    auto &value = arguments[index + 1];
    parsed_args_[da.name] = value;
    index += 1;

    conditionally_mark_found(da);
    return true;
  }

  inline void conditionally_mark_found(arg_c &da) {
    // found won't store a value if the arg is not required
    // if it is required, it will store a value and we set it to true now
    if (da.found.has_value()) {
      da.found = true;
    }
  }
};

// arger.add_arg<T>
} // namespace args

#endif
