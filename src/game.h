#ifndef GAME_H
#define GAME_H
#include <stdlib.h>
#include <stdbool.h>

GAMESTATE PREFLOP1(GAMESTATE game); //Bidding in PreFlop
GAMESTATE FLOP1(GAMESTATE game);
int EQUALBIDS(GAMESTATE game); //Checking if all bids are equal to proceed further

void swap(int *a, int *b);
void randomSwap(int arr[], int n);

typedef enum{
    GS_UPDATE = 1,
    GS_REQUEST = 2
}PacketType;
#endif
