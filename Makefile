CFLAGS = -Wno-deprecated-declarations
CFLAGS += `pkg-config --cflags gtk+-2.0`
LIBS = `pkg-config --libs gtk+-2.0`
DESTDIR = bin

main:
	gcc src/client.c src/deck.c src/game.c -o bin/poker $(CFLAGS) $(LIBS)
