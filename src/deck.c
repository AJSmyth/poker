#include "deck.h"
#include <stdlib.h>
#include <stdio.h>

DECK INIT(){
    DECK deck;
    int count = 0;
    for(SUIT s = 0; s <= 3; s++){
        for(int r = 1; r <= 13; r++){
            deck.cards[count].suit = s;
            deck.cards[count].rank = r;
            printf("%d %d\n" , deck.cards[count].suit, deck.cards[count].rank);
            count++;
            

        }
    }
    return deck;
}

DECK ShuffleCards (DECK deck)
{
    DECK shuffle;
    int a[52];
    int x = 0;

    for (int X = 0 ; X < 53 ; X++) //Asssign values from 0-52 in an Array
    {
        a[X] = X;
    }

    random(a,51); //Randomise the order of numbers in the array 

    for (int i = 0 ; i <= 51 ; i++) //Repeat for 52 cards
    {
        shuffle.cards[i] = deck.cards[a[i]]; //Shuffle the cards
    }
    return shuffle;
}

void AssignCards (PLAYER player[], DECK deck, int n, GAMESTATE G)
{
    int i,x;
    PLAYER P[n+1]; //Temporary variable

    for (i = 0, x = 0; i <= 4; i++, x=x+2)
    {
        player[i].card1 = deck.cards[x]; //Assign 1st Card to Player[i]
        player[i].card2 = deck.cards[x+1]; //Assign 2nd Card to Player[i]
    }

    for(int a = 0; a<= 41 - 10;a++)
    {
        deck.cards[a] = deck.cards[a+10]; //Changing the deck by removing the assigned cards
    }
    
    for (int a = 0; a <= n ; a++) //Making P equal to Player as a temporary variable
    {
        P[a] = player[a];
    }
    
    if (G.stage = PREFLOP)
    {
        player[0].role = 0; //Assigning small blind to player[0]
        player[0].Bid = 5; //Assigning default bid on small blind
        player[1].role = 1; //Assigning big blind to player[1]
        player[1].Bid = 10; //Assigning default bid on big blind
        for (int a = 2; a <= n ; a++)
        {
            player[a].role = 2; //Assigning other players ar normal players
            player[a].Bid = 0; //Assigning 0 bid for non-Blind player
        }
    }

    else 
    {
        for (int a = 0; a <= n ; a++) //Moving the player roles[Blinds] and assigning 
        {                             //default bids on binds, at the start of the round   
            if (a > 0)
          {
                player[a].role = P[a-1].role;
            }

            else 
            {
                player[a].role = P[n].role;
            }
        }
    }
    //return deck;
}

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

