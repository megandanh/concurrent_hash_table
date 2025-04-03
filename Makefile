CC = gcc
CFLAGS = -pthread -Wall -g
LDFLAGS = -pthread
OBJS = chash.o insert.o delete.o utils.o

chash: $(OBJS)
	$(CC) -o chash $(OBJS) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o chash output.txt
