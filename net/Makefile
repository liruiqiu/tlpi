include ../Makefile.inc
GEN_EXE = 
#connector connector_pool
LINUX_EXE = net

EXE = ${GEN_EXE} ${LINUX_EXE}

all : ${EXE}

allgen : ${GEN_EXE}

CFLAGS = ${IMPL_CFLAGS}  -I../lib/libev 
LDLIBS = ${IMPL_LDLIBS} ${IMPL_THREAD_FLAGS} ../lib/libev/.libs/libev.a

#fifo_seqnum_client.o fifo_seqnum_server.o : fifo_seqnum.h

net: connector.o connector_pool.o connector_manager.o
      #$(CC) $^ -o $@ $(CFLAGS)


clean : 
	${RM} ${EXE} *.o

showall :
	@ echo ${EXE}

${EXE} : ${LPLIB}		# True as a rough approximation
