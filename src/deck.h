#ifndef GAME_H
#define GAME_H
#include <stdlib.h>
#include <stdbool.h>


typedef enum{ // 4 suits in a deck of cards
 SPADES = 0,
 CLUBS = 1,
 HEARTS = 2,
 DIAMONDS = 3
}SUIT;

typedef enum{ //Face cards are assigned value for assignment purposes
 ACE = 14,
 JACK = 11,
 QUEEN = 12,
 KING = 13
}RANK;

typedef enum{
 PREFLOP = 0,
 FLOP = 1,
 TURN = 2,
 RIVER = 3,
 WIN = -1
}STAGES;

typedef enum{
 SMALLBLIND = 0,
 BIGBLIND = 1,
 NORMAL = 2
}ROLE;

typedef enum{
 FOLD,
 CALL,
 CHECK,
 RAISE,
 NoAction
}ACTIONS;

typedef struct{ //CARD is a struct with variables of a unique suit and rank
 SUIT suit;
 int rank;
}CARD;

typedef struct{ // DECK is a struct of a static CARD object which is an array of 52 CARD types
CARD cards[52];
int BOTTOM;
}DECK;

typedef struct{
int Balance;
int Bid;
int raiseAmt;
CARD card1;
CARD card2;
ACTIONS action;
ROLE role;
int ID;
bool online;
}PLAYER;

typedef struct{
STAGES stage;
PLAYER players[9];
DECK shuffleDeck;
DECK communityCards;
int pot;
int currCall;
int numberPlayers;
int playerTurn;
int GameCount;
} GAMESTATE;

DECK INIT();
DECK ShuffleCards (DECK deck); //Shuffles the deck 
GAMESTATE AssignCards (GAMESTATE G); //Assigns Cards and Blinds to players

#endif