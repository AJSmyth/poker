#include "deck.h"
#include "game.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>


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
    
    for(int i = 0; i < game.numberPlayers; i++){
        if(game.players[i].action != FOLD){
            currentBid = game.players[i].Bid;
            firstPlayer = i;
            break;
        }
    }
    //check every player after the first active player to see if their bids are equal to the first
    for(int i = firstPlayer+1; i < game.numberPlayers; i++){
        if(game.players[i].action != FOLD && game.players[i].Bid != currentBid) return 0; 
    }

    return 1;
}

PLAYER Sequence_Winner (GAMESTATE game)
{
    int A[9], i;
    for (i = 0; i < game.numberPlayers; i++)
    {
        if(game.players[i].action != FOLD)
        {
            A[i] = CheckPlayer(game, i); //Assign a Value to A[i] with i = PlayerNumber
        }
    }
    int Winner = LargestinArray (A, i); //Winner is the player with the highest number from CheckPlayer()

    return game.players[Winner];
}

int CheckPlayer (GAMESTATE game, int PlayerNumber)
{ //The better the card sequence, the higher is the priority number returned.
    int priority;

    if (IsStraightFlush(game,PlayerNumber) == 1)
    {
        priority = 22;
    }

    else if (IsFourofaKind (game,PlayerNumber) == 1)
    {
        priority = 21;
    }

    else if (IsFullHouse(game,PlayerNumber) == 1)
    {
        priority = 20;
    }
    
    else if (IsFlush (game,PlayerNumber) == 1)
    {
        priority = 19;
    }

    else if (IsStraight (game,PlayerNumber) == 1)
    {
        priority = 18;
    }

    else if (IsThreeofaKind (game,PlayerNumber) == 1)
    {
        priority = 17;
    }

    else if (IsTwoPair (game,PlayerNumber) ==1)
    {
        priority = 16;
    }

    else if (IsOnePair (game,PlayerNumber) ==1)
    {
        priority = 15;
    }

    else //Implement High Card here
    {
        priority = IsHighCard(game, PlayerNumber);
    }

    return priority;
}

int IsRoyalFlush (GAMESTATE game, int PlayerNumber) //Change
{
    if (IsFlush(game, PlayerNumber) == 1 && IsStraight(game, PlayerNumber) == 1)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int IsStraightFlush(GAMESTATE game, int PlayerNumber)
{
    if (IsFlush(game, PlayerNumber) == 1 && IsStraight(game, PlayerNumber) == 1)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
    
int IsFourofaKind (GAMESTATE game, int PlayerNumber)
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

    if ((a == b && b == c && c == d) || (b == c && c == d && d == e) || (c == d && d == e && e == f) || (d == e && e == f && f == g))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int IsFullHouse(GAMESTATE game, int PlayerNumber) // Change ?
{
    if (IsThreeofaKind(game, PlayerNumber) == 1 && IsTwoPair(game, PlayerNumber) == 1)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

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

    NewDeck = SortbySuit(NewDeck);

    a = NewDeck.cards[0].suit;
    b = NewDeck.cards[1].suit;
    c = NewDeck.cards[2].suit;
    d = NewDeck.cards[3].suit;
    e = NewDeck.cards[4].suit;
    f = NewDeck.cards[5].suit;
    g = NewDeck.cards[6].suit;

    if ((a == b && b == c && c == d && d == e) || (b == c && c == d && d == e && e == f) || (c == d && d == e && e == f && f == g))
    {
        return 1;
    }
    else
    {
        return 0;
    }

} 

int IsStraight (GAMESTATE game, int PlayerNumber)
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
}

int IsThreeofaKind (GAMESTATE game, int PlayerNumber)
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

    if ((a == b && b == c) || (b == c && c == d) || (c == d && d == e) || (d == e && e == f) || (e == f && f == g))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
int IsTwoPair (GAMESTATE game, int PlayerNumber)
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

    if ((a == b && (c == d || d == e || e == f || f == g))  || (b == c && (d == e || e == f || f == g)) || (c == d && (e == f || f == g)) || (d == e && (f == g)))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int IsOnePair (GAMESTATE game, int PlayerNumber)
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

    if ((a == b) || (b == c) || (c == d) || (d == e) || (e == f) || (f == g))
    {
        return 1;
    }
    else
    {
        return 0;
    }
}

int IsHighCard(GAMESTATE game, int PlayerNumber){
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
    
    if(NewDeck.cards[6].rank == 14){
        return 14;
    }
    if(NewDeck.cards[6].rank == 13){
        return 13;
    }
    if(NewDeck.cards[6].rank == 12){
        return 12;
    }
    if(NewDeck.cards[6].rank == 11){
        return 11;
    }
    if(NewDeck.cards[6].rank == 10){
        return 10;
    }
    if(NewDeck.cards[6].rank == 9){
        return 9;
    }
    if(NewDeck.cards[6].rank == 8){
        return 8;
    }
    if(NewDeck.cards[6].rank == 7){
        return 7;
    }
    if(NewDeck.cards[6].rank == 6){
        return 6;
    }
    if(NewDeck.cards[6].rank == 5){
        return 5;
    }
    if(NewDeck.cards[6].rank == 4){
        return 4;
    }
    if(NewDeck.cards[6].rank == 3){
        return 3;
    }
    if(NewDeck.cards[6].rank == 2){
        return 2;
    }


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
    return D;
}

DECK SortbySuit (DECK D)
{
    CARD X;
    int i, j;
    for(i=0;i<7;i++)
    {
        for (j=0 ; j<7-i-1; j++)
        {
            if(D.cards[j].suit > D.cards[j+1].suit)
            {
                X = D.cards[j];
                D.cards[j] = D.cards[j+1];
                D.cards[j+1] = X;
            }
        }
    }
    return D;
}

int LargestinArray (int A[], int B)
{
    int i;
    int max = 1;
    int a;
 
    for (i = 0; i < B; i++)
        if (A[i] > max)
        {
            max = A[i];
            a = i;
        }
            
    return a;
}

void random(int A[], int B) {
    srand(time(NULL));
 
    for(int i = B-1; i > 0; i--) {
        int j = rand() % (i+1); //Assign a random value for j within bounds
        swap(&A[i], &A[j]); //Swap two elements in the array randomly
    }
}

void swap(int *a, int *b) {
    int X = *a;
    *a = *b;
    *b = X;
}
    
