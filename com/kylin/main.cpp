#include <iostream>
#include <fstream>
#include <cstring>

#include "include/settings.hpp"
#include "include/formatter.hpp"

std::string input_file_name, exclusion_file_name, output_file_name;
std::vector<std::string> exclusions;
bool print_links = false;
int browser = -1;

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
			i++;
		} else if (strcmp("-v", argv[i]) == 0 || strcmp("--verbose", argv[i]) == 0) {
			print_links = true;
		} else if (strcmp("-o", argv[i]) == 0 || strcmp("--output", argv[i]) == 0) {
			if (i + 1 >= argc) {
				std::cout << "Not enough parameters provided for command \'" << argv[i] << "\'!" << std::endl;
				return PROG_ERROR_PARAM;
			}
			output_file_name = argv[i + 1];
			i++;
		} else if (strcmp("-b", argv[i]) == 0 || strcmp("--browser", argv[i]) == 0) {
			if (i + 1 >= argc) {
				std::cout << "Not enough parameters provided for command \'" << argv[i] << "\'!" << std::endl;
				return PROG_ERROR_PARAM;
			}
			for (unsigned int j = 0; j < strlen(argv[i + 1]); j++)
				if (argv[i + 1][j] < 97)
					argv[i + 1][j] += 32;
			if (strcmp(argv[i + 1], "firefox") == 0) {
				browser = 0;
			} else if (strcmp(argv[i + 1], "chrome") == 0) {
				browser = 1;
			} else {
				std::cout << "Browser \'" << argv[i + 1] << "\' not supported!" << std::endl;
				return PROG_ERROR_NO_BROWSER;
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

	if (browser == -1) {
		std::cout << "No browser specified!" << std::endl;
		return PROG_ERROR_NO_BROWSER;
	}

	if (exclusion_file_name.size() > 0) {
		std::ifstream exclusions_file(exclusion_file_name, std::ios::in);
		if (exclusions_file.is_open()) {
			std::string tmp;
			while (std::getline(exclusions_file, tmp)) {
				if (exclusions_file.fail()) {
					std::cout << "Error opening file \'" << input_file_name << "\'!" << std::endl;
					return PROG_ERROR_FILE_IO;
				}
				exclusions.push_back(tmp);
			}
			exclusions_file.close();
		} else {
			std::cout << "Error opening file \'" << exclusion_file_name << "\'!" << std::endl;
			return PROG_ERROR_FILE_IO;
		}
	} else {
		std::cout << "No exclusion file specified. Running without exclusions..." << std::endl;
	}

	std::string buffer;
	std::ifstream input_file(input_file_name, std::ios::in);
	if (input_file.is_open()) {
		std::string tmp;
		while (std::getline(input_file, tmp, '\n')) {
			if (input_file.fail()) {
				std::cout << "Error reading file \'" << input_file_name << "\'!" << std::endl;
				return PROG_ERROR_FILE_IO;
			}
			buffer += tmp;
			buffer += '\n';
		}
		input_file.close();
	} else {
		std::cout << "Error opening file \'" << input_file_name << "\'!" << std::endl;
		return PROG_ERROR_FILE_IO;
	}

	if (buffer.size() == 0) {
		std::cout << "Empty input file. Exiting..." << std::endl;
		return PROG_ERROR_INPUT_EMPTY;
	}

	formatter form(buffer, exclusions);
	form.format();

	if (print_links || output_file_name.size() > 0) {
		std::string output;
		std::vector<std::string> lines = form.fetch_lines();
		if (print_links)
			std::cout << "LINKS:" << std::endl;
		for (unsigned int i = 0; i < lines.size(); i++) {
			std::size_t index;
			while ((index = lines.at(i).find('+', 0)) != std::string::npos)
				lines.at(i)[index] = ' ';
			output += lines.at(i);
			output += '\n';
			if (print_links)
				std::cout << "  " << lines.at(i) << std::endl;
		}
		if (output_file_name.size() > 0) {
			std::ofstream output_file(output_file_name, std::ios::out);
			if (output_file.is_open()) {
				output_file << output;
				if (output_file.fail()) {
					std::cout << "Error writing to file \'" << output_file_name << "\'!" << std::endl;
					return PROG_ERROR_FILE_IO;
				}
				output_file.close();
			} else {
				std::cout << "Error opening/creating file \'" << output_file_name << "\'!" << std::endl;
				return PROG_ERROR_FILE_IO;
			}
		}
	}
	std::string output;
	if (browser == 0)
		output += "firefox ";
	else if (browser == 1)
		output += "google-chrome ";
	output += form.fetch();
	system(output.c_str());

	return PROG_ERROR_NONE;
}
