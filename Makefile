CXX = g++
CPPFLAGS = -MT $@ -MMD -MP -MF $*.d
OPT = -O3 -flto -march=native
INCLUDE = -I include/
CXXFLAGS = $(OPT) $(CPPFLAGS) $(INCLUDE) -Wall -Wextra -std=c++17
LDFLAGS = -flto -pthread

SRCS :=    $(shell find tools -path "*.cpp")
OBJS :=    $(SRCS:.cpp=.o)
TARGETS := $(patsubst %.cpp, %, $(SRCS))
DEPS :=    $(OBJS:.o=.d)

all: $(TARGETS)

tools/wordhash: tools/wordhash.o
	$(CXX) $(LDFLAGS) -o $@ $< -lxxhash

%: %.o

clean:
	rm -fr $(OBJS) $(DEPS) $(TARGETS)

-include $(DEPS)
