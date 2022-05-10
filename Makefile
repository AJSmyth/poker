main:
	gcc `pkg-config --cflags gtk+-3.0` -o bin/poker src/client.c `pkg-config --libs gtk+-3.0`
