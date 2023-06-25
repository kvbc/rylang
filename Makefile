SRC = $(wildcard */*.c */*/*.c */*/*/*.c)
OUT = ry
FLAGS = -O2 -Wpedantic -Wall -Wno-format -Wno-unknown-pragmas
debug:
	gcc -o $(OUT) $(SRC) $(FLAGS)
# -D RY_DEBUG
release:
	gcc -o $(OUT) $(SRC) $(FLAGS) -s