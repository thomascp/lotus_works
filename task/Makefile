
SRC= $(wildcard *.c) $(wildcard *.S) 
OBJTMP= $(SRC:.c=.o)
OBJ=$(OBJTMP:.S=.o)

all: $(OBJ)
	cp $(OBJ) $(OUTPUTPATH)/

.c.o:
	$(CC) $(CFLAGS) -c $<
.S.o:
	$(CC) $(ASFLAGS) -c $<
	
clean:
	rm *.o

 