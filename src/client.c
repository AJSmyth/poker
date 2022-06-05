//#include "deck.h"
#include "game.h"
#include <stdio.h>
#include <ctype.h>
#include <string.h>
//socket stuff
#include <sys/socket.h>	//socket
#include <arpa/inet.h>	//inet_addr
#include <unistd.h>

const int STARTING_BALANCE = 500;
const int SMALL_BLIND = 5;

GAMESTATE DoGame(GAMESTATE game);
char *StageStr(STAGES s);
char *SuitStr(SUIT s);
char *RankStr(RANK r);

int player_net_id;

int main(){
	//initialize connection
	int sock;
	int WINNER;
	struct sockaddr_in server;
	
	//Create socket
	sock = socket(AF_INET , SOCK_STREAM , 0);
	if (sock == -1)
	{
		printf("Could not create socket");
	}
	
	server.sin_addr.s_addr = inet_addr("127.0.0.1");//128.200.85.17
	server.sin_family = AF_INET;
	server.sin_port = htons( 9000 );

	//Connect to remote server
	if (connect(sock , (struct sockaddr *)&server , sizeof(server)) < 0)
	{
		perror("connect failed. Error");
		return 1;
	}
	
	printf("Connected to game server.\n");
	if(read(sock , &player_net_id , sizeof(player_net_id)) < 0){
		printf("Unable to retrieve player net ID.");
	}
	// if((player_net_id = (int) recv(sock , server_reply , sizeof(int) , 0)) < 0){
	// 	printf("Unable to retrieve player net ID.");
	// 	return 1;
	// }
	printf("Player ID: %d: ", player_net_id);
	//------------------------ SETUP (Done by GTK) ------------------------------

	printf("\n.------..------..------..------..------..------..------.     .------..------..------..------..------.\n");
	printf("|Q.--. ||U.--. ||E.--. ||E.--. ||N.--. ||'.--. ||S.--. |.-.  |P.--. ||O.--. ||K.--. ||E.--. ||R.--. |\n");
	printf("| (\\/) || (\\/) || (\\/) || (\\/) || :(): || :/\\: || :/\\: ((5)) | :/\\: || :/\\: || :/\\: || (\\/) || :(): |\n");
	printf("| :\\/: || :\\/: || :\\/: || :\\/: || ()() || :\\/: || :\\/: |'-.-.| (__) || :\\/: || :\\/: || :\\/: || ()() |\n");
	printf("| '--'Q|| '--'U|| '--'E|| '--'E|| '--'N|| '--''|| '--'S| ((1)) '--'P|| '--'O|| '--'K|| '--'E|| '--'R|\n");
	printf("`------'`------'`------'`------'`------'`------'`------'  '-'`------'`------'`------'`------'`------'\n");
	printf("\nVersion Alpha: Game Logic Test -- GUI Test available in bin/GUI, although integration is not yet finished\n");

	GAMESTATE game;
	if(player_net_id == 0){
		DECK deck = INIT();
		
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
		puts("Writing game to other players in session.\n");
		write(sock, &game, sizeof(game));
		read(sock, &game, sizeof(game));//additional read to stall the host until every player has joined
	}else{
		puts("Waiting for host to initialize game.\n");
		read(sock, &game, sizeof(game));
	}

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
		if(game.playerTurn == player_net_id){
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
			}else{
				write(sock, &game, sizeof(game));
			}
		}else{
			char out[100];
			sprintf(out, "\nWaiting for player %d to take an action...", game.playerTurn + 1);
			puts(out);
			read(sock, &game, sizeof(game));
		}
		
	}
        WINNER = Sequence_Winner(game); 
	
	if (game.SplitPot == 1)
	{
	  printf("Pot is split between Player %d and Player %d!\n",game.SplitPotPlayers[0]+1,game.SplitPotPlayers[1]+1);
	  printf("Game over\n");
	  game.players[game.SplitPotPlayers[0]].Balance =+ game.pot / 2;
	  game.players[game.SplitPotPlayers[1]].Balance =+ game.pot / 2;
	  game.SplitPot = 0;
	}
	else
	{
	  game.players[WINNER].Balance = game.players[WINNER].Balance + game.pot;
	  printf("Winner is Player %d\n", game.players[WINNER].ID+1);
	  printf("Game over\n");
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
		if(game.players[game.playerTurn].Bid == game.currCall){
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
					printf("Player %d turnasdfasdfasd", i);
					break;
				}

				//if the smallblind is the last player and is folded
				if (game.players[i].action == FOLD && i == game.numberPlayers) i = 0;
			}		
			
			game.currCall = 0;
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
