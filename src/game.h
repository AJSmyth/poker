#include <stdlib.h>
#include <stdbool.h>

GAMESTATE PREFLOP1(GAMESTATE game); //Bidding in PreFlop
GAMESTATE FLOP1(GAMESTATE game);
int EQUALBIDS(PLAYER player[], int n); //Checking if all bids are equal to proceed further

void swap(int *a, int *b);
void random(int arr[], int n);
