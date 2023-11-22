T1 = sow_bptree

CC = g++
CFLAGS = -g -c -Wall -O2
LDFLAGS = -lm -pthread 

OBJ_T1 = sow_bptree.o

all: $(T1) 

$(T1): $(OBJ_T1)
	$(CC) $(LDFLAGS) -o $@ $^

.cc.o:
	$(CC) $(CFLAGS) $<

clean:
	rm -f *~ *.o *.exe *.stackdump $(T1) 
