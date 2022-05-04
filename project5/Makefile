all: oss user

oss: oss.o
	gcc -std=c11 oss.o -o oss

user: user.o
	gcc -std=c11 user.o -o user

oss.o: oss.c common.h
	gcc -c -std=c11 oss.c

user.o: user.c common.h
	gcc -c -std=c11 user.c

clean:
	rm *.o oss user logfile