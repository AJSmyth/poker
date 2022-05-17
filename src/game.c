#include "deck.h"
#include "game.h"
#include <stdlib.h>
#include <stdio.h>

void PREFLOP1(GAMESTATE game, int n)
{
    int option = 0;
    int raise = 0;
    int min_bid = 10;
    int player_temp = 0;

    while (1)
    {
        if(EQUALBIDS(game, n) == 1) //Bidding goes on untill everyone has equal bids
        {
            break;
        }
        
        if (game.players[n].action == FOLD) //If a player folded, we do not ask his input again
        {
            player_temp++;
            if(player_temp == n+1)
            {
                player_temp = 0;
            }
            continue;
        }

        printf("Player %d, choose 1  for call, 2 for raise, 3 for fold: ", player_temp);
        scanf("%d",&option);

        switch (option)
        {
            case 1 : //Calling the Bid
                game.players[n].action = CALL;
                game.players[n].Bid = min_bid;
                break;

            case 2 : //Raising the bid
                game.players[n].action = RAISE;
                printf("Enter the raise amount: ");
                scanf("%d",&raise);
                min_bid = min_bid + raise;
                game.players[n].Bid = min_bid;
                break;

            case 3: //Folding
                game.players[n].action = FOLD;
                break;
        }
        player_temp++;
        if(player_temp == n+1) //To keep the loop going within the bounds
        {
            player_temp = 0;
        }

    } 
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