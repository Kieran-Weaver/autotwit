#include <rapidjson/document.h>
#include <rapidjson/encodings.h>
#include <csv.h>
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
			case '\n':
			case '\r':
			case '\t':
			case '\\':
			case '\"':
				buffer.push_back('\\');
				break;
			default:
				break;
		}
		buffer.push_back(st[i]);
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
	io::LineReader lr("STDIN", stdin);
	const char* line = lr.next_line();
	std::string buf = "[";

	while (line) {
		buf.append(line);
		if (buf.length() > BUF_SIZE) {
			buf.push_back(']');
			writeout(buf.c_str());
			buf = "[";
		} else {
			buf.push_back(',');
		}
		line = lr.next_line();
	}
	
	if (buf.back() == ',') {
		buf.pop_back();
	}
	buf.push_back(']');
	writeout(buf.c_str());

	return 0;
}
