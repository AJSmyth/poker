#ifndef CLIENT_H
#define CLIENT_H
#include <gtk/gtk.h>
#include "deck.h"

typedef enum {
	MENU,
	GAME
} EGameState;

typedef struct {
	GtkWidget *vbox;
} MenuObjects;

typedef struct {
	GtkWidget *vbox;
	GtkWidget *mainLabel;
} GameObjects;

typedef struct {
	EGameState state;
	MenuObjects menu;
	GameObjects game;

	PLAYER player;
} Game;

#endif
