#include "deck.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

// GAMESTATE PREFLOP1(GAMESTATE game)
// {
//     int option = 0;
//     int raise = 0;
//     int min_bid = 10;
//     int x = 0;

//     while (EQUALBIDS(game) == 0)
//     {
//         if (game.players[x].action == FOLD) //If a player folded, we do not ask his input again
//         {
//             x++;
//             if(x == game.numberPlayers+1)
//             {
//                 x = 0;
//             }
//             continue;
//         }

//         printf("Player %d, choose 1  for call, 2 for raise, 3 for fold: ",x);
//         scanf("%d",&option);

//         switch (option)
//         {
//             case 1 : //Calling the Bid
//                 game.players[x].action = CALL;
//                 game.players[x].Bid = min_bid;
//                 break;

//             case 2 : //Raising the bid
//                 game.players[x].action = RAISE;
//                 printf("Enter the raise amount: ");
//                 scanf("%d",&raise);
//                 min_bid = min_bid + raise;
//                 game.players[x].Bid = min_bid;
//                 break;

//             case 3: //Folding
//                 game.players[x].action = FOLD;
//                 break;
//         }
//         x++;
//         if(x == game.numberPlayers+1) //To keep the loop going within the bounds
//         {
//             x = 0;
//         }

//     } 
//     return game;
// }

// //TODO: NEED TO TEST AND VERIFY METHOD FUNCTIONALITY -Oliver
// GAMESTATE FLOP1(GAMESTATE game) {
//     //for loop for each of the players
//     for (int i=0; i<game.numberPlayers; i++){
//     int option = 0;
//     int raise = 0;
//     int min_bid = 0;
//     int x = 0;

//     //first step assign 3 cards to community deck.
//     for (int i=0; i<3; i++) {
//         game.communityCards.cards[game.communityCards.BOTTOM] = game.shuffleDeck.cards[game.shuffleDeck.BOTTOM];
//         game.communityCards.BOTTOM ++; //add a card to the community card
//         game.shuffleDeck.BOTTOM--; //take away a card from the shuffle deck.
//     }

//     //next step, I want to get all the options for calls.
//     int option;
//     printf("Player %d, choose 1  for call, 2 for raise, 3 for fold, 4 for check: ",x);
//     scanf("%d",&option);
//     switch (option) {
//             case 1 : //Calling the Bid
//                 game.players[x].action = CALL;
//                 game.players[x].Bid = min_bid;
//                 break;

//             case 2 : //Raising the bid
//                 game.players[x].action = RAISE;
//                 printf("Enter the raise amount: ");
//                 scanf("%d",&raise);
//                 min_bid = min_bid + raise;
//                 game.players[x].Bid = min_bid;
//                 break;

//             case 3: //Folding
//                 game.players[x].action = FOLD;
//                 break;
//             case 4: //check
//                 game.players[x].action = CHECK;
//         }
//     }
// }


int EQUALBIDS(GAMESTATE game)
//This works only if player[0] did not fold (For now)
{
    int currentBid;
    int firstPlayer;
    //find the first player in play and record their bid and index
    for(int i = 0; i < game.numberPlayers; i++){
        if(game.players[game.playerTurn].action != FOLD){
            currentBid = game.players[i].bid;
            firstPlayer = i;
            break;
        }
    }
    //check every player after the first active player to see if their bids are equal to the first
    for(int i = firstPlayer+1; i < game.numberPlayers; i++){
        if(game.players[game.playerTurn].action != FOLD){
            if(game.players[i].bid != currentBid){
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

void random(int A[], int B) {
    srand(time(NULL));
 
    for(int i = B-1; i > 0; i--) {
        int j = rand() % (i+1); //Assign a random value for j within bounds
        swap(&A[i], &A[j]); //Swap two elements in the array randomly
    }
}
