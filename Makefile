all: melt

melt: melt.o melt.c
	gcc -no-pie -O0 melt.c melt.o -o melt

melt.o: melt.s
	nasm -f elf64 melt.s
