#ifndef GAME_H
#define GAME_H
#include <stdlib.h>
#include <stdbool.h>

GAMESTATE PREFLOP1(GAMESTATE game); //Bidding in PreFlop
GAMESTATE FLOP1(GAMESTATE game);
int EQUALBIDS(GAMESTATE game); //Checking if all bids are equal to proceed further
int nUnfolded(GAMESTATE game);

void swap(int *a, int *b);
void randomSwap(int arr[], int n);
int CheckPlayer (GAMESTATE game, int PlayerNumber);

int IsRoyalFlush (GAMESTATE game, int PlayerNumber);
int IsStraightFlush(GAMESTATE game, int PlayerNumber);
int IsFourofaKind (GAMESTATE game, int PlayerNumber);
int IsFullHouse(GAMESTATE game, int PlayerNumber);
int IsFlush(GAMESTATE game, int PlayerNumber);
int IsStraight (GAMESTATE game, int PlayerNumber);
int IsThreeofaKind (GAMESTATE game, int PlayerNumber);
int IsTwoPair (GAMESTATE game, int PlayerNumber);
int IsOnePair (GAMESTATE game, int PlayerNumber);
int IsHighCard(GAMESTATE game, int PlayerNumber);
int findWeiner(GAMESTATE game);
GAMESTATE distributePot(GAMESTATE game);

int LargestinArray (int A[], int B);
DECK SortbyRank(DECK D);
DECK SortbySuit (DECK D);

typedef enum{
    GS_UPDATE = 1,
    GS_REQUEST = 2
}PacketType;
	
#endif
