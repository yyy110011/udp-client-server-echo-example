server.out: server.c logging/src/log.c logging/src/log.h
	gcc -DLOG_USE_COLOR -o server.out server.c logging/src/log.c

client.out: client.c logging/src/log.c logging/src/log.h
	gcc -DLOG_USE_COLOR -pthread -o client.out client.c logging/src/log.c

all: server.out client.out

clean:
	rm -f server.out client.out
