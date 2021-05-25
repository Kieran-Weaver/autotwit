#ifndef CSVREAD_H
#define CSVREAD_H

#include <csv.h>

static inline std::vector<std::string> readRow(const char* line) {
	bool escaped = false;
	bool quoted = false;
	std::vector<std::string> cols;
	char c;
	cols.emplace_back();
	
	while ((c = *(line++))) {

		if (escaped) {
			escaped = false;
		} else {
			switch (c) {
			case '\\':
				escaped = true;
				break;
			case '"':
				quoted = !quoted;
				continue;
				break;
			case ',':
				if (!quoted) {
					cols.emplace_back();
					continue;
				}
				break;
			default: break;
			}
		}
		
		cols.back().push_back(c);
	}
	
	return cols;
}

#endif
