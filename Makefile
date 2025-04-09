CC = gcc
CFLAGS = -pthread -Wall -g
LDFLAGS = -pthread
OBJS = chash.o insert.o delete.o search.o utils.o print.o

chash: $(OBJS)
	$(CC) -o chash $(OBJS) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o chash output.txt
