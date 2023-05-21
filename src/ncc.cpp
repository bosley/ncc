#include <iostream>

#include "arger.hpp"
#include "log/log.hpp"
#include "ncc.hpp"

#include <cstdint>
#include <filesystem>
#include <unordered_map>

namespace ncc {

config_s config;
config_s &get_config() { return config; }

} // namespace ncc


void set_logger(const std::string level) {
	std::unordered_map<std::string, uint8_t> level_map = {
		{"trace", rxi::LOG_TRACE},
		{"debug", rxi::LOG_DEBUG},
		{"info", rxi::LOG_INFO},
		{"warn", rxi::LOG_WARN},
		{"error", rxi::LOG_ERROR},
		{"fatal", rxi::LOG_FATAL},
	};

	if (level_map.find(level) == level_map.end()) {
		std::cerr << "invalid log level: " << level << std::endl;
		std::exit(1);
	}

	rxi::log_set_level(level_map[level]);
}

int main(int argc, char **argv) {

  args::arger_c arger;
  arger.add_flag("-h", "print help", false);
	arger.add_arg("-i", "input file", "", true);
  arger.add_arg("-I", "include directories (';' delim)", "", true);
	arger.add_arg("-o", "output file name", "out.nvm", false);
	arger.add_arg("-l", "log level [trace debug info warn error fatal]", "fatal", false);
	arger.add_flag("-r", "build in release mode", false, false);
  
	if (!arger.parse(argc, argv)) return 1;

	if (*arger.get_arg<bool>("-h")) {
		arger.print_help();
		return 0;
	}

	set_logger(*arger.get_arg<std::string>("-l"));

	if (!arger.validate_required_args()) {
		return 1;
	}

	if (*arger.get_arg<bool>("-r")) {
		ncc::config.build_type = ncc::build_type_e::RELEASE;
		rxi::log_debug("building in release mode");
	}

	if (arger.get_arg<std::string>("-i").has_value()) {
		auto input_file = *arger.get_arg<std::string>("-i");
		std::filesystem::path input_path(input_file);
		if (!std::filesystem::exists(input_path)) {
			rxi::log_fatal("input file does not exist: %s", input_file.c_str());
			return 1;
		}
		if (!std::filesystem::is_regular_file(input_path)) {
			rxi::log_fatal("input file is not a regular file: %s", input_file.c_str());
			return 1;
		}
		ncc::config.target_path = input_path;
		rxi::log_debug("input file: %s", ncc::config.target_path.c_str());
	}

	if (arger.get_arg<std::string>("-o").has_value()) {
		auto output_file = *arger.get_arg<std::string>("-o");
		std::filesystem::path output_path(output_file);
		if (std::filesystem::is_directory(output_file)) {
			rxi::log_fatal("output file already exists, and it is a directory: %s", output_file.c_str());
			return 1;
		}
		ncc::config.output_path = output_path;
		rxi::log_debug("output file: %s", ncc::config.output_path.c_str());
	}

	if (arger.get_arg<std::string>("-I").has_value()) {
		auto include_dirs = *arger.get_arg<std::string>("-I");
		std::string token;
		std::istringstream token_stream(include_dirs);
		while (std::getline(token_stream, token, ';')) {
			std::filesystem::path include_path(token);
			if (!std::filesystem::is_directory(include_path)) {
				rxi::log_fatal("include directory is not a directory: %s", token.c_str());
				return 1;
			}
			ncc::config.include_paths.push_back(std::filesystem::absolute(include_path));
			rxi::log_debug("include directory: %s", ncc::config.include_paths.back().c_str());
		}
	}
  return 0;
}

