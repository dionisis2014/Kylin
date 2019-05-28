#include <iostream>
#include <fstream>
#include <cstring>

#include "include/settings.hpp"
#include "include/formatter.hpp"

std::string input_file_name, exclusion_file_name, output_file_name;
std::vector<std::string> exclusions;
bool print_links = false;

int main(int argc, char **argv) {
	for (int i = 1; i < argc; i++) {
		if (strcmp("-h", argv[i]) == 0 || strcmp("--help", argv[i]) == 0) {
			std::cout << PROG_NAME_LONG << std::endl;
			std::cout << "Version: " << PROG_VERSION << std::endl;
			std::cout << "Usage: Kylin [ options ... ]" << std::endl << std::endl;
			std::cout << " SHORT\t LONG\t\t\t DESCRIPTION" << std::endl;
			std::cout << "  -h\t--help\t\t\tPrints this help message" << std::endl;
			std::cout << "  -if\t--input-file\t\tFile with content to search" << std::endl;
			std::cout << "  -ef\t--exclusion-file\tFile with content to exclude" << std::endl;
			std::cout << "  -v\t--verbose\t\tPrint links to be searched" << std::endl;
			std::cout << "  -o\t--output\t\tOutput formated links to file specified" << std::endl;
			return PROG_ERROR_NONE;
		} else if (strcmp("-if", argv[i]) == 0 || strcmp("--input-file", argv[i]) == 0) {
			if (i + 1 >= argc) {
				std::cout << "Not enough parameters provided for command \'" << argv[i] << "\'!" << std::endl;
				return PROG_ERROR_PARAM;
			}
			input_file_name = argv[i + 1];
			i++;
		} else if (strcmp("-ef", argv[i]) == 0 || strcmp("--exclusion-file", argv[i]) == 0) {
			if (i + 1 >= argc) {
				std::cout << "Not enough parameters provided for command \'" << argv[i] << "\'!" << std::endl;
				return PROG_ERROR_PARAM;
			}
			exclusion_file_name = argv[i + 1];
			output_file_name = argv[i + 1];
			i++;
		} else if (strcmp("-v", argv[i]) == 0 || strcmp("--verbose", argv[i]) == 0) {
			print_links = true;
		} else if (strcmp("-o", argv[i]) == 0 || strcmp("--output", argv[i]) == 0) {
			if (i + 1 >= argc) {
				std::cout << "Not enough parameters provided for command \'" << argv[i] << "\'!" << std::endl;
				return PROG_ERROR_PARAM;
			}
			i++;
		} else {
			std::cout << "Unrecognized command \'" << argv[i] << "\'!" << std::endl;
			return PROG_ERROR_PARAM;
		}
	}

	if (input_file_name.size() == 0) {
		std::cout << "No input file specified!" << std::endl;
		return PROG_ERROR_NO_INPUT;
	}

	if (exclusion_file_name.size() > 0) {
		std::ifstream exclusion_file(exclusion_file_name, std::ios::in);
		if (exclusion_file.is_open()) {
			std::string tmp;
			while (std::getline(exclusion_file, tmp, '\n'))
				exclusions.push_back(tmp);
			exclusion_file.close();
			if (!exclusion_file.eof()) {
				std::cout << "Error reading file: \'" << exclusion_file_name << "\'!" << std::endl;
				return PROG_ERROR_FILE_IO;
			}
		} else {
			std::cout << "Error opening file: \'" << exclusion_file_name << "\'!" << std::endl;
			return PROG_ERROR_FILE_IO;
		}
	} else {
		std::cout << "Executing with no exclusion file specified..." << std::endl;
	}

	std::stringstream buffer;
	std::ifstream input_file(input_file_name, std::ios::in);
	if (input_file.is_open()) {
		buffer << input_file.rdbuf();
		if (input_file.fail()) {
			std::cout << "Error reading file: \'" << input_file_name << "\'!" << std::endl;
			return PROG_ERROR_FILE_IO;
		}
	} else {
		std::cout << "Error opening file: \'" << input_file_name << "\'!" << std::endl;
		return PROG_ERROR_FILE_IO;
	}

	formatter form(buffer.str(), exclusions);
	form.format();

	if (print_links || output_file_name.size() > 0) {
		std::vector<std::string> lines = form.fetch_lines();
		if (print_links)
			std::cout << "LINKS:" << std::endl;
		std::size_t index;
		std::ofstream output_file;
		if (output_file_name.size() > 0) {
			output_file.open(output_file_name, std::ios::out);
			if (!output_file.is_open()) {
				std::cout << "Error opening file: \'" << output_file_name << "\'!" << std::endl;
				return PROG_ERROR_FILE_IO;
			}
		}
		for (unsigned int i = 0; i < lines.size(); i++) {
			while ((index = lines.at(i).find('+')) != std::string::npos)
				lines.at(i)[index] = ' ';
			if (print_links) {
				std::cout << "  " << lines.at(i) << std::endl;
			} else {
				if (output_file.is_open()) {
					output_file << lines.at(i) << std::endl;
				}
				if (!output_file.good()) {
					output_file.close();
					std::cout << "Error writing to file: \'" << output_file_name << "\'!" << std::endl;
					return PROG_ERROR_FILE_IO;
				}
			}
		}
		if (output_file.is_open()) {
			output_file.close();
		}
	}

	//system(form.fetch().c_str());

	return PROG_ERROR_NONE;
}
