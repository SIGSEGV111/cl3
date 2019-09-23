.PHONY: all clean

all: run

clean:
	rm -vf example

example: Makefile example.cpp
	g++ -Wall -Wextra -Werror -Wno-deprecated-copy -fdata-sections -ffunction-sections -std=c++11 -flto -Os -fwhole-program -fvisibility=hidden -ldl example.cpp -o example

run: example
	./example
