all:
	gcc -I src/include -L src/lib -o snake snake.c -lmingw32 -lSDL3