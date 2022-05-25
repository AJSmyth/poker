#include "deck.h"
#include "game.h"
#include <stdio.h>
#include <ctype.h>
#include <string.h>

const int STARTING_BALANCE = 500;
const int SMALL_BLIND = 5;

GAMESTATE DoGame(GAMESTATE game);
char *StageStr(STAGES s);
char *SuitStr(SUIT s);
char *RankStr(RANK r);

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

	printf("\n.------..------..------..------..------..------..------.     .------..------..------..------..------.\n");
	printf("|Q.--. ||U.--. ||E.--. ||E.--. ||N.--. ||'.--. ||S.--. |.-.  |P.--. ||O.--. ||K.--. ||E.--. ||R.--. |\n");
	printf("| (\\/) || (\\/) || (\\/) || (\\/) || :(): || :/\\: || :/\\: ((5)) | :/\\: || :/\\: || :/\\: || (\\/) || :(): |\n");
	printf("| :\\/: || :\\/: || :\\/: || :\\/: || ()() || :\\/: || :\\/: |'-.-.| (__) || :\\/: || :\\/: || :\\/: || ()() |\n");
	printf("| '--'Q|| '--'U|| '--'E|| '--'E|| '--'N|| '--''|| '--'S| ((1)) '--'P|| '--'O|| '--'K|| '--'E|| '--'R|\n");
	printf("`------'`------'`------'`------'`------'`------'`------'  '-'`------'`------'`------'`------'`------'\n");
	printf("\nVersion Alpha: Game Logic Test -- GUI Test available in bin/GUI, although integration is not yet finished\n");

	while (true) {	
		printf("\nPlease enter the number of players: ");
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
	printf("\nPlayer %d has small blind", 1);

	//------------------------- EVENT LOOP (Done by GTK) ----------------------
	while (game.stage != WIN) {
		printf("\n\nCurrent Stage: %s -- Pot: %d, Current Call: %d", StageStr(game.stage), game.pot, game.currCall);
		//print community cards if applicable
		if (game.stage >= FLOP) {
			printf("\nCommunity Cards: ");
			int nCards = 3 + (game.stage - FLOP);
			for (int i = 0; i < nCards; i++) {
				printf("%s of %s", RankStr(game.communityCards.cards[i].rank), SuitStr(game.communityCards.cards[i].suit));
				if (i != nCards - 1) printf(", ");
			}
		}


		char actStr[6];
		CARD c1 = game.players[game.playerTurn].card1;
		CARD c2 = game.players[game.playerTurn].card2;
		printf("\nPlayer %d --  Cards: %s of %s, %s of %s -- Balance: %d", game.playerTurn + 1, RankStr(c1.rank), SuitStr(c1.suit), RankStr(c2.rank), SuitStr(c2.suit), game.players[game.playerTurn].Balance);
		printf("\nPlayer %d, type an action: ", game.playerTurn + 1);
		scanf("%5s", actStr);
		for (int i = 0; i < 5; i++) actStr[i] = tolower(actStr[i]);


		if (!strcmp(actStr, "fold") || !strcmp(actStr, "f")) {
			game.players[game.playerTurn].action = FOLD;
		}
		else if (!strcmp(actStr, "call") || !strcmp(actStr, "c")) {
			game.players[game.playerTurn].action = CALL;
		}
		else if (!strcmp(actStr, "check") || !strcmp(actStr, "ch")) {
			game.players[game.playerTurn].action = CHECK;
		}
		else if (!strcmp(actStr, "raise") || !strcmp(actStr, "r")) {
			game.players[game.playerTurn].action = RAISE;
			printf("\nRaise amount: ");
			scanf("%d", &game.players[game.playerTurn].raiseAmt);
		}
		else {
			game.players[game.playerTurn].action = NoAction;
		}

		int tempturn = game.playerTurn;
		STAGES tempstage = game.stage;
		game = DoGame(game);
		if(game.playerTurn == tempturn && game.stage == tempstage){
			printf("\nInvalid Move!");
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
  if(EQUALBIDS(game) == 1 && validmove == 1){
    if(game.stage != RIVER){
      //set all bets to -1 to avoid premature progression
      for (int i = 0; i < game.numberPlayers; i++) game.players[i].Bid = -1;

      //find first player after smallblind
      bool foundSB = false;
      for (int i = 0; i < game.numberPlayers; i++) {
	      //if haven't found small blind, continue searching
	      if (!foundSB) foundSB = (game.players[i].role == SMALLBLIND);

	      //if small blind or closest unfolded player to sb, make it their turn
	      if (foundSB && game.players[i].action != FOLD) {
		      game.playerTurn = i;
		      printf("Player %d turnasdfasdfasd", i);
		      break;
	      }

	      //if the smallblind is the last player and is folded
	      if (game.players[i].role == SMALLBLIND && game.players[i].action == FOLD && i == game.numberPlayers) i = 0;
      }		
      
      game.currCall = 0;
      game.stage++;
    }else{
      PLAYER P;
      P = Sequence_Winner(game); 
      printf("Winner is Player %d\n", P.ID);
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
