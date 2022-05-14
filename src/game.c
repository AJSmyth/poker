#include "deck.h"
#include <stdlib.h>
#include <stdio.h>

void PREFLOP1(PLAYER player[], DECK deck, int n)
{
    int option = 0;
    int raise = 0;
    int min_bid = 10;
    int x = 0;

    while (1)
    {
        if(EQUALBIDS(player, n) == 1) //Bidding goes on untill everyone has equal bids
        {
            break;
        }
        
        if (player[x].action == FOLD) //If a player folded, we do not ask his input again
        {
            x++;
            if(x == n+1)
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
                player[x].action = CALL;
                player[x].Bid = min_bid;
                break;

            case 2 : //Raising the bid
                player[x].action = RAISE;
                printf("Enter the raise amount: ");
                scanf("%d",&raise);
                min_bid = min_bid + raise;
                player[x].Bid = min_bid;
                break;

            case 3: //Folding
                player[x].action = FOLD;
                break;
        }
        x++;
        if(x == n+1) //To keep the loop going within the bounds
        {
            x = 0;
        }

    } 
}


int EQUALBIDS(PLAYER player[], int n)
//This works only if player[0] did not fold (For now)
{
    if (player[0].action != FOLD) 
    {
        for(int a = 1; a <= n ; a++)
        {
            if (player[a].action == FOLD) //Not considering the bid of a folded player
                {
                   continue;
                }

            if (player[0].Bid == player[a].Bid) //Checking if all bids are equal
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