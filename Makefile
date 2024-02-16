GCC=gcc

all:
	${GCC} main.c -o tinyrsa -lgmp

clean:
	rm tinyrsa
