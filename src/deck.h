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
 JACK = 10,
 QUEEN = 11,
 KING = 12
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
CARD deck[52];
int TOP;
}DECK;

typedef struct{
int Balance;
int Bid;
CARD card1;
CARD card2;
}PLAYER;

typedef struct{
STAGES stage;
}GAMESTATE;
