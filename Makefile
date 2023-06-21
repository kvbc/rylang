SRC = $(wildcard */*.c */*/*.c */*/*/*.c)
all:
	gcc -O2 -s -o ry $(SRC) -Wpedantic -Wall -Wno-format