# Makefile

CC = gcc
CFLAGS = -Wall -Wextra -Iinclude
OBJS = src/app.o src/datalink.o src/statemachine.o

app: $(OBJS)
	$(CC) $(CFLAGS) -o $@ $(OBJS)
	rm $(OBJS)  # Delete object files after linking

%.o: %.c
	$(CC) $(CFLAGS) -c -o $@ $<

clean:
	rm -f $(OBJS) app