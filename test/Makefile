
INCLUDE = -I./ -I../qc_lib -I../qc_mq

STEXT=a
DYEXT=so

CFLAG=-c -g
LD=$(CC) -shared

LIBS= 
SYSLIBS=-lpthread

STLIB = ../lib/qc_lib.$(STEXT) ../lib/qc_mq.$(STEXT)
QCEXE = ../bin/test


OBJS =  qc_message.o \
		qc_queue.o \
		qc_getlist.o \
		qc_putlist.o \
		qc_msgchain.o


all: $(QCEXE)


$(QCEXE):
	$(CC) -g test.c -o $(QCEXE) $(INCLUDE) $(LIBS) $(STLIB) $(SYSLIBS)
	#mv test_queue ../bin/
	rm -f *.o

.c.o:
	$(CC) $(CFLAG) $(DFLAG) $(INCLUDE) $<

clean :
	rm -f *.o
	rm -f ../bin/*

