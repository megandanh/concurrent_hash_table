CC = gcc
CFLAGS = -pthread -Wall -g
OBJS = chash.o insert.o utils.o

chash: $(OBJS)
	$(CC) -o chash $(OBJS) $(CFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

clean:
	rm -f *.o chash output.txt
