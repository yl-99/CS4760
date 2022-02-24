master: master.o
	gcc master.o -o master

slave: slave.o
	gcc slave.o -o slave

master.o: master.c config.h
	gcc -c master.c

slave.o: slave.c config.h
	gcc -c slave.c

clean:
	rm *.o master slave