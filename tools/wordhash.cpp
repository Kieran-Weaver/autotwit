#include <iostream>
#include <cmath>
#include <unordered_set>
#include <charconv>
#include <xxhash.h>
#include <csvread.h>
#include <simdasciicheck.h>
// simdasciicheck.h from https://github.com/lemire/fastvalidate-utf-8

#define WORD 0
#define FREQ 1

struct idf_t {
	uint64_t hash;
	double   freq;
};

int main(void) {
	io::LineReader lr("STDIN", stdin);
	const char* line = lr.next_line();
	std::unordered_set<XXH64_hash_t> hashes;
	std::freopen(NULL, "wb", stdout);

	while (line) {
		auto row = readRow(line);
		idf_t curr;
		
		curr.hash = XXH3_64bits(row[WORD].data(), row[WORD].size());

		if (hashes.count(curr.hash)) {
			std::cerr << "Fail: Collision found at " << row[WORD] << std::endl;
			return 1;
		}
		
		hashes.insert(curr.hash);
		
		std::from_chars(row[FREQ].data(), row[FREQ].data() + row[FREQ].size(), curr.freq);
		fwrite(&curr, sizeof(curr), 1, stdout);
		
		line = lr.next_line();
	}
	
	std::cerr << "No collisions found" << std::endl;
	return 0;
}
