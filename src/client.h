#ifndef CLIENT_H
#define CLIENT_H
#include <gtk/gtk.h>

typedef enum {
	MENU,
	GAME
} EGameState;

typedef struct {
	GtkWidget *vbox;
} MenuObjects;

typedef struct {
	GtkWidget *vbox;
} GameObjects;

typedef struct {
	EGameState state;
	MenuObjects menu;
	GameObjects game;
} Game;

#endif
