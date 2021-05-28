#include <iostream>
#include <cmath>
#include <csvread.h>
#include <simdasciicheck.h>

#define ID   0
#define NAME 1
#define DESC 2
#define TEXT 3

bool isASCII(const std::string& str) {
#ifdef __AVX2__
	return validate_ascii_fast_avx(str.data(), str.size());
#else
	return validate_ascii_fast(str.data(), str.size());
#endif
}

int main(void) {
	io::LineReader lr("STDIN", stdin);
	const char* line = lr.next_line();

	while (line) {
		auto row = readRow(line);
		// Don't count invalid rows or retweets
		if ((row.size() > TEXT) && (row[TEXT].rfind("RT ", 0) != 0) && (isASCII(row[TEXT]))) {
			std::cout << line << "\n";
		}
		line = lr.next_line();
	}
	
	std::cout << std::flush;
		
	return 0;
}
