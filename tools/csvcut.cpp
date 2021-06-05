#include <iostream>
#include <csvread.h>
#define OPTPARSE_IMPLEMENTATION
#include <optparse.h>
#define PRINT_LINENUM 0xFF

int main(int argc, char **argv) {
	io::LineReader lr("STDIN", stdin);
	const char* line = lr.next_line();
	size_t max_col = 0;
	std::vector<int> columns;
	uint64_t i = 0;

	int option;
	struct optparse options;

	optparse_init(&options, argv);
	while ((option = optparse(&options, "0123456789n")) != -1) {
		if (option == 'n') {
			columns.emplace_back(PRINT_LINENUM);
		} else if (option == '?') {
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
				if (columns[i] == PRINT_LINENUM) {
					std::cout << i << ",";
				} else {
					std::cout << "\"" << row[columns[i]] << "\",";
				}
			}
			if (columns.back() == PRINT_LINENUM) {
				std::cout << i << "\n";
			} else {
				std::cout << "\"" << row[columns.back()] << "\"\n";
			}
		}
		line = lr.next_line();
		i++;
	}
	
	std::cout << std::flush;
		
	return 0;
}
