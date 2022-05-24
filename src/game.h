#include <stdlib.h>
#include <stdbool.h>

GAMESTATE PREFLOP1(GAMESTATE game); //Bidding in PreFlop
GAMESTATE FLOP1(GAMESTATE game);
int EQUALBIDS(GAMESTATE game); //Checking if all bids are equal to proceed further

PLAYER Sequence_Winner (GAMESTATE game);
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

int LargestinArray (int A[], int B);
DECK SortbyRank(DECK D);
DECK SortbySuit (DECK D);

void swap(int *a, int *b);
void random(int arr[], int n);
