#include <iostream>
#include <cmath>
#include <unordered_map>
#include <charconv>
#include <fstream>
#include <vector>
#include <xxhash.h>
#include <csvread.h>
#include <xorfilter.h>
#define OPTPARSE_IMPLEMENTATION
#define OPTPARSE_API static
#include <optparse.h>
#define LEN_MULT 2

#define ID 0
#define NAME 1
#define DESC 2
#define TEXT 3

struct idf_t {
	uint64_t hash;
	double   freq;
};

using idf_map = std::unordered_map<XXH64_hash_t, uint64_t>;

std::unordered_map<std::string, uint32_t> TF(const std::string& data) {
	std::string current;
	auto it = data.begin();
	std::unordered_map<std::string, uint32_t> freq;
	
	while (it != data.end()) {
		char c = std::toupper(*(it++));
		// Unescape \n, \t, and \r
		if (c == '\\') {
			if (it == data.end()) break;
			c = std::toupper(*(it++));
			switch (c) {
				case 'n':
				case 't':
				case 'r':
					if (current.size() != 0) {
						freq[current]++;
					}
					current = "";
					continue;
					break;
				default: break;
			}
		}

		// Skip punctuation that isn't mentions or hashtags
		if (std::ispunct(c) && (c != '@') && (c != '#')) {
			continue;
		}

		// Handle word breaks
		if (std::isspace(c)) {
			if (current.size() != 0) {
				freq[current]++;
			}
			current = "";
		} else current.push_back(c);
	}

	if (current.size() != 0) {
		freq[current]++;
	}
	
	return freq;
}

int main(int argc, char **argv) {
	io::LineReader lr("STDIN", stdin);
	const char* line = lr.next_line();
	idf_map hashes;
	std::ofstream wordsfile, filterfile, mapfile;
	std::string prefix;
	std::vector<idf_t> idf_vect;
	std::vector<uint64_t> hash_vect;
	std::vector<std::string> keys;
	xor8_t filter;
	uint64_t tweets = 0;
	
	// Parse options: -p prefix
	int option;
	struct optparse options;
	
	optparse_init(&options, argv);
	while ((option = optparse(&options, "p:")) != -1) {
		switch (option) {
		case 'p':
			prefix = std::string(options.optarg);
			break;
		case '?':
		default:
			std::cerr << argv[0] << ": " << options.errmsg;
			break;
		}
	}

	wordsfile.open(prefix + "words.txt");
	filterfile.open(prefix + "filter.bin", std::ofstream::binary | std::ofstream::out);
	mapfile.open(prefix + "dict.bin", std::ofstream::binary | std::ofstream::out);

	// Convert tweets to raw IDF map
	while (line) {
		auto row = readRow(line);
		if (row.size() <= 3) {
			line = lr.next_line();
			continue;
		}
		auto tf = TF(row[TEXT]);
		
		for (const auto& tf_pair : tf) {
			auto hash = XXH3_64bits(tf_pair.first.data(), tf_pair.first.size());
			if (hashes.count(hash) == 0) {
				keys.emplace_back(tf_pair.first);
			}
			hashes[hash]++;
		}

		tweets++;
		line = lr.next_line();
	}
	
	// Sort keys by frequency
	std::sort(keys.begin(), keys.end(), [&](const std::string& a, const std::string& b) {
		auto ahash = XXH3_64bits(a.data(), a.size());
		auto bhash = XXH3_64bits(b.data(), b.size());
		return hashes[ahash] > hashes[bhash];
	});
	
	// Remove every word that occurs exactly once in the dataset
	for (const auto& key : keys) {
		auto hash = XXH3_64bits(key.data(), key.size());
		if (hashes[hash] > 1) {
			wordsfile << (key + "\n");
		} else {
			hashes.erase(hash);
		}
	}
	wordsfile << std::flush;
	wordsfile.close();
	
	// Turn raw IDF map into sorted IDF vector and clear it
	for (auto idf_it = hashes.begin(); idf_it != hashes.end(); idf_it = hashes.erase(idf_it)) {
		idf_vect.emplace_back(
			idf_t({idf_it->first, 
			-std::log((double)(idf_it->second) / (double)tweets)})
		);
	}
	
	std::sort(idf_vect.begin(), idf_vect.end(), [](const idf_t& a, const idf_t& b) {
		return std::tie(a.hash, a.freq) < std::tie(b.hash, b.freq);
	});
	
	// Write out IDF vector to file
	mapfile.write(reinterpret_cast<const char*>(idf_vect.data()), idf_vect.size() * sizeof(idf_t));
	mapfile.flush();
	mapfile.close();
	
	// Convert IDF vector to vector of hash values and clear it
	std::transform(idf_vect.begin(), idf_vect.end(), std::back_inserter(hash_vect), [](const idf_t& idf) {
		return idf.hash;
	});
	std::vector<idf_t>().swap(idf_vect);
	
	xor8_allocate(hash_vect.size() * LEN_MULT, &filter);
	if (!xor8_buffered_populate(hash_vect.data(), hash_vect.size(), &filter)) {
		std::cerr << "Duplicate value in XOR filter" << std::endl;
		return 1;
	}
	filterfile.write(reinterpret_cast<const char*>(&(filter.seed)), sizeof(filter.seed));
	filterfile.write(reinterpret_cast<const char*>(&(filter.blockLength)), sizeof(filter.blockLength));
	filterfile.write(reinterpret_cast<const char*>(filter.fingerprints), sizeof(uint8_t) * 3 * filter.blockLength);
	filterfile.flush();
	filterfile.close();
	xor8_free(&filter);

	return 0;
}
