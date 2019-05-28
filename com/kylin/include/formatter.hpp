#ifndef COM_KYLIN_INCLUDE_FORMATTER_HPP_
#define COM_KYLIN_INCLUDE_FORMATTER_HPP_

#include <string>
#include <vector>
#include <sstream>
#include <iostream>

class formatter {
private:
	std::string input, output;
	std::vector<std::string> input_lines, exclusions;
public:
	formatter(const std::string&, const std::vector<std::string>&);
	virtual ~formatter();

	void set(const std::string&, const std::vector<std::string>&);
	std::string fetch();
	std::vector<std::string> fetch_lines();
	void format();
};

#endif
