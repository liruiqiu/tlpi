CC=gcc
CFLAGS=-I.
DEPS = sms4.h
OBJ = sms4_test.o sms4.o  

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS)

sms4_test: $(OBJ)
	gcc -o $@ $^ $(CFLAGS)

.PHONY: clean

clean:
	rm -f *.o *~ core sms4_test
