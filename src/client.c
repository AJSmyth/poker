#include <gtk/gtk.h>
#include "client.h"
#include "deck.h"
#include "game.h"
#include <string.h>
#include <time.h>
#include <stdlib.h>
//socket stuff
#include <sys/socket.h>	//socket
#include <arpa/inet.h>	//inet_addr
#include <unistd.h>
#include <pthread.h>

const int STARTING_BALANCE = 500;
const int CARD_W = 50;
const int CARD_H = 73;
static gboolean delete_event(GtkWidget *widget, GdkEvent *event, gpointer data);
static void destroy(GtkWidget *widget, gpointer data);

GAMESTATE DoGame(GAMESTATE game);
char *StageStr(STAGES s);
char *SuitStr(SUIT s);
char *RankStr(RANK r);
int StageNum(STAGES s);


static void startGame(GtkWidget *widget, gpointer data);
static void doInput(GtkWidget *widget, gpointer data);
gboolean updateData(gpointer data);
static void quitGame(GtkWidget *widget, gpointer data);
static void paint(GtkWidget *widget, GdkEventExpose *eev, gpointer data);
void draw_image(cairo_t *cr, char *img_name, int x, int y, double scale);
cairo_surface_t *scale_to_whatever(cairo_surface_t *s, int orig_width, int orig_height, double scale);
void draw_cards(cairo_t *cr, CARD card, int x, int y, double scale);
void draw_text(cairo_t *cr, char *text, double x, double y, double size);
void draw_title(cairo_t *cr, char *text, double x, double y, double size);
void *connection_handler(void *game);


int main(int argc, char *argv[] ) {
	DECK deck = INIT();
	
	Game game;
	//initialize static members of game
	GAMESTATE gs;
	//fill the player array with empty, offline players
	CARD nullCard = {-1,-1};
	PLAYER emptyPlayer = {-1, -1, 0, nullCard, nullCard, NoAction, NORMAL, 0, false};
	for (int i = 0; i < 9; i++) {
		gs.players[i] = emptyPlayer;
		gs.players[i].ID = i;
	}

	gs.shuffleDeck = ShuffleCards (deck);
	gs.GameCount = 0;
	gs.stage = PREFLOP;

	//initialize other game variables
	GtkWidget *window;
	GtkWidget *mainVbox;

	gtk_init (&argc, &argv);
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_widget_set_size_request(window, 640, 480);
	gtk_window_set_resizable(GTK_WINDOW(window), FALSE);
	gtk_container_set_border_width(GTK_CONTAINER(window), 1);
	g_signal_connect(G_OBJECT(window), "delete_event", G_CALLBACK(delete_event), NULL);
	g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(destroy), NULL);
	mainVbox = gtk_vbox_new(FALSE, 1);
	gtk_container_add(GTK_CONTAINER(window), mainVbox);

	//=================================== MENU ====================================
	//initialize menu widgets
	GtkWidget *menuTitle = gtk_label_new("Queen's Poker");
	GtkWidget *menuStartBtn = gtk_button_new_with_label("Start");
	GtkWidget *serverLabel = gtk_label_new("Join");
	GtkWidget *playersLabel = gtk_label_new("Number of Players");
	game.menu.server = gtk_entry_new_with_max_length(24);
	GtkAdjustment *menuPlayers = GTK_ADJUSTMENT(gtk_adjustment_new(2,1,9,1,5,0));
	game.menu.playerBtn = gtk_spin_button_new(menuPlayers, 0.0, 0);
	game.menu.vbox = gtk_vbox_new(FALSE, 1);
	GtkWidget *menuHbox = gtk_hbox_new(FALSE, 1);

	gtk_entry_set_text(GTK_ENTRY(game.menu.server), "127.0.0.1:9000");

	//menu signals
	g_signal_connect(G_OBJECT(menuStartBtn), "clicked", G_CALLBACK(startGame), &game);

	//menu container packing
	gtk_box_pack_start(GTK_BOX(mainVbox), game.menu.vbox, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(game.menu.vbox), menuTitle, FALSE, FALSE, 10);
	gtk_box_pack_start(GTK_BOX(menuHbox), serverLabel, TRUE, FALSE, 10);
	gtk_box_pack_start(GTK_BOX(menuHbox), game.menu.server, TRUE, FALSE, 10);
	gtk_box_pack_start(GTK_BOX(menuHbox), playersLabel, TRUE, FALSE, 10);
	gtk_box_pack_start(GTK_BOX(menuHbox), game.menu.playerBtn, TRUE, FALSE, 10);
	gtk_box_pack_start(GTK_BOX(menuHbox), menuStartBtn, TRUE, FALSE, 10);
	gtk_box_pack_start(GTK_BOX(game.menu.vbox), menuHbox, FALSE, FALSE, 10);

	gtk_widget_show(menuTitle);
	gtk_widget_show(serverLabel);
	gtk_widget_show(game.menu.server);
	gtk_widget_show(playersLabel);
	gtk_widget_show(game.menu.playerBtn);
	gtk_widget_show(menuStartBtn);
	gtk_widget_show(menuHbox);
	gtk_widget_show(game.menu.vbox);

	//=================================== GAME =====================================	
	//initialize game widgets
	game.game.canvas = gtk_drawing_area_new();
	GtkWidget *gameQuit = gtk_button_new_with_label("Quit");
	GtkWidget *gameFold = gtk_button_new_with_label("Fold");
	GtkWidget *gameCall = gtk_button_new_with_label("Call");
	GtkWidget *gameCheck = gtk_button_new_with_label("Check");
	GtkWidget *gameRaise = gtk_button_new_with_label("Raise");
	//temporary labels
	game.game.commLabel = gtk_label_new("| Pot: $0 | === | Call: $0 |");
	game.game.cardLabel = gtk_label_new("Community Cards: hidden");
	game.game.playerLabel = gtk_label_new("Player Cards: n/a");
	//temporary labels
	GtkAdjustment *gameBetAdj = GTK_ADJUSTMENT(gtk_adjustment_new(1,0,1000,1,5,0));
	game.game.raiseBtn = gtk_spin_button_new(gameBetAdj, 0.1, 0);
	game.game.vbox = gtk_vbox_new(FALSE, 1);
	game.game.mainLabel = gtk_label_new("Ur move dumbass");
	GtkWidget *gameHboxBtn = gtk_hbox_new(TRUE, 1);
	gtk_widget_set_size_request(game.game.canvas, 640, 420);

	//game signals
	g_signal_connect(G_OBJECT(game.game.canvas), "expose-event", G_CALLBACK(paint), &game);
	g_signal_connect(G_OBJECT(gameFold), "clicked", G_CALLBACK(doInput), &game);
	g_signal_connect(G_OBJECT(gameCall), "clicked", G_CALLBACK(doInput), &game);
	g_signal_connect(G_OBJECT(gameCheck), "clicked", G_CALLBACK(doInput), &game);
	g_signal_connect(G_OBJECT(gameRaise), "clicked", G_CALLBACK(doInput), &game);
	g_signal_connect(G_OBJECT(gameQuit), "clicked", G_CALLBACK(quitGame), &game);

	//game container packing
	gtk_box_pack_start(GTK_BOX(mainVbox), game.game.vbox, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(game.game.vbox), game.game.canvas, FALSE, FALSE, 0);
	//temporary labels
	gtk_box_pack_start(GTK_BOX(game.game.vbox), game.game.commLabel, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(game.game.vbox), game.game.cardLabel, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(game.game.vbox), game.game.playerLabel, TRUE, TRUE, 0);
	//temporary labels
	gtk_box_pack_start(GTK_BOX(game.game.vbox), gameHboxBtn, FALSE, FALSE, 0);
	gtk_box_pack_start(GTK_BOX(game.game.vbox), game.game.mainLabel, TRUE, TRUE, 0);
	gtk_box_pack_start(GTK_BOX(gameHboxBtn), gameQuit, TRUE, TRUE, 10);
	gtk_box_pack_start(GTK_BOX(gameHboxBtn), gameFold, TRUE, TRUE, 10);
	gtk_box_pack_start(GTK_BOX(gameHboxBtn), gameCall, TRUE, TRUE, 10);
	gtk_box_pack_start(GTK_BOX(gameHboxBtn), gameCheck, TRUE, TRUE, 10);
	gtk_box_pack_start(GTK_BOX(gameHboxBtn), gameRaise, TRUE, TRUE, 10);
	gtk_box_pack_start(GTK_BOX(gameHboxBtn), game.game.raiseBtn, TRUE, TRUE, 10);


	gtk_widget_show(game.game.canvas);
	gtk_widget_show(gameQuit);
	gtk_widget_show(gameFold);
	gtk_widget_show(gameCall);
	gtk_widget_show(gameCheck);
	gtk_widget_show(gameRaise);
	gtk_widget_show(game.game.raiseBtn);
	//temporary labels
	//gtk_widget_show(game.game.playerLabel);
	//gtk_widget_show(game.game.commLabel);
	//gtk_widget_show(game.game.cardLabel);
	//temporary labels
	gtk_widget_show(gameHboxBtn);

	gtk_widget_show(mainVbox);
	gtk_widget_show(window);
	
	g_timeout_add(100,G_SOURCE_FUNC(updateData), &game);
	
	gtk_main();
	return 0;
}



gboolean updateData(gpointer data) {
	Game *g = data;
	if (g->state == GAME && (g->gs.playerTurn != g->oldgs.playerTurn || g->gs.stage != g->oldgs.stage)) {
		paint(GTK_WIDGET(g->game.canvas), NULL, data);
		g->oldgs = g->gs;
	}
	/*
	if (g->state == GAME) {
		char temp[256];
		//update stats 
		sprintf(temp, "| Pot: $%d | === | Call: $%d | === | %s | === | Player %d |", game.pot, game.currCall, StageStr(game.stage), game.playerTurn + 1);
		gtk_label_set_text(GTK_LABEL(g->game.commLabel), temp);

		//update community cards
		if (game.stage > PREFLOP) {
			DECK cc = game.communityCards;
			sprintf(temp, "Community Cards: %s of %s", RankStr(cc.cards[0].rank), SuitStr(cc.cards[1].suit));

			for (int i = 1; i < 2 + (game.stage - PREFLOP); i++) sprintf(temp + strlen(temp), ", %s of %s", RankStr(cc.cards[i].rank), SuitStr(cc.cards[i].suit));
			gtk_label_set_text(GTK_LABEL(g->game.cardLabel), temp);
		}
		else gtk_label_set_text(GTK_LABEL(g->game.cardLabel), "Community Cards: hidden");

		//update player cards
		CARD c1 = game.players[g->ID].card1, c2 = game.players[g->ID].card2;
		sprintf(temp, "Player %d's cards: %s of %s, %s of %s", g->ID + 1, RankStr(c1.rank), SuitStr(c1.suit), RankStr(c2.rank), SuitStr(c2.suit));
	}
	*/
	return 69420;
}



void doInput(GtkWidget *widget, gpointer data) {
	Game *g = data;
	GAMESTATE game = g->gs;
	
	if (game.playerTurn == g->ID) {
		if (game.stage != WIN) {	
			//put data into gamestate struct
			if (!strcmp(gtk_button_get_label(GTK_BUTTON(widget)), "Fold")) {
				game.players[game.playerTurn].action = FOLD;
			}
			else if (!strcmp(gtk_button_get_label(GTK_BUTTON(widget)), "Call")) {
				game.players[game.playerTurn].action = CALL;
			}
			else if (!strcmp(gtk_button_get_label(GTK_BUTTON(widget)), "Check")) {
				game.players[game.playerTurn].action = CHECK;
			}
			else if (!strcmp(gtk_button_get_label(GTK_BUTTON(widget)), "Raise")) {
				game.players[game.playerTurn].action = RAISE;
				game.players[game.playerTurn].raiseAmt = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(g->game.raiseBtn));
			}

			//submit move
			int tturn = g->gs.playerTurn;
			STAGES tstage = g->gs.stage;

			g->gs = DoGame(game);
			
			//check if a move was executed (if not invalid move)
			if (g->gs.playerTurn == tturn && g->gs.stage == tstage) {
				gtk_label_set_text(GTK_LABEL(g->game.mainLabel), "Invalid move!");
				gtk_widget_show(g->game.mainLabel);
			}
			//if valid move hide the error message
			else {
				PacketType update = GS_UPDATE;
				write(g->fd, &update, sizeof(update));
				write(g->fd, &g->gs, sizeof(g->gs));
				gtk_widget_hide(g->game.mainLabel);
			}
		}
		else {
			gtk_label_set_text(GTK_LABEL(g->game.mainLabel), "Game Over!");
			gtk_widget_show(g->game.mainLabel);
		}
	}

	else {
		gtk_label_set_text(GTK_LABEL(g->game.mainLabel), "Not your move!");
		gtk_widget_show(g->game.mainLabel);
	}
} 



static void startGame(GtkWidget *widget, gpointer data) {
	Game *g = data;
	int sock;
	struct sockaddr_in server;
	
	//connect to server
	//Create socket
	sock = socket(AF_INET , SOCK_STREAM , 0);
	g->fd = sock;

	if (sock == -1)
	{
		printf("\nCould not create socket");
		return;
	}

	//process input from GTK
	char s[25];
        strcpy(s, gtk_entry_get_text(GTK_ENTRY(g->menu.server)));
        char *serv = strtok(s, ":"), *port = strtok(NULL, ":");
	printf("\n SERVER %s::%s", serv, port);
	
	server.sin_addr.s_addr = inet_addr(serv);//currently on local host for testing purposes
	server.sin_family = AF_INET;
	server.sin_port = htons(atoi(port));

	//Connect to remote server
	if (connect(sock, (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		perror("\nConnect failed. Error");
		close(sock);
		return;
	}
	
	printf("\nConnected to game server.\n");
	if(read(sock , &g->ID , sizeof(g->ID)) < 0){
		printf("\nUnable to retrieve player net ID.");
		close(sock);
		return;
	}

	if(g->ID == 0){
		GAMESTATE game;
		DECK deck = INIT();
		CARD nullCard = {-1,-1};
		PLAYER emptyPlayer = {-1, -1, 0, nullCard, nullCard, NoAction, NORMAL, 0, false};
		for (int i = 0; i < 9; i++) {
			game.players[i] = emptyPlayer;
			game.players[i].ID = i;
		}

		//initialize other game variables
		game.shuffleDeck = ShuffleCards (deck);
		game.GameCount = 0;
		game.stage = PREFLOP;

		game.numberPlayers = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(g->menu.playerBtn));

		//set that many players to online
		for (int i = 0; i < game.numberPlayers; i++) {
			game.players[i].online = true;
			game.players[i].Balance = STARTING_BALANCE;
		}

		game = AssignCards(game);
		g->gs = game;
		puts("Writing game to other players in session.\n");
		write(sock, &game, sizeof(game));
		read(sock, &(g->gs), sizeof(g->gs));
	}else{
		puts("Waiting for host to initialize game.\n");
		read(sock, &(g->gs), sizeof(g->gs));
	}
	gtk_widget_hide(g->menu.vbox);
	gtk_widget_show(g->game.vbox);
	g->state = GAME;
	pthread_t thread; 
	if (pthread_create(&thread, NULL , connection_handler, data) < 0)
	{
		perror("could not create thread");
	}
}

void *connection_handler(void *game)
{
	Game* g = (Game*)game;
	int sock = g->fd;
	PacketType request = GS_REQUEST;
	struct timespec tim, tim2;
	tim.tv_sec = 0;
	tim.tv_nsec = 100000000L;
	while(1){
		write(sock, &request, sizeof(request));
		read(sock, &g->gs, sizeof(g->gs));
		nanosleep(&tim, &tim2);
	}
}



static void quitGame(GtkWidget *widget, gpointer data) {
	Game *game = data;
	gtk_widget_hide(game->game.vbox);
	gtk_widget_show(game->menu.vbox);
	game->state = MENU;
	close(game->fd);
}

void draw_image(cairo_t *cr, char *img_name, int x, int y, double scale)
{
	//printf("%s", img_name);
	cairo_surface_t *image = cairo_image_surface_create_from_png(img_name);

	if (cairo_surface_status(image) != CAIRO_STATUS_SUCCESS)
	{
		printf("Could not load image \"%s\"\n", cairo_status_to_string(cairo_surface_status(image)));
	}

	int imgW = cairo_image_surface_get_width(image);
	int imgH = cairo_image_surface_get_height(image);

	cairo_surface_t *scaledImage = scale_to_whatever(image, imgW, imgH, scale);
	cairo_set_source_surface(cr, scaledImage, x, y);
	cairo_paint(cr);
}

cairo_surface_t *scale_to_whatever(cairo_surface_t *s, int orig_width, int orig_height, double scale)
{
	cairo_surface_t *result = cairo_surface_create_similar(s,
														   cairo_surface_get_content(s), orig_width / 2, orig_height / 2);
	cairo_t *cr = cairo_create(result);
	cairo_scale(cr, scale, scale);
	cairo_set_source_surface(cr, s, 0, 0);
	cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
	cairo_paint(cr);
	cairo_destroy(cr);
	return result;
}

void draw_cards(cairo_t *cr, CARD card, int x, int y, double scale)
{
	char buffer[100] = "";

	switch (card.suit)
	{
	case (0):
		sprintf(buffer, "../assets/%s_of_diamonds.png", RankStr(card.rank));
		draw_image(cr, buffer, x, y, scale);
		//printf("%s", buffer);
		memset(buffer, 0, 100);
		break;

	case (1):
		sprintf(buffer, "../assets/%s_of_clubs.png", RankStr(card.rank));
		draw_image(cr, buffer, x, y, scale);
		//printf("%s", buffer);
		memset(buffer, 0, 100);
		break;

	case (2):
		sprintf(buffer, "../assets/%s_of_hearts.png", RankStr(card.rank));
		draw_image(cr, buffer, x, y, scale);
		//printf("%s", buffer);
		memset(buffer, 0, 100);
		break;

	case (3):
		sprintf(buffer, "../assets/%s_of_spades.png", RankStr(card.rank));
		draw_image(cr, buffer, x, y, scale);
		//printf("%s", buffer);
		memset(buffer, 0, 100);
	}
}

void draw_text(cairo_t *cr, char *text, double x, double y, double size)
{
	cairo_set_source_rgb(cr, 0, 0, 0);
	cairo_select_font_face(cr, "Sans", CAIRO_FONT_SLANT_NORMAL,
						   CAIRO_FONT_WEIGHT_BOLD);
	cairo_set_font_size(cr, size);
	//printf("%s\n", text);
	cairo_move_to(cr, x, y);
	cairo_show_text(cr, text);
}

void draw_title(cairo_t *cr, char *text, double x, double y, double size)
{
	cairo_set_source_rgb(cr, 0, 0, 0);
	cairo_select_font_face(cr, "Apple Chancery", CAIRO_FONT_SLANT_NORMAL,
						   CAIRO_FONT_WEIGHT_BOLD);
	cairo_set_font_size(cr, size);
	//printf("%s\n", text);
	cairo_move_to(cr, x, y);
	cairo_show_text(cr, text);
}


static void paint(GtkWidget *widget, GdkEventExpose *eev, gpointer data)
{
	gint width, height;
	gint i;
	cairo_t *cr;

	width = widget->allocation.width;
	height = widget->allocation.height;

	cr = gdk_cairo_create(widget->window);

	/* clear background */
	cairo_set_source_rgb(cr, 0, 0.2, 0);
	cairo_paint(cr);

	Game *g = data;
	GAMESTATE game = g->gs;

	char buffer[100];
	switch (game.stage) {

	case PREFLOP:
		{
		sprintf(buffer, "Stage: %s", StageStr(game.stage));
		draw_title(cr, buffer, ((width) / 2) - 3.5 * (CARD_W / 2), (height) / 2 - CARD_H, 30);
		memset(buffer, 0, 100);

		// community deck (hidden)
		for (int i = 0; i <= 4; i++)
		{
			draw_image(cr, "../assets/card_back.png", ((width - CARD_W) / 2 + i * (CARD_W)) - 4 * (CARD_W / 2), (height - CARD_H) / 2, 0.1);
		}

		//Draw pot
		sprintf(buffer, "Pot: $%d", game.pot);
		draw_text(cr, buffer, ((width) / 2) - 1 * (CARD_W / 2), (height) / 2 + CARD_H, 15);
		//Draw current call
		sprintf(buffer, "Current Call: $%d", game.currCall);
		draw_text(cr, buffer, ((width) / 2) - 1.1 * (CARD_W), (height) / 2 + 1.25*CARD_H, 15);

		// your own hand!
		draw_cards(cr, game.players[g->ID].card1, ((width - CARD_W) / 2 - (CARD_W / 2)), (height - CARD_H), 0.1);
		draw_cards(cr, game.players[g->ID].card2, ((width - CARD_W) / 2 - (CARD_W / 2)) + CARD_W, (height - CARD_H), 0.1);

		// highlight cards if its player's turn
		if ((game.playerTurn == g->ID) && game.players[g->ID].action != FOLD)
		{
			cairo_set_source_rgba(cr, 1.0, 1.0, 0.0, 1);
			cairo_rectangle(cr, ((width - CARD_W) / 2 - (CARD_W / 2)), (height - CARD_H), CARD_W, CARD_H);
			cairo_rectangle(cr, ((width - CARD_W) / 2 - (CARD_W / 2)) + CARD_W, (height - CARD_H), CARD_W, CARD_H);
			cairo_set_line_width(cr, 2.5);
			cairo_stroke(cr);
		}

		if (game.players[g->ID].action == FOLD)
		{
			// shadow cards when user folds
			cairo_set_source_rgba(cr, 0, 0, 0, 0.65);
			cairo_rectangle(cr, ((width - CARD_W) / 2 - (CARD_W / 2)), (height - CARD_H), CARD_W, CARD_H);
			cairo_rectangle(cr, ((width - CARD_W) / 2 - (CARD_W / 2)) + CARD_W, (height - CARD_H), CARD_W, CARD_H);
			cairo_fill(cr);
		}

		int spacing = 0;
		// the rest of the players (hidden)
		for (int i = 0; i < game.numberPlayers; i++)
		{
			if (i != g->ID)
			{
				int cardpos = 0 + (game.numberPlayers * (i - spacing)) * (CARD_W * 0.5);

				draw_image(cr, "../assets/card_back.png", cardpos, 0, 0.05);
				draw_image(cr, "../assets/card_back.png", cardpos + (CARD_W * 0.5), 0, 0.05);

				// highlight cards if its player's turn
				if ((game.playerTurn == i) && game.players[i].action != FOLD)
				{
					cairo_set_source_rgba(cr, 1.0, 1.0, 0.0, 1);
					cairo_rectangle(cr, cardpos, 0, CARD_W * 0.5, CARD_H * 0.5);
					cairo_rectangle(cr, cardpos + (CARD_W * 0.5), 0, CARD_W * 0.5, CARD_H * 0.5);
					cairo_set_line_width(cr, 2.5);
					cairo_stroke(cr);
				}

				if (game.players[i].action == FOLD)
				{
					// shadow cards when user folds
					cairo_set_source_rgba(cr, 0, 0, 0, 0.65);
					cairo_rectangle(cr, cardpos, 0, CARD_W * 0.5, CARD_H * 0.5);
					cairo_rectangle(cr, cardpos + (CARD_W * 0.5), 0, CARD_W * 0.5, CARD_H * 0.5);
					cairo_fill(cr);
				}

				//display player attributes
				sprintf(buffer, "Id:%d", game.players[i].ID);
				draw_text(cr, buffer, cardpos, (CARD_H * 0.75), 12.5);
				sprintf(buffer, "Balance:");
				draw_text(cr, buffer, cardpos, (CARD_H * 0.95), 12.5);
				sprintf(buffer, "$%d", game.players[i].Balance);
				draw_text(cr, buffer, cardpos, (CARD_H * 1.15), 12.5);
			}
			else
			{
				spacing = 1;
			}
		}
		}
		break;

	case FLOP:
		{
		sprintf(buffer, "Stage: %s", StageStr(game.stage));
		draw_title(cr, buffer, ((width) / 2) - 3.5 * (CARD_W / 2), (height) / 2 - CARD_H, 30);
		memset(buffer, 0, 100);

		// community deck (3 revealing)
		for (int i = 0; i <= 4; i++)
		{
			if (i < 3)
			{
				draw_cards(cr, game.communityCards.cards[i], ((width - CARD_W) / 2 + i * (CARD_W)) - 4 * (CARD_W / 2), (height - CARD_H) / 2, 0.1);
			}
			else
			{
				draw_image(cr, "../assets/card_back.png", ((width - CARD_W) / 2 + i * (CARD_W)) - 4 * (CARD_W / 2), (height - CARD_H) / 2, 0.1);
			}
		}

		//Draw pot
		sprintf(buffer, "Pot: $%d", game.pot);
		draw_text(cr, buffer, ((width) / 2) - 1 * (CARD_W / 2), (height) / 2 + CARD_H, 15);
		//Draw current call
		sprintf(buffer, "Current Call: $%d", game.currCall);
		draw_text(cr, buffer, ((width) / 2) - 1.1 * (CARD_W), (height) / 2 + 1.25*CARD_H, 15);

		// your own hand!
		draw_cards(cr, game.players[g->ID].card1, ((width - CARD_W) / 2 - (CARD_W / 2)), (height - CARD_H), 0.1);
		draw_cards(cr, game.players[g->ID].card2, ((width - CARD_W) / 2 - (CARD_W / 2)) + CARD_W, (height - CARD_H), 0.1);

		// highlight cards if its player's turn
		if ((game.playerTurn == g->ID) && game.players[g->ID].action != FOLD)
		{
			cairo_set_source_rgba(cr, 1.0, 1.0, 0.0, 1);
			cairo_rectangle(cr, ((width - CARD_W) / 2 - (CARD_W / 2)), (height - CARD_H), CARD_W, CARD_H);
			cairo_rectangle(cr, ((width - CARD_W) / 2 - (CARD_W / 2)) + CARD_W, (height - CARD_H), CARD_W, CARD_H);
			cairo_set_line_width(cr, 2.5);
			cairo_stroke(cr);
		}

		if (game.players[g->ID].action == FOLD)
		{
			// shadow cards when user folds
			cairo_set_source_rgba(cr, 0, 0, 0, 0.65);
			cairo_rectangle(cr, ((width - CARD_W) / 2 - (CARD_W / 2)), (height - CARD_H), CARD_W, CARD_H);
			cairo_rectangle(cr, ((width - CARD_W) / 2 - (CARD_W / 2)) + CARD_W, (height - CARD_H), CARD_W, CARD_H);
			cairo_fill(cr);
		}

		int spacing = 0;
		for (int i = 0; i < game.numberPlayers; i++)
		{
			if (i != g->ID)
			{
				int cardpos = 0 + (game.numberPlayers * (i - spacing)) * (CARD_W * 0.5);

				draw_image(cr, "../assets/card_back.png", cardpos, 0, 0.05);
				draw_image(cr, "../assets/card_back.png", cardpos + (CARD_W * 0.5), 0, 0.05);
				
				// highlight cards if its player's turn
				if ((game.playerTurn == i) && game.players[i].action != FOLD)
				{
					cairo_set_source_rgba(cr, 1.0, 1.0, 0.0, 1);
					cairo_rectangle(cr, cardpos, 0, CARD_W * 0.5, CARD_H * 0.5);
					cairo_rectangle(cr, cardpos + (CARD_W * 0.5), 0, CARD_W * 0.5, CARD_H * 0.5);
					cairo_set_line_width(cr, 2.5);
					cairo_stroke(cr);
				}

				if (game.players[i].action == FOLD)
				{
					// shadow cards when user folds
					cairo_set_source_rgba(cr, 0, 0, 0, 0.65);
					cairo_rectangle(cr, cardpos, 0, CARD_W * 0.5, CARD_H * 0.5);
					cairo_rectangle(cr, cardpos + (CARD_W * 0.5), 0, CARD_W * 0.5, CARD_H * 0.5);
					cairo_fill(cr);
				}

				//display player attributes
				sprintf(buffer, "Id:%d", game.players[i].ID);
				draw_text(cr, buffer, cardpos, (CARD_H * 0.75), 12.5);
				sprintf(buffer, "Balance:");
				draw_text(cr, buffer, cardpos, (CARD_H * 0.95), 12.5);
				sprintf(buffer, "$%d", game.players[i].Balance);
				draw_text(cr, buffer, cardpos, (CARD_H * 1.15), 12.5);
			}
			else
			{
				spacing = 1;
			}
		}
		}
		break;

	case TURN:
		{
		sprintf(buffer, "Stage: %s", StageStr(game.stage));
		draw_title(cr, buffer, ((width) / 2) - 3.5 * (CARD_W / 2), (height) / 2 - CARD_H, 30);
		memset(buffer, 0, 100);
		// community deck (4 revealing)
		for (int i = 0; i <= 4; i++)
		{
			if (i < 4)
			{
				draw_cards(cr, game.communityCards.cards[i], ((width - CARD_W) / 2 + i * (CARD_W)) - 4 * (CARD_W / 2), (height - CARD_H) / 2, 0.1);
			}
			else
			{
				draw_image(cr, "../assets/card_back.png", ((width - CARD_W) / 2 + i * (CARD_W)) - 4 * (CARD_W / 2), (height - CARD_H) / 2, 0.1);
			}
		}

		//Draw pot
		sprintf(buffer, "Pot: $%d", game.pot);
		draw_text(cr, buffer, ((width) / 2) - 1 * (CARD_W / 2), (height) / 2 + CARD_H, 15);
		//Draw current call
		sprintf(buffer, "Current Call: $%d", game.currCall);
		draw_text(cr, buffer, ((width) / 2) - 1.1 * (CARD_W), (height) / 2 + 1.25*CARD_H, 15);

		// your own hand!
		draw_cards(cr, game.players[g->ID].card1, ((width - CARD_W) / 2 - (CARD_W / 2)), (height - CARD_H), 0.1);
		draw_cards(cr, game.players[g->ID].card2, ((width - CARD_W) / 2 - (CARD_W / 2)) + CARD_W, (height - CARD_H), 0.1);

		// highlight cards if its player's turn
		if ((game.playerTurn == g->ID) && game.players[g->ID].action != FOLD)
		{
			cairo_set_source_rgba(cr, 1.0, 1.0, 0.0, 1);
			cairo_rectangle(cr, ((width - CARD_W) / 2 - (CARD_W / 2)), (height - CARD_H), CARD_W, CARD_H);
			cairo_rectangle(cr, ((width - CARD_W) / 2 - (CARD_W / 2)) + CARD_W, (height - CARD_H), CARD_W, CARD_H);
			cairo_set_line_width(cr, 2.5);
			cairo_stroke(cr);
		}

		if (game.players[g->ID].action == FOLD)
		{
			// shadow cards when user folds
			cairo_set_source_rgba(cr, 0, 0, 0, 0.65);
			cairo_rectangle(cr, ((width - CARD_W) / 2 - (CARD_W / 2)), (height - CARD_H), CARD_W, CARD_H);
			cairo_rectangle(cr, ((width - CARD_W) / 2 - (CARD_W / 2)) + CARD_W, (height - CARD_H), CARD_W, CARD_H);
			cairo_fill(cr);
		}

		int spacing = 0;
		for (int i = 0; i < game.numberPlayers; i++)
		{
			if (i != g->ID)
			{
				int cardpos = 0 + (3 * (i - spacing)) * (CARD_W * 0.5);

				draw_image(cr, "../assets/card_back.png", cardpos, 0, 0.05);
				draw_image(cr, "../assets/card_back.png", cardpos + (CARD_W * 0.5), 0, 0.05);
				
				// highlight cards if its player's turn
				if ((game.playerTurn == i) && game.players[i].action != FOLD)
				{
					cairo_set_source_rgba(cr, 1.0, 1.0, 0.0, 1);
					cairo_rectangle(cr, cardpos, 0, CARD_W * 0.5, CARD_H * 0.5);
					cairo_rectangle(cr, cardpos + (CARD_W * 0.5), 0, CARD_W * 0.5, CARD_H * 0.5);
					cairo_set_line_width(cr, 2.5);
					cairo_stroke(cr);
				}

				if (game.players[i].action == FOLD)
				{
					// shadow cards when user folds
					cairo_set_source_rgba(cr, 0, 0, 0, 0.65);
					cairo_rectangle(cr, cardpos, 0, CARD_W * 0.5, CARD_H * 0.5);
					cairo_rectangle(cr, cardpos + (CARD_W * 0.5), 0, CARD_W * 0.5, CARD_H * 0.5);
					cairo_fill(cr);
				}

				//display player attributes
				sprintf(buffer, "Id:%d", game.players[i].ID);
				draw_text(cr, buffer, cardpos, (CARD_H * 0.75), 12.5);
				sprintf(buffer, "Balance:");
				draw_text(cr, buffer, cardpos, (CARD_H * 0.95), 12.5);
				sprintf(buffer, "$%d", game.players[i].Balance);
				draw_text(cr, buffer, cardpos, (CARD_H * 1.15), 12.5);
			}
			else
			{
				spacing = 1;
			}
		}
		}
		break;

	case RIVER:
		{
		sprintf(buffer, "Stage: %s", StageStr(game.stage));
		draw_title(cr, buffer, ((width) / 2) - 3.5 * (CARD_W / 2), (height) / 2 - CARD_H, 30);
		memset(buffer, 0, 100);
		// community deck (5 revealing)
		for (int i = 0; i <= 4; i++)
		{
			draw_cards(cr, game.communityCards.cards[i], ((width - CARD_W) / 2 + i * (CARD_W)) - 4 * (CARD_W / 2), (height - CARD_H) / 2, 0.1);
		}

		//Draw pot
		sprintf(buffer, "Pot: $%d", game.pot);
		draw_text(cr, buffer, ((width) / 2) - 1 * (CARD_W / 2), (height) / 2 + CARD_H, 15);
		//Draw current call
		sprintf(buffer, "Current Call: $%d", game.currCall);
		draw_text(cr, buffer, ((width) / 2) - 1.1 * (CARD_W), (height) / 2 + 1.25*CARD_H, 15);

		// your own hand!
		draw_cards(cr, game.players[g->ID].card1, ((width - CARD_W) / 2 - (CARD_W / 2)), (height - CARD_H), 0.1);
		draw_cards(cr, game.players[g->ID].card2, ((width - CARD_W) / 2 - (CARD_W / 2)) + CARD_W, (height - CARD_H), 0.1);


		// highlight cards if its player's turn
		if ((game.playerTurn == g->ID) && game.players[g->ID].action != FOLD)
		{
			cairo_set_source_rgba(cr, 1.0, 1.0, 0.0, 1);
			cairo_rectangle(cr, ((width - CARD_W) / 2 - (CARD_W / 2)), (height - CARD_H), CARD_W, CARD_H);
			cairo_rectangle(cr, ((width - CARD_W) / 2 - (CARD_W / 2)) + CARD_W, (height - CARD_H), CARD_W, CARD_H);
			cairo_set_line_width(cr, 2.5);
			cairo_stroke(cr);
		}

		if (game.players[g->ID].action == FOLD)
		{
			// shadow cards when user folds
			cairo_set_source_rgba(cr, 0, 0, 0, 0.65);
			cairo_rectangle(cr, ((width - CARD_W) / 2 - (CARD_W / 2)), (height - CARD_H), CARD_W, CARD_H);
			cairo_rectangle(cr, ((width - CARD_W) / 2 - (CARD_W / 2)) + CARD_W, (height - CARD_H), CARD_W, CARD_H);
			cairo_fill(cr);
		}

		int spacing = 0;
		for (int i = 0; i < game.numberPlayers; i++)
		{
			if (i != g->ID)
			{
				int cardpos = 0 + (3 * (i - spacing)) * (CARD_W * 0.5);

				draw_image(cr, "../assets/card_back.png", cardpos, 0, 0.05);
				draw_image(cr, "../assets/card_back.png", cardpos + (CARD_W * 0.5), 0, 0.05);

				// highlight cards if its player's turn
				if ((game.playerTurn == i) && game.players[i].action != FOLD)
				{
					cairo_set_source_rgba(cr, 1.0, 1.0, 0.0, 1);
					cairo_rectangle(cr, cardpos, 0, CARD_W * 0.5, CARD_H * 0.5);
					cairo_rectangle(cr, cardpos + (CARD_W * 0.5), 0, CARD_W * 0.5, CARD_H * 0.5);
					cairo_set_line_width(cr, 2.5);
					cairo_stroke(cr);
				}

				if (game.players[i].action == FOLD)
				{
					// shadow cards when user folds
					cairo_set_source_rgba(cr, 0, 0, 0, 0.65);
					cairo_rectangle(cr, cardpos, 0, CARD_W * 0.5, CARD_H * 0.5);
					cairo_rectangle(cr, cardpos + (CARD_W * 0.5), 0, CARD_W * 0.5, CARD_H * 0.5);
					cairo_fill(cr);
				}

				//display player attributes
				sprintf(buffer, "Id:%d", game.players[i].ID);
				draw_text(cr, buffer, cardpos, (CARD_H * 0.75), 12.5);
				sprintf(buffer, "Balance:");
				draw_text(cr, buffer, cardpos, (CARD_H * 0.95), 12.5);
				sprintf(buffer, "$%d", game.players[i].Balance);
				draw_text(cr, buffer, cardpos, (CARD_H * 1.15), 12.5);
			}
			else
			{
				spacing = 1;
			}
		}
		}
		break;

	case WIN:
		{
		sprintf(buffer, "%s is the winner!", StageStr(game.stage));
		draw_title(cr, buffer, ((width) / 2) - 3.5 * (CARD_W / 2), (height) / 2 - CARD_H, 30);
		memset(buffer, 0, 100);
		// community deck (5 revealing)
		for (int i = 0; i <= 4; i++)
		{
			draw_cards(cr, game.communityCards.cards[i], ((width - CARD_W) / 2 + i * (CARD_W)) - 4 * (CARD_W / 2), (height - CARD_H) / 2, 0.1);
		}

		//Draw pot
		sprintf(buffer, "Pot: $%d", game.pot);
		draw_text(cr, buffer, ((width) / 2) - 1 * (CARD_W / 2), (height) / 2 + CARD_H, 15);
		//Draw current call
		sprintf(buffer, "Current Call: $%d", game.currCall);
		draw_text(cr, buffer, ((width) / 2) - 1.1 * (CARD_W), (height) / 2 + 1.25*CARD_H, 15);

		// your own hand!
		draw_cards(cr, game.players[g->ID].card1, ((width - CARD_W) / 2 - (CARD_W / 2)), (height - CARD_H), 0.1);
		draw_cards(cr, game.players[g->ID].card2, ((width - CARD_W) / 2 - (CARD_W / 2)) + CARD_W, (height - CARD_H), 0.1);

		// highlight cards if its player's turn
		if ((game.playerTurn == g->ID) && game.players[g->ID].action != FOLD)
		{
			cairo_set_source_rgba(cr, 1.0, 1.0, 0.0, 1);
			cairo_rectangle(cr, ((width - CARD_W) / 2 - (CARD_W / 2)), (height - CARD_H), CARD_W, CARD_H);
			cairo_rectangle(cr, ((width - CARD_W) / 2 - (CARD_W / 2)) + CARD_W, (height - CARD_H), CARD_W, CARD_H);
			cairo_set_line_width(cr, 2.5);
			cairo_stroke(cr);
		}

		if (game.players[g->ID].action == FOLD)
		{
			// shadow cards when user folds
			cairo_set_source_rgba(cr, 0, 0, 0, 0.65);
			cairo_rectangle(cr, ((width - CARD_W) / 2 - (CARD_W / 2)), (height - CARD_H), CARD_W, CARD_H);
			cairo_rectangle(cr, ((width - CARD_W) / 2 - (CARD_W / 2)) + CARD_W, (height - CARD_H), CARD_W, CARD_H);
			cairo_fill(cr);
		}

		int spacing = 0;
		// the rest of the players(reveal their cards)
		for (int i = 0; i < game.numberPlayers; i++)
		{
			if (i != g->ID)
			{
				int cardpos = 0 + (3 * (i - spacing)) * (CARD_W * 0.5);

				draw_cards(cr, game.players[g->ID].card1, cardpos, 0, 0.05);
				draw_cards(cr, game.players[g->ID].card2, cardpos + (0.5 * CARD_W), 0, 0.05);

				// highlight cards if its player's turn
				if ((game.playerTurn == i) && game.players[i].action != FOLD)
				{
					cairo_set_source_rgba(cr, 1.0, 1.0, 0.0, 1);
					cairo_rectangle(cr, cardpos, 0, CARD_W * 0.5, CARD_H * 0.5);
					cairo_rectangle(cr, cardpos + (CARD_W * 0.5), 0, CARD_W * 0.5, CARD_H * 0.5);
					cairo_set_line_width(cr, 2.5);
					cairo_stroke(cr);
				}

				if (game.players[i].action == FOLD)
				{
					// shadow cards when user folds
					cairo_set_source_rgba(cr, 0, 0, 0, 0.65);
					cairo_rectangle(cr, cardpos, 0, CARD_W * 0.5, CARD_H * 0.5);
					cairo_rectangle(cr, cardpos + (CARD_W * 0.5), 0, CARD_W * 0.5, CARD_H * 0.5);
					cairo_fill(cr);
				}

				//display player attributes
				sprintf(buffer, "Id:%d", game.players[i].ID);
				draw_text(cr, buffer, cardpos, (CARD_H * 0.75), 12.5);
				sprintf(buffer, "Balance:");
				draw_text(cr, buffer, cardpos, (CARD_H * 0.95), 12.5);
				sprintf(buffer, "$%d", game.players[i].Balance);
				draw_text(cr, buffer, cardpos, (CARD_H * 1.15), 12.5);
			}
			else
			{
				spacing = 1;
			}
		}
		}
	}

	cairo_destroy(cr);
}

GAMESTATE DoGame(GAMESTATE game) {
	//check player actions
	int validmove = 1;
	switch(game.players[game.playerTurn].action){
	case CALL:
		if(game.currCall != 0 && !(game.players[game.playerTurn].role == BIGBLIND && game.stage == PREFLOP)){
	game.players[game.playerTurn].Bid = game.currCall;
	game.players[game.playerTurn].Balance = game.players[game.playerTurn].Balance-game.currCall;
	game.pot += game.currCall;
		}else{
	validmove = 0;
		}
	break;

	case RAISE:
		game.currCall += game.players[game.playerTurn].raiseAmt;
		game.players[game.playerTurn].Bid = game.currCall;
		game.players[game.playerTurn].Balance = game.players[game.playerTurn].Balance-game.currCall;
		game.pot += game.currCall;
	break;

	case CHECK:
		//can do it better by resetting bid & checking if player is at call
		if(game.players[game.playerTurn].Bid == game.currCall || (game.players[game.playerTurn].Bid == -1 && game.currCall == 0)){
		game.players[game.playerTurn].Bid = game.currCall;
		break;
		}
		else validmove = 0;
	break;

	case FOLD:
	int count = 0;
	for (int i = 0; i < game.numberPlayers; i++) {
		if (game.players[game.numberPlayers].action != FOLD) count++;
	}
	if (count == 1) game.stage = WIN;
	break;

	default:
	validmove = 0;
	break;
	}

	if (nUnfolded(game) != 1) {
	    if(validmove == 1){
			//stage change logic
			if(EQUALBIDS(game) == 1 && (game.stage != PREFLOP || (game.stage == PREFLOP && game.players[game.playerTurn].role == BIGBLIND))){

				//find first player after smallblind
				bool foundSB = false;
				for (int i = 0; i < game.numberPlayers; i++) {
					//if haven't found small blind, continue searching
					if (!foundSB) foundSB = (game.players[i].role == SMALLBLIND);

					//if small blind or closest unfolded player to sb, make it their turn
					if (foundSB && game.players[i].action != FOLD) {
						game.playerTurn = i;
						break;
					}

					//if the smallblind is the last player and is folded
					if (game.players[i].action == FOLD && i == game.numberPlayers) i = 0;
				}

				game.currCall = 0;
				for (int i = 0; i < game.numberPlayers; i++) game.players[i].Bid = -1;
				game.stage++;
			}
			//turn incrementing logic
			else {
				while (true) {
					if (game.playerTurn < game.numberPlayers-1) game.playerTurn++;
					else (game.playerTurn = 0);

					if (game.players[game.playerTurn].action != FOLD) break;
					}
			}
		}
	}
	else game.stage = WIN;

  return game;
}

char *StageStr(STAGES s) {
	switch (s) {
		case PREFLOP:
		return "preflop";
		case FLOP:
		return "flop";
		case TURN:
		return "turn";
		case RIVER:
		return "river";
		case WIN:
		return "game over";
		default:
		return "undefined stage!";
	}
}	

char *RankStr(RANK r) {
	//looks dumb but significantly easier than alternative
	switch (r) {
		case ACE: return "ace";
		case 2: return "2";
		case 3: return "3";
		case 4: return "4";
		case 5: return "5";
		case 6: return "6";
		case 7: return "7";
		case 8: return "8";
		case 9: return "9";
		case 10: return "10";
		case JACK: return "jack";
		case QUEEN: return "queen";
		case KING: return "king";
	}
}

char *SuitStr(SUIT s) {
       switch (s) {
	       case DIAMONDS: return "diamonds";
	       case HEARTS: return "hearts";
	       case SPADES: return "spades";
		case CLUBS: return "clubs";			    
	}
}

int StageNum(STAGES s)
{
	switch (s)
	{
	case PREFLOP:
		return 0;
	case FLOP:
		return 1;
	case TURN:
		return 2;
	case RIVER:
		return 3;
	case WIN:
		return 4;
	default:
		return -2;
	}
}


static gboolean delete_event(GtkWidget *widget, GdkEvent *event, gpointer data)
{
	return FALSE;
}

static void destroy(GtkWidget *widget, gpointer data)
{
	gtk_main_quit();
}
