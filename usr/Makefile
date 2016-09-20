
SRC= $(wildcard *.c) $(wildcard *.s) 
OBJTMP= $(SRC:.c=.o)
OBJ=$(OBJTMP:.s=.o)

all: $(OBJ)
	cp $(OBJ) $(OUTPUTPATH)/

.c.o:
	$(CC) $(CFLAGS) -c $<
.s.o:
	$(CC) $(ASFLAGS) -c $<
	
clean:
	rm *.o

 