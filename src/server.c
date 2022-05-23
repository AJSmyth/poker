#include "deck.h"
#include "game.h"
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
  game.GameCount = 0;
	game.stage = PREFLOP;

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
	}

  game = AssignCards(game);

	//------------------------- EVENT LOOP (Done by GTK) ----------------------
	while (true) {
		printf("\n\nCurrent Stage: %s -- Pot: %d, Current Call: %d\n", StageStr(game.stage), game.pot, game.currCall);

		char actStr[6];
		printf("\nPlayer %d, type an action: ", game.playerTurn + 1);
		scanf("%5s", actStr);
		for (int i = 0; i < 5; i++) actStr[i] = tolower(actStr[i]);


		if (!strcmp(actStr, "fold") || !strcmp(actStr, "f")) {
			game.players[game.playerTurn].action = FOLD;
		}
		if (!strcmp(actStr, "call") || !strcmp(actStr, "c")) {
			game.players[game.playerTurn].action = CALL;
		}
		if (!strcmp(actStr, "check") || !strcmp(actStr, "ch")) {
			game.players[game.playerTurn].action = CHECK;
		}
		if (!strcmp(actStr, "raise") || !strcmp(actStr, "r")) {
			game.players[game.playerTurn].action = RAISE;
			printf("\nRaise amount: ");
			scanf("%d", &game.players[game.playerTurn].raiseAmt);
		}
		int tempturn = game.playerTurn;
		game = DoGame(game);
    if(game.playerTurn==tempturn){
      printf("Invalid Move!");
    }
	}

	return 0;
}



//only called after player move, playerTurn will point to moving player
GAMESTATE DoGame(GAMESTATE game) {
  //check player actions
  int validmove = 1;
  switch(game.players[game.playerTurn].action){
    case CALL:
      if(game.currCall != 0){
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
      if(game.players[game.playerTurn].role == SMALLBLIND){
        game.players[game.playerTurn].Bid = 0;
        break;
      }
      int curr;
      while (true) {
        curr = game.playerTurn; 
        if (curr-1 >= 0) curr--;
        else (curr = game.numberPlayers);

        if (game.players[curr].action == FOLD) break;
      
      }
      if (game.players[curr].action == CHECK){
        game.players[game.playerTurn].Bid = 0;
      }else{
        validmove = 0;
      }
    break;
    default:
    break;
  }
  if(!(game.players[game.playerTurn].role == SMALLBLIND && game.players[game.playerTurn].action == FOLD) && EQUALBIDS(game) == 1){
    //stage change logic
    if(game.stage != RIVER){
      game.stage++;
    }else{
      //end game/restart
    }
  }
	//find next player's turn
  if(validmove == 1){
    while (true) {
      if (game.playerTurn < game.numberPlayers-1) game.playerTurn++;
      else (game.playerTurn = 0);

      if (game.players[game.playerTurn].action != FOLD) break;
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
