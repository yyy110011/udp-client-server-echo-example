server.out: server.c logging/src/log.c logging/src/log.h utils.c utils.h
	gcc -DLOG_USE_COLOR -o server.out server.c logging/src/log.c utils.c

client.out: client.c logging/src/log.c logging/src/log.h utils.c utils.h
	gcc -DLOG_USE_COLOR -pthread -o client.out client.c logging/src/log.c utils.c

all: server.out client.out

clean:
	rm -f server.out client.out
