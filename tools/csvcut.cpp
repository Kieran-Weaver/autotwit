#include <iostream>
#include <csvread.h>
#define OPTPARSE_IMPLEMENTATION
#include <optparse.h>

int main(int argc, char **argv) {
	io::LineReader lr("STDIN", stdin);
	const char* line = lr.next_line();
	size_t max_col = 0;
	std::vector<int> columns;

	// Parse options: -p prefix
	int option;
	struct optparse options;

	optparse_init(&options, argv);
	while ((option = optparse(&options, "0123456789")) != -1) {
		if (option == '?') {
			std::cerr << argv[0] << ": " << options.errmsg;
		} else {
			columns.emplace_back(option - '0'); // Convert to int
			if (max_col < static_cast<size_t>(option - '0')) {
				max_col = option - '0';
			}
		}
	}

	while (line) {
		auto row = readRow(line);
		if (row.size() > max_col) {
			for (size_t i = 0; i < (columns.size() - 1); i++) {
				std::cout << "\"" << row[columns[i]] << "\",";
			}
			std::cout << "\"" << row[columns.back()] << "\"\n";
		}
		line = lr.next_line();
	}
	
	std::cout << std::flush;
		
	return 0;
}
