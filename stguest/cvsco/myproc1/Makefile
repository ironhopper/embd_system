OBJECT = main.o bye.o hello.o
13184322out : main.o bye.o hello.o
	gcc -o 13184322out main.o bye.o hello.o
main.o : main.c
	gcc -c main.c
bye.o : bye.c
	gcc -c bye.c
hello.o : hello.c
	gcc -c hello.c
clean :
	rm ${OBJECT} 13184322out
