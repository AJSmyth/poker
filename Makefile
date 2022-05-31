CFLAGS = -Wno-deprecated-declarations -std=c11 -g
CFLAGS += `pkg-config --cflags gtk+-2.0`
LIBS = `pkg-config --libs gtk+-2.0`
DESTDIR = bin

main:
	gcc src/client.c src/deck.c src/game.c -o bin/poker $(CFLAGS) $(LIBS)
	gcc -std=c11 -pthread src/server.c src/game.c src/deck.c -o bin/server
