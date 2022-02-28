master: master.o
	gcc -std=c11 master.o -o master

slave: slave.o
	gcc -std=c11 slave.o -o slave

master.o: master.c shared_memory.o config.h
	gcc -c -std=c11 master.c

slave.o: slave.c shared_memory.o config.h
	gcc -c -std=c11 slave.c

shared_memory.o: shared_memory.c shared_memory.h
	gcc -c -std=c11 shared_memory.c

clean:
	rm *.o master slave logfile.* cstest