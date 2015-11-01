all: main

main: cipher
	gcc -c main.c -o main.o -g
	gcc main.o cipher.o -o main -lm -g

cipher:
	gcc -c -o cipher.o cipher.c -g

test: cipher
	gcc -c -o test_cipher.o test_cipher.c -g
	gcc test_cipher.o cipher.o -o test_cipher -lm -g
	./test_cipher

clean:
	rm *.o
	rm test_cipher
	rm main
