CC = gcc

FLAGS = -lSDL2 -lm -lSDL2_mixer

main :
	$(CC) -o main main.c $(FLAGS)

clean :
	rm -f main
