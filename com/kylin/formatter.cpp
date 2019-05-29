#include "include/formatter.hpp"

formatter::formatter(const std::string &input, const std::vector<std::string> &exclusions) {
	this->input = input;
	this->exclusions = exclusions;
}

formatter::~formatter() {

}

void formatter::set(const std::string &input, const std::vector<std::string> &exclusions) {
	this->input.clear();
	this->input = input;
	this->exclusions.clear();
	this->exclusions = exclusions;
}

std::string formatter::fetch() {
	return output;
}

std::vector<std::string> formatter::fetch_lines() {
	return input_lines;
}

void formatter::format() {
	input_lines.clear();
	output.clear();

	std::istringstream sstmp(input);
	std::string stmp;
	while (std::getline(sstmp, stmp, '\n')) {
		if (stmp.size() > 0 && stmp[0] != '#')
			input_lines.push_back(stmp);
		stmp.clear();
	}

	std::size_t index;
	for (unsigned int i = 0; i < input_lines.size(); i++) {
		if (exclusions.size() > 0)
			for (unsigned int j = 0; j < exclusions.size(); j++)
				while ((index = input_lines.at(i).find(exclusions.at(j))) != std::string::npos)
					input_lines.at(i).erase(index, exclusions.at(j).size());
		while ((index = input_lines.at(i).find(' ')) != std::string::npos)
			input_lines.at(i)[index] = '+';
		while ((index = input_lines.at(i).find("++")) != std::string::npos)
			input_lines.at(i).erase(index);
		if (input_lines.at(i).find("http") == std::string::npos && input_lines.at(i).find("www") == std::string::npos)
			output += "https://www.google.com/search?q=";
		output += input_lines.at(i);
		output += ' ';
	}
}

