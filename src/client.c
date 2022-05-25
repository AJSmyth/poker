#include <gtk/gtk.h>
#include "client.h"
#include "deck.h"
#include "game.h"

const int STARTING_BALANCE = 500;
static gboolean delete_event(GtkWidget *widget, GdkEvent *event, gpointer data);
static void destroy(GtkWidget *widget, gpointer data);

GAMESTATE DoGame(GAMESTATE game);
char *StageStr(STAGES s);
char *SuitStr(SUIT s);
char *RankStr(RANK r);

static void startGame(GtkWidget *widget, gpointer data);
static void changePlayer(GtkWidget *widget, gpointer data);
static void doInput(GtkWidget *widget, gpointer data);
static void updateData(GtkWidget *widget, gpointer data);
static void quitGame(GtkWidget *widget, gpointer data);
static void paint(GtkWidget *widget, GdkEventExpose *eev, gpointer data);


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
    gs.numberPlayers = 5;

    for (int i = 0; i < gs.numberPlayers; i++) {
        gs.players[i].online = true;
        gs.players[i].Balance = STARTING_BALANCE;
    }

    gs = AssignCards(gs);
    game.gs = gs;

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
    game.menu.vbox = gtk_vbox_new(FALSE, 1);

    //menu signals
    g_signal_connect(G_OBJECT(menuStartBtn), "clicked", G_CALLBACK(startGame), &game);

    //menu container packing
    gtk_box_pack_start(GTK_BOX(mainVbox), game.menu.vbox, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(game.menu.vbox), menuTitle, FALSE, FALSE, 10);
    gtk_box_pack_start(GTK_BOX(game.menu.vbox), menuStartBtn, FALSE, FALSE, 10);

    gtk_widget_show(menuTitle);
    gtk_widget_show(menuStartBtn);
    gtk_widget_show(game.menu.vbox);

    //=================================== GAME =====================================    
    //initialize game widgets
    GtkWidget *gameCanvas = gtk_drawing_area_new();
    GtkWidget *gameQuit = gtk_button_new_with_label("Quit");
    GtkWidget *gameFold = gtk_button_new_with_label("Fold");
    GtkWidget *gameCall = gtk_button_new_with_label("Call");
    GtkWidget *gameCheck = gtk_button_new_with_label("Check");
    GtkWidget *gameRaise = gtk_button_new_with_label("Raise");
    //temporary labels
    game.game.commLabel = gtk_label_new("| Pot: $0 | === | Call: $0 |");
    game.game.cardLabel = gtk_label_new("Community Cards: hidden");
    game.game.playerLabel = gtk_label_new("Player Cards: n/a");
    GtkAdjustment *playerNoAdj = GTK_ADJUSTMENT(gtk_adjustment_new(1,1,game.gs.numberPlayers,1,5,0));
    game.game.playerBtn = gtk_spin_button_new(playerNoAdj, 0.0, 0);
    //temporary labels
    GtkAdjustment *gameBetAdj = GTK_ADJUSTMENT(gtk_adjustment_new(1,0,1000,1,5,0));
    game.game.raiseBtn = gtk_spin_button_new(gameBetAdj, 0.1, 0);
    game.game.vbox = gtk_vbox_new(FALSE, 1);
    game.game.mainLabel = gtk_label_new("Ur move dumbass");
    GtkWidget *gameHboxBtn = gtk_hbox_new(TRUE, 1);
    gtk_widget_set_size_request(gameCanvas, 640, 420);

    //game signals
    g_signal_connect(G_OBJECT(gameCanvas), "expose-event", G_CALLBACK(paint), NULL);
    g_signal_connect(G_OBJECT(gameFold), "clicked", G_CALLBACK(doInput), &game);
    g_signal_connect(G_OBJECT(gameCall), "clicked", G_CALLBACK(doInput), &game);
    g_signal_connect(G_OBJECT(gameCheck), "clicked", G_CALLBACK(doInput), &game);
    g_signal_connect(G_OBJECT(gameRaise), "clicked", G_CALLBACK(doInput), &game);
    g_signal_connect(G_OBJECT(gameQuit), "clicked", G_CALLBACK(quitGame), &game);
    g_signal_connect(G_OBJECT(game.game.playerBtn), "value-changed", G_CALLBACK(changePlayer), &game);

    //game container packing
    gtk_box_pack_start(GTK_BOX(mainVbox), game.game.vbox, FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(game.game.vbox), gameCanvas, FALSE, FALSE, 0);
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

    gtk_box_pack_start(GTK_BOX(game.game.vbox), game.game.playerBtn, TRUE, TRUE, 0);

    gtk_widget_show(gameCanvas);
    gtk_widget_show(gameQuit);
    gtk_widget_show(gameFold);
    gtk_widget_show(gameCall);
    gtk_widget_show(gameCheck);
    gtk_widget_show(gameRaise);
    gtk_widget_show(game.game.raiseBtn);
    //temporary labels
    gtk_widget_show(game.game.playerLabel);
    gtk_widget_show(game.game.commLabel);
    gtk_widget_show(game.game.cardLabel);
    gtk_widget_show(game.game.playerBtn);
    //temporary labels
    gtk_widget_show(gameHboxBtn);

    gtk_widget_show(mainVbox);
    gtk_widget_show(window);
    
    updateData(NULL, &game);
    gtk_main();
    return 0;
}



void updateData(GtkWidget *widget, gpointer data) {
    Game *g = data;
    GAMESTATE game = g->gs;

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
    gtk_label_set_text(GTK_LABEL(g->game.playerLabel), temp); 
}



void doInput(GtkWidget *widget, gpointer data) {
    Game *g = data;
    GAMESTATE game = g->gs;
    
    if (game.playerTurn == g->ID) {
    updateData(NULL, data);
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
            if (g->gs.playerTurn == tturn && g->gs.stage != tstage) {
                gtk_label_set_text(GTK_LABEL(g->game.mainLabel), "Invalid move!");
                gtk_widget_show(g->game.mainLabel);
            }
            //if valid move hide the error message
            else gtk_widget_hide(g->game.mainLabel);
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
    updateData(NULL, data);
} 



static void startGame(GtkWidget *widget, gpointer data) {
    Game *game = data;
    gtk_widget_hide(game->menu.vbox);
    gtk_widget_show(game->game.vbox);
    game->state = GAME;
    
    //connect to server
    //
    //
}



static void quitGame(GtkWidget *widget, gpointer data) {
    Game *game = data;
    gtk_widget_hide(game->game.vbox);
    gtk_widget_show(game->menu.vbox);
    game->state = MENU;
}



static void changePlayer(GtkWidget *widget, gpointer data) {
    Game *g = data;
    g->ID = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(widget)) - 1;
    updateData(NULL, data);
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
    cairo_set_source_rgb(cr,0,0.2,0);
    cairo_set_line_width(cr, 1);
    cairo_rectangle(cr, 0, 0, width, height);
    cairo_stroke_preserve(cr);
    cairo_fill(cr);

    cairo_set_font_size (cr, 40);
    cairo_move_to (cr, 40, 60);
    cairo_set_source_rgb (cr, 0,0,0);
    cairo_show_text (cr, "Queen's Poker");
    cairo_restore (cr);

    // cairo_set_source_rgb (cr, 0,0,1);
    cairo_move_to (cr, 40, 70);
    for (i=0; i< width/10; i++)
    {
    cairo_rel_line_to (cr, 5,  10);
    cairo_rel_line_to (cr, 5, -10);
    }
    cairo_stroke (cr);

    cairo_move_to (cr, 0, 70);
    for (i=0; i< 5; i++)
    {
        cairo_set_source_rgb (cr, 0,0,0);
        cairo_line_to (cr, 0.5, 0);
        cairo_rel_line_to (cr, 0.25, -1);
        cairo_rel_line_to (cr, 0.25, -1);
        cairo_rel_line_to (cr, 0.25, -1);
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
      if(game.players[game.playerTurn].role == SMALLBLIND && game.stage != PREFLOP){
        game.players[game.playerTurn].Bid = 0;
        break;
      }
      else if(game.players[game.playerTurn].role == BIGBLIND && game.stage == PREFLOP) {
    //bet is set to 0 in setup to avoid premature progression
        game.players[game.playerTurn].Bid = 10;
    break;
      }
      
      if (game.currCall == 0) game.players[game.playerTurn].Bid = 0;
      else if (game.players[game.playerTurn].Bid != game.currCall) validmove = 0;
    break;

    case FOLD:
        validmove = 1;
    int count;
    for (int i = 0; i < game.numberPlayers; i++) {
        if (game.players[game.numberPlayers].action != FOLD) count++;
    }
    if (count == 1) game.stage = WIN;
    break;

    default:
        validmove = 0;
    break;
  }

  //stage change logic
  if(!(game.players[game.playerTurn].role == SMALLBLIND && game.players[game.playerTurn].action == FOLD) && EQUALBIDS(game) == 1 && validmove == 1){
    if(game.stage != RIVER){
      //set all bets to -1 to avoid premature progression
      for (int i = 0; i < game.numberPlayers; i++) game.players[i].Bid = -1;

      game.playerTurn = 0;
      game.currCall = 0;
      game.stage++;
    }else{
      printf("Game over");
      game.stage = WIN;
      //end game/restart
    }
  }

  //turn incrementing logic
  else {
      if(validmove == 1){
        while (true) {
          if (game.playerTurn < game.numberPlayers-1) game.playerTurn++;
          else (game.playerTurn = 0);

          if (game.players[game.playerTurn].action != FOLD) break;
          }
      }
  }

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

