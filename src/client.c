#include <gtk/gtk.h>
#include "client.h"

static gboolean delete_event(GtkWidget *widget, GdkEvent *event, gpointer data);
static void destroy(GtkWidget *widget, gpointer data);

static void startGame(GtkWidget *widget, gpointer data);
static void paint(GtkWidget *widget, GdkEventExpose *eev, gpointer data);


int main( int   argc, char *argv[] ) {
	MenuObjects m = { NULL };
	GameObjects g = { NULL };
	Game game = { MENU, m, g };

	/* GtkWidget is the storage type for widgets */
	GtkWidget *window;
	GtkWidget *mainVbox;

	gtk_init (&argc, &argv);
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_container_set_border_width(GTK_CONTAINER(window), 1);
	g_signal_connect(G_OBJECT(window), "delete_event", G_CALLBACK(delete_event), NULL);
	g_signal_connect(G_OBJECT(window), "destroy", G_CALLBACK(destroy), NULL);
	mainVbox = gtk_vbox_new(FALSE, 1);
	gtk_container_add(GTK_CONTAINER(window), mainVbox);

	//=================================== MENU ====================================
	//initialize menu widgets
	GtkWidget *menuTitle = gtk_label_new("Queen's Poker");
	GtkWidget *menuStartBtn = gtk_button_new_with_label("Start");
	game.menu.vbox = gtk_vbox_new(FALSE, 1);

	//menu signals
	g_signal_connect(G_OBJECT(menuStartBtn), "clicked", G_CALLBACK(startGame), &game);

	//menu container packing
	gtk_box_pack_start(mainVbox, game.menu.vbox, FALSE, FALSE, 0);
	gtk_box_pack_start(game.menu.vbox, menuTitle, FALSE, FALSE, 10);
	gtk_box_pack_start(game.menu.vbox, menuStartBtn, FALSE, FALSE, 10);

	gtk_widget_show(menuTitle);
	gtk_widget_show(menuStartBtn);
	gtk_widget_show(game.menu.vbox);

	//=================================== GAME =====================================	
	//initialize game widgets
	GtkWidget *gameCanvas = gtk_drawing_area_new();
	game.game.vbox = gtk_vbox_new(FALSE, 1);
	gtk_widget_set_size_request(gameCanvas, 480, 120);

	//game signals
	g_signal_connect(G_OBJECT(gameCanvas), "expose-event", G_CALLBACK(paint), NULL);

	//game container packing
	gtk_box_pack_start(mainVbox, game.game.vbox, FALSE, FALSE, 0);
	gtk_box_pack_start(game.game.vbox, gameCanvas, FALSE, FALSE, 10);

	gtk_widget_show(gameCanvas);

	gtk_widget_show(mainVbox);
	gtk_widget_show(window);

	gtk_main();
	return 0;
}



static void startGame(GtkWidget *widget, gpointer data) {
	Game *game = data;
	gtk_widget_hide(game->menu.vbox);
	gtk_widget_show(game->game.vbox);
	game->state = GAME;
}



static void paint(GtkWidget *widget, GdkEventExpose *eev, gpointer data) {
	gint width, height;
	gint i;
	cairo_t *cr;

	width  = widget->allocation.width;
	height = widget->allocation.height;

	cr = gdk_cairo_create (widget->window);

	/* clear background */
	cairo_set_source_rgb (cr, 1,1,1);
	cairo_paint (cr);


	cairo_select_font_face (cr, "Sans", CAIRO_FONT_SLANT_NORMAL,
	CAIRO_FONT_WEIGHT_BOLD);

	/* enclosing in a save/restore pair since we alter the
	*      * font size
	*           */
	cairo_save (cr);
	cairo_set_font_size (cr, 40);
	cairo_move_to (cr, 40, 60);
	cairo_set_source_rgb (cr, 0,0,0);
	cairo_show_text (cr, "Hello World");
	cairo_restore (cr);

	cairo_set_source_rgb (cr, 1,0,0);
	cairo_set_font_size (cr, 20);
	cairo_move_to (cr, 50, 100);
	cairo_show_text (cr, "greetings from gtk and cairo");

	cairo_set_source_rgb (cr, 0,0,1);

	cairo_move_to (cr, 0, 150);
	for (i=0; i< width/10; i++)
	{
	cairo_rel_line_to (cr, 5,  10);
	cairo_rel_line_to (cr, 5, -10);
	}
	cairo_stroke (cr);

	cairo_destroy (cr);

}

static gboolean delete_event(GtkWidget *widget, GdkEvent *event, gpointer data) {
	return FALSE;
}

static void destroy(GtkWidget *widget, gpointer data) {
	gtk_main_quit ();
}
