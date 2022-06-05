#include "deck.h"
#include "game.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

int EQUALBIDS(GAMESTATE game)
{
    int currentBid;
    int firstPlayer;
    //find the first player in play and record their bid and index
    for(int i = 0; i < game.numberPlayers; i++){
        if(game.players[game.playerTurn].action != FOLD){
            currentBid = game.players[i].Bid;
            firstPlayer = i;
            break;
        }
    }
    //check every player after the first active player to see if their bids are equal to the first
    for(int i = firstPlayer+1; i < game.numberPlayers; i++){
        if(game.players[game.playerTurn].action != FOLD){
            if(game.players[i].Bid != currentBid){
                return 0;
            }
        }
    }

    return 1;
}


void swap(int *a, int *b) {
    int X = *a;
    *a = *b;
    *b = X;
}

void randomSwap(int A[], int B) {
    srand(time(NULL));
 
    for(int i = B-1; i > 0; i--) {
        int j = rand() % (i+1); //Assign a random value for j within bounds
        swap(&A[i], &A[j]); //Swap two elements in the array randomly
    }
}

int nUnfolded(GAMESTATE game) {
  int count = 0;
  for (int i = 0; i < game.numberPlayers; ++i) {
    if (game.players[i].action != FOLD) count++;
  }
  return count;
}
