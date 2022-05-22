#include "deck.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>

GAMESTATE PREFLOP1(GAMESTATE game)
{
    int option = 0;
    int raise = 0;
    int min_bid = 10;
    int x = 0;

    while (1)
    {
        if(EQUALBIDS(game.players, game.numberPlayers) == 1) //Bidding goes on untill everyone has equal bids
        {
            break;
        }
        
        if (game.players[x].action == FOLD) //If a player folded, we do not ask his input again
        {
            x++;
            if(x == game.numberPlayers+1)
            {
                x = 0;
            }
            continue;
        }

        printf("Player %d, choose 1  for call, 2 for raise, 3 for fold: ",x);
        scanf("%d",&option);

        switch (option)
        {
            case 1 : //Calling the Bid
                game.players[x].action = CALL;
                game.players[x].Bid = min_bid;
                break;

            case 2 : //Raising the bid
                game.players[x].action = RAISE;
                printf("Enter the raise amount: ");
                scanf("%d",&raise);
                min_bid = min_bid + raise;
                game.players[x].Bid = min_bid;
                break;

            case 3: //Folding
                game.players[x].action = FOLD;
                break;
        }
        x++;
        if(x == game.numberPlayers+1) //To keep the loop going within the bounds
        {
            x = 0;
        }

    } 
    return game;
}

GAMESTATE FLOP(GAMESTATE game) {
    
}


int EQUALBIDS(GAMESTATE game, int n)
//This works only if player[0] did not fold (For now)
{
    if (game.players[n].action != FOLD) 
    {
        for(int a = 1; a <= n ; a++)
        {
            if (game.players[n].action == FOLD) //Not considering the bid of a folded player
                {
                   continue;
                }

            if (game.players[a].Bid == game.players[a].Bid) //Checking if all bids are equal
                {
                if (a == n)
                    {
                       return 1; //Returning 1 if all bids are equal
                    }

                else
                    {
                        continue;
                    }
                }

            else
            {
                return 0; //Returning 0 if even one of the bids are not equal
            }
        }
    }
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
