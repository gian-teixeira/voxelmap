cflags = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11

all:
	gcc *.c -o main

tmp:
	gcc cube.c $(cflags) -o out