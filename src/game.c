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


DECK SortbyRank(DECK D)
{
    CARD X;
    int i, j;
    for(i=0;i<7;i++)
    {
        for (j=0 ; j<7-i-1; j++)
        {
            if(D.cards[j].rank > D.cards[j+1].rank)
            {
                X = D.cards[j];
                D.cards[j] = D.cards[j+1];
                D.cards[j+1] = X;
            }
        }
        
    }
    for (int i=0;i<7;i++)
    {
        printf("Card %d Rank = %d\n", i, D.cards[i].rank);
    }
    return D;
}

int IsRoyalFlush (GAMESTATE game, int PlayerNumber)
{return 0;}
int IsStraightFlush(GAMESTATE game, int PlayerNumber){return 0;}
int IsFourofaKind (GAMESTATE game, int PlayerNumber){return 0;}
int IsFullHouse(GAMESTATE game, int PlayerNumber){return 0;}
int IsStraight (GAMESTATE game, int PlayerNumber){return 0;}
int IsThreeofaKind (GAMESTATE game, int PlayerNumber){return 0;}
int IsTwoPair (GAMESTATE game, int PlayerNumber){return 0;}
int IsOnePair (GAMESTATE game, int PlayerNumber){return 0;}

int IsFlush(GAMESTATE game, int PlayerNumber)
{
    DECK NewDeck;
    int i, a, b, c, d, e, f, g;
    for (i = 0; i< 5; i++)
    {
        NewDeck.cards[i] = game.communityCards.cards[i];
    }
    NewDeck.cards[5] = game.players[PlayerNumber].card1;

    NewDeck.cards[6] = game.players[PlayerNumber].card2;

    NewDeck = SortbyRank(NewDeck);

    a = NewDeck.cards[0].rank;
    b = NewDeck.cards[1].rank;
    c = NewDeck.cards[2].rank;
    d = NewDeck.cards[3].rank;
    e = NewDeck.cards[4].rank;
    f = NewDeck.cards[5].rank;
    g = NewDeck.cards[6].rank;

    if ((e == d+1 && d == c+1 && c == b+1 && b == a+1) || (f == e+1 && e == d+1 && d == c+1 && c == b+1) || (g == f+1 && f == e+1 && e == d+1 && d == c+1))
    {
        return 1;
    }
    else 
    {
        return 0;
    }
    return 0;
} 

int CheckPlayer (GAMESTATE game, int PlayerNumber)
{
    int priority;

    if (IsRoyalFlush(game,PlayerNumber) == 1)
    {
        priority = 10;
    }
    
    else if (IsStraightFlush(game,PlayerNumber) == 1)
    {
        priority = 9;
    }

    else if (IsFourofaKind (game,PlayerNumber) == 1)
    {
        priority = 8;
    }

    else if (IsFullHouse(game,PlayerNumber) == 1)
    {
        priority = 7;
    }
    
    else if (IsFlush (game,PlayerNumber) == 1)
    {
        priority = 6;
    }

    else if (IsStraight (game,PlayerNumber) == 1)
    {
        priority = 5;
    }

    else if (IsThreeofaKind (game,PlayerNumber) == 1)
    {
        priority = 4;
    }

    else if (IsTwoPair (game,PlayerNumber) ==1)
    {
        priority = 3;
    }

    else if (IsOnePair (game,PlayerNumber) ==1)
    {
        priority = 2;
    }

    else 
    {
        priority = 1;
    }

    return priority;
}

int LargestinArray (int A[], int B)
{
    int i;
    int max = 1;
 
    for (i = 0; i < B; i++)
        if (A[i] > max)
        {
            max = i;
        }
            
    return max;
}

PLAYER Sequence_Winner (GAMESTATE game)
{
    int A[9], i;
    for (i = 0; i < game.numberPlayers; i++)
    {
        //if(game.players[i].action != FOLD)
        {
            A[i] = CheckPlayer(game, i);
        }
    }
    int Winner = LargestinArray (A, i);

    return game.players[Winner];
}



void random(int A[], int B) {
    srand(time(NULL));
 
    for(int i = B-1; i > 0; i--) {
        int j = rand() % (i+1); //Assign a random value for j within bounds
        swap(&A[i], &A[j]); //Swap two elements in the array randomly
    }
}

