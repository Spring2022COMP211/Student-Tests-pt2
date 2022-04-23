CC=gcc
CFLAGS=-c -Wall -Werror -g

all: tests

tests: sa_tests
	./sa_tests

dm_tests: dm_tests.o main_mem.o dm_cache.o main_mem_log.o
	$(CC) -o dm_tests dm_tests.o main_mem.o dm_cache.o main_mem_log.o

fa_tests: fa_tests.o main_mem.o fa_cache.o main_mem_log.o
	$(CC) -o fa_tests fa_tests.o main_mem.o fa_cache.o main_mem_log.o

sa_tests: sa_tests.o main_mem.o sa_cache.o main_mem_log.o
	$(CC) -o sa_tests sa_tests.o main_mem.o sa_cache.o main_mem_log.o

dm_tests.o: dm_tests.c 
	$(CC) $(CFLAGS) dm_tests.c

fa_tests.o: fa_tests.c 
	$(CC) $(CFLAGS) fa_tests.c

sa_tests.o: sa_tests.c 
	$(CC) $(CFLAGS) sa_tests.c

main_mem.o: main_mem.c main_mem.h
	$(CC) $(CFLAGS) main_mem.c

main_mem_log.o: main_mem_log.c main_mem_log.h
	$(CC) $(CFLAGS) main_mem_log.c

dm_cache.o: dm_cache.c dm_cache.h main_mem.h
	$(CC) $(CFLAGS) dm_cache.c

fa_cache.o: fa_cache.c fa_cache.h main_mem.h
	$(CC) $(CFLAGS) fa_cache.c

sa_cache.o: sa_cache.c sa_cache.h main_mem.h
	$(CC) $(CFLAGS) sa_cache.c


clean:
	rm -f *.o main_mem_test_01 *.txt

