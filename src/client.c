#include <gtk/gtk.h>

/* This is a callback function. The data arguments are ignored
 *  * in this example. More on callbacks below. */
static void hello( GtkWidget *widget, gpointer   data ) {
	    g_print ("Hello World\n");
}

static gboolean delete_event( GtkWidget *widget, GdkEvent  *event, gpointer   data ) {
	g_print ("delete event occurred\n");
	return FALSE;
}

/* Another callback */
static void destroy(GtkWidget *widget, gpointer data) {
	    gtk_main_quit ();
}

static void paint(GtkWidget *widget, GdkEventExpose *eev, gpointer data);
						      

int main( int   argc, char *argv[] ) {
	/* GtkWidget is the storage type for widgets */
	GtkWidget *window;
	GtkWidget *button;
	GtkWidget *canvas;
	GtkWidget *vbox;

	/* This is called in all GTK applications. Arguments are parsed
	*      * from the command line and are returned to the application. */
	gtk_init (&argc, &argv);
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);

	g_signal_connect(G_OBJECT (window), "delete_event", G_CALLBACK(delete_event), NULL);
	g_signal_connect(G_OBJECT (window), "destroy", G_CALLBACK (destroy), NULL);


	button = gtk_button_new_with_label("Hello World");
	canvas = gtk_drawing_area_new();
	vbox = gtk_vbox_new(FALSE, 10);

	gtk_container_set_border_width(GTK_CONTAINER(window), 1);
	gtk_widget_set_size_request(canvas, 480, 110);

	g_signal_connect(G_OBJECT (button), "clicked", G_CALLBACK (hello), NULL);
	g_signal_connect_swapped(G_OBJECT (button), "clicked", G_CALLBACK (gtk_widget_destroy), G_OBJECT (window));
	g_signal_connect(G_OBJECT(canvas), "expose-event", G_CALLBACK(paint), NULL);

	gtk_container_add(GTK_CONTAINER(window), vbox);

	gtk_box_pack_start(vbox, canvas, FALSE, FALSE, 10);
	gtk_box_pack_start(vbox, button, FALSE, FALSE, 10);

	/* The final step is to display this newly created widget. */
	gtk_widget_show(vbox);
	gtk_widget_show(button);
	gtk_widget_show(canvas);

	/* and the window */
	gtk_widget_show(window);

	/* All GTK applications must have a gtk_main(). Control ends here
	*      * and waits for an event to occur (like a key press or
	*           * mouse event). */
	gtk_main ();

	return 0;
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
