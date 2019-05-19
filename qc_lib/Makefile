
INCLUDE = -I./

STEXT=a
DYEXT=so

CFLAG=-c -g
DFLAG=-fPIC

CC=gcc
LD=$(CC) -shared

LIBS=
SYSLIBS=-lpthread

STLIB = qc_lib.$(STEXT)
DYLIB = qc_lib.$(DYEXT)

OBJS =  qc_error.o \
        qc_hash.o \
		qc_list.o \
		qc_log.o \
		qc_pool.o \
		qc_queue.o \
		qc_socket.o \
		qc_thread.o \
		qc_thread_ex.o


all: $(STLIB)
#all: $(DYLIB)


$(DYLIB):$(STLIB)
	$(LD) -o $(DYLIB) ./*.o $(LIBS) $(SYSLIBS)
	mv $(DYLIB) ../lib/

$(STLIB):$(OBJS)
	ar ru $(STLIB) $(OBJS)
	mv $(STLIB) ../lib/
	rm -f *.o

.c.o:
	$(CC) $(CFLAG) $(DFLAG) $< $(INCLUDE) 

clean :
	rm -f *.o
	rm -f ../lib/$(STLIB)
	rm -f ../lib/$(DYLIB)

