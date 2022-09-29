all: main.out

main.out: main.o mines.o random.o
	gcc -o $@ $^

%.o: %.c
	gcc -o $@ -c $<
