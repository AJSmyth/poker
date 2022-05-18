#include <stdlib.h>
#include <stdbool.h>

typedef enum{ // 4 suits in a deck of cards
 SPADES = 0,
 CLUBS = 1,
 HEARTS = 2,
 DIAMONDS = 3
}SUIT;

typedef enum{ //Face cards are assigned value for assignment purposes
 ACE = 1,
 JACK = 11,
 QUEEN = 12,
 KING = 13
}RANK;

typedef enum{
 PREFLOP,
 FLOP,
 TURN,
 RIVER
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
CARD card1;
CARD card2;
ACTIONS action;
ROLE role;
}PLAYER;

typedef struct{
STAGES stage;
PLAYER players[5];
DECK communityCards;
int pot;
int numberPlayers;
DECK shuffleDeck;
int GameCount;
} GAMESTATE;

DECK INIT();
DECK ShuffleCards (DECK deck); //Shuffles the deck 
GAMESTATE AssignCards (GAMESTATE G); //Assigns Cards and Blinds to players
GAMESTATE PREFLOP1(GAMESTATE game); //Bidding in PreFlop
int EQUALBIDS(PLAYER player[], int n); //Checking if all bids are equal to proceed further

void swap(int *a, int *b);
void random(int arr[], int n);

