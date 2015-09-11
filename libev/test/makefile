CC=gcc
FLAGS=-I. -I.. 
LDFLAGS=$(FLAGS) -L/usr/lib -L../.libs -lev -lpthread -Wall -Wl,-rpath ../.libs

OBJECTS=server.o
ALL_BIN=server evtest server_multi
 

all:$(ALL_BIN)

#$(ALL_BIN):$(OBJECTS) 
#	$(CC) $(FLAGS) $(LDFLAGS) -o $@ $^

%.o:%.c
	$(CC) -c $< $(FLAGS)

clean:
	rm -fr $(OBJECTS) $(ALL_BIN) *.o
#gcc  -g -O3 -I. evtest_multi.c -L.libs -lev -lpthread -o evtest_multi  -Wl,-rpath .libs
#cc -std=c99 -D_XOPEN_SOURCE=600 -D_DEFAULT_SOURCE -g -I../lib -pedantic -Wall -W -Wmissing-prototypes -Wno-sign-compare -Wno-unused-parameter    t_chown.c  ../libtlpi.a -lm -o t_chown
