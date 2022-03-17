master: master.o
	gcc -std=c11 master.o -o master

slave: slave.o
	gcc -std=c11 slave.o -o slave

master.o: master.c config.h
	gcc -c -std=c11 master.c

slave.o: slave.c config.h
	gcc -c -std=c11 slave.c

clean:
	rm *.o master slave logfile.* cstest