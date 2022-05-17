#include <stdlib.h>
#include <stdbool.h>

void PREFLOP1(GAMESTATE, int n); //Bidding in PreFlop
int EQUALBIDS(GAMESTATE game, int n); //Checking if all bids are equal to proceed further
void swap(int *a, int *b);
void random(int arr[], int n);