main:
	gcc src/client.c -o bin/poker `pkg-config --cflags gtk+-2.0` `pkg-config --libs gtk+-2.0`
