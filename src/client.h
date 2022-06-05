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
	GtkWidget *playerBtn;
	GtkWidget *server;
} MenuObjects;

typedef struct {
	GtkWidget *vbox;
	GtkWidget *canvas;
	GtkWidget *mainLabel;
	GtkWidget *commLabel;
	GtkWidget *cardLabel;
	GtkWidget *playerLabel;
	GtkWidget *raiseBtn;
} GameObjects;

typedef struct {
	EGameState state;
	MenuObjects menu;
	GameObjects game;
	GAMESTATE gs;
	int fd;

	int ID;
} Game;
#endif
