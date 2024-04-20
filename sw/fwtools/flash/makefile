CFLAGS = -Wall -g -O
SRCS = apiflash.c
OBJS = $(SRCS:%.c=%.o)
apiflash: $(OBJS)
	$(CC) -o $@ $(OBJS)

clean:
	rm -f $(OBJS) apiflash
