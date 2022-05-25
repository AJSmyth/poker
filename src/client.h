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
	GtkWidget *commLabel;
	GtkWidget *cardLabel;
	GtkWidget *playerLabel;
	GtkWidget *raiseBtn;
	GtkWidget *playerBtn;
} GameObjects;

typedef struct {
	EGameState state;
	MenuObjects menu;
	GameObjects game;
	GAMESTATE gs;

	int ID;
} Game;
#endif
