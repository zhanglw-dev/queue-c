
INCLUDE = -I./ -I../qc_lib

STEXT=a
DYEXT=so

CFLAG=-c -g
DFLAG=-fPIC

CC=gcc
LD=$(CC) -shared

LIBS=
SYSLIBS=-lpthread

STLIB = qc_mq.$(STEXT)
DYLIB = qc_mq.$(DYEXT)

OBJS =  qc_getter_list.o \
        qc_message.o \
		qc_msg_chain.o \
		qc_msg_queue.o \
		qc_putter_list.o

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

