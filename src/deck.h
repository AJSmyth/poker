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
 FOLD,
 CALL,
 CHECK,
 RAISE
}ACTIONS;

typedef struct{ //CARD is a struct with variables of a unique suit and rank
 SUIT suit;
 int rank;
}CARD;

typedef struct{ // DECK is a struct of a static CARD object which is an array of 52 CARD types
CARD cards[52];
int TOP;
}DECK;

typedef struct{
int Balance;
int Bid;
CARD card1;
CARD card2;
ACTIONS action;
}PLAYER;

typedef struct{
STAGES stage;
}GAMESTATE;

DECK INIT();
DECK ShuffleCards (DECK deck);
DECK AssignCards (PLAYER player[],DECK deck);
void swap(int *a, int *b);
void random(int arr[], int n);


