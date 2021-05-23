#include <rapidjson/document.h>
#include <rapidjson/encodings.h>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <array>

#define BUF_SIZE 65536

using namespace rapidjson;

void escapestring(const Value& v, std::string& buffer) {
	const char* st = v.GetString();
	const auto len = v.GetStringLength();
	
	for (size_t i = 0; i < len; i++) {
		switch (st[i]) {
			case '\n':  buffer += "\\n"; break;
			case '\r':  buffer += "\\r"; break;
			case '\t':  buffer += "\\t"; break;
			case '\\':  buffer += "\\\\"; break;
			case '\"':  buffer += "\\\""; break;
			default: buffer.push_back(st[i]); break;
		}
	}
}

void writeout(const char* cbuf) {
	Document idoc;
	idoc.Parse(cbuf);
	
	if (!idoc.IsArray()) {
		std::cerr << cbuf << std::endl;
	}
	
	for (auto& v : idoc.GetArray()) {
		if (v.HasMember("text")) {
			std::string buf = ",";
			
			if (!(v["user"]["name"].IsNull())) {
				buf.push_back('"');
				escapestring(v["user"]["name"], buf);
				buf.push_back('"');
			}
			buf.push_back(',');

			if (!(v["user"]["description"].IsNull())) {
				buf.push_back('"');
				escapestring(v["user"]["description"], buf);
				buf.push_back('"');
			}
			buf.push_back(',');
	
			if (!(v["text"].IsNull())) {
				buf.push_back('"');
				escapestring(v["text"], buf);
				buf.push_back('"');
			}

			std::cout << v["user"]["id"].GetInt64() << buf << std::endl;
		}
	}
	
}

int main(void) {
	std::array<char, BUF_SIZE> buf1;
	std::array<char, BUF_SIZE> buf2;
	uint64_t pos = 1;
	
	char* cbuf = buf1.data();

	buf1[0] = '[';
	buf2[0] = '[';

	std::string line;

	while (std::getline(std::cin, line)) {
		if ((line.size() + pos) > (BUF_SIZE - 2)) {
			pos--;
			cbuf[pos] = ']';
			cbuf[pos+1] = '\0';
			writeout(cbuf);
			if (cbuf == buf1.data()) {
				cbuf = buf2.data();
			} else {
				cbuf = buf1.data();
			}
			pos = 1;
		}

		std::copy(line.begin(), line.end() - 1, cbuf + pos);
		pos += line.size() - 1;
		cbuf[pos] = ',';
		pos++;
	}
	
	cbuf[pos - 1] = ']';
	cbuf[pos] = '\0';
	writeout(cbuf);

	return 0;
}
