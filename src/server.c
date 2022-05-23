#include "deck.h"
#include <stdio.h>
#include <ctype.h>
#include <string.h>

const int STARTING_BALANCE = 500;
const int SMALL_BLIND = 5;

GAMESTATE DoGame(GAMESTATE game);
char *StageStr(STAGES s);

int main(){
	//------------------------ SETUP (Done by GTK) ------------------------------
	DECK deck = INIT();

	//initialize static members of game
	GAMESTATE game;
	//fill the player array with empty, offline players
	CARD nullCard = {-1,-1};
	PLAYER emptyPlayer = {-1, -1, 0, nullCard, nullCard, NoAction, NORMAL, 0, false};
	for (int i = 0; i < 9; i++) {
		game.players[i] = emptyPlayer;
		game.players[i].ID = i;
	}

	//initialize other game variables
	game.shuffleDeck = ShuffleCards (deck);
	game.stage = PREFLOP;
	game.playerTurn = 0;

	while (true) {	
		printf("Please enter the number of players: ");
		scanf("%d", &game.numberPlayers);
		if (game.numberPlayers > 9) printf("\nPlease enter a number between 2 and 9");
		else break;
	}
	
	//set that many players to online
	for (int i = 0; i < game.numberPlayers; i++) {
		game.players[i].online = true;
		game.players[i].Balance = STARTING_BALANCE;
		if (i == 0) game.players[i].role = SMALLBLIND;
		else if (i == 1) game.players[i].role = BIGBLIND;
	}

	

	//------------------------- EVENT LOOP (Done by GTK) ----------------------
	while (true) {
		printf("\n\nCurrent Stage: %s -- Pot: %d, Current Call: %d\n", StageStr(game.stage), game.pot, game.currCall);

		char actStr[6];
		printf("\nPlayer %d, type an action: ", game.playerTurn + 1);
		scanf("%5s", actStr);
		for (int i = 0; i < 5; i++) actStr[i] = tolower(actStr[i]);


		if (!strcmp(actStr, "fold") || !strcmp(actStr, "f")) {
			game.players[game.playerTurn].action = FOLD;
			game.players[game.playerTurn].action = FOLD;
		}
		if (!strcmp(actStr, "call") || !strcmp(actStr, "c")) {
			game.players[game.playerTurn].action = CALL;
			game.players[game.playerTurn].Bid = game.currCall; 
		}
		if (!strcmp(actStr, "check") || !strcmp(actStr, "ch")) {
			game.players[game.playerTurn].action = CHECK;
			game.players[game.playerTurn].Bid = game.currCall; 
		}
		if (!strcmp(actStr, "raise") || !strcmp(actStr, "r")) {
			game.players[game.playerTurn].action = RAISE;
			printf("\nRaise amount: ");
			scanf("%d", &game.players[game.playerTurn].raiseAmt);
		}
		
		game = DoGame(game);
	}

	return 0;
}



//only called after player move, playerTurn will point to moving player
GAMESTATE DoGame(GAMESTATE game) {
	//find next player's turn
	while (true) {
		if (game.playerTurn < 8) game.playerTurn++;
		else (game.playerTurn = 0);

		if (game.players[game.playerTurn].online) break;
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
