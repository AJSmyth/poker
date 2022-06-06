#include "game.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <limits.h>


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

int Sequence_Winner (GAMESTATE game)
{
    int A[9], i, BREAK;
    for (i = 0; i < game.numberPlayers; i++)
    {
        if(game.players[i].action != FOLD)
        {
            A[i] = CheckPlayer(game, i); //Assign a Value to A[i] with i = PlayerNumber
        }
    }

    int Winner = LargestinArray (A, i); //Winner is the player with the highest number from CheckPlayer()
    for (int i = 0; i < game.numberPlayers ; i++)
    {
        for (int j = i + 1; j < game.numberPlayers ; j++)
        {
            if (A[i] == A[j] && i == LargestinArray (A, i))
            {
                //Winner = TieBreaker (i, j, A[i],  game);
                if (Winner == -1)
                {
                    game.SplitPotPlayers[0] = i;
                    game.SplitPotPlayers[1] = j;
                    game.SplitPot = 1;
                    return 0;
                }
                BREAK = 1;
                break;
            }
        }
        if (BREAK == 1)
        {
            break;
        }
    }
    
    return Winner;
}

int test_for_tiebreaker(void) 
{
    //gcc -std=c11 src/game.c src/deck.c -o bin/test -g
    GAMESTATE game;
    CARD card1;
    card1.rank = ACE;
    card1.suit = HEARTS;
    CARD card2;
    card2.rank = ACE;
    card2.suit = SPADES;
    CARD card3;
    card3.rank = ACE;
    card3.suit = CLUBS;
    CARD card4;
    card4.rank = ACE;
    card4.suit = DIAMONDS;

    game.numberPlayers = 2;
    game.players[0].Balance = 500;
    game.players[1].Balance = 600;
    game.pot = 1000;
    game.players[0].card1 = card1;
    game.players[0].card2 = card2;
    game.players[1].card1 = card3;
    game.players[1].card2 = card4;
    game = TieBreaker(game);
    printf("Player 1: actual %5d, expecting %5d\n", game.players[0].Balance, 1000);
    printf("Player 2: actual %5d, expecting %5d\n", game.players[1].Balance, 1100);
    printf("Pot:      actual %5d, expecting %5d\n", game.pot, 0);
    
    return 0;
}
GAMESTATE TieBreaker(GAMESTATE game) {
    int priorities[game.numberPlayers];
    for (int i=0; i<game.numberPlayers; i++) {
        priorities[i] = getMaxPriorityOfThePlayer(game, i);
    }

    //pick the player with the highest priority
    int maxPriorityValue;
    int countOfPlayersWhoTied = 0;
    for (int i=0; i<game.numberPlayers; i++) {
        maxPriorityValue = maxPriorityValue > priorities[i] ? maxPriorityValue: priorities[i];
    }
    for (int i=0; i<game.numberPlayers; i++) {
        if(priorities[i] == maxPriorityValue);
        countOfPlayersWhoTied++;
    }
    int indexOfplayersTiedForMostPriority[countOfPlayersWhoTied];
    int k = 0;
    for (int i=0; i<game.numberPlayers; i++) {
        if(priorities[i] == maxPriorityValue);
        indexOfplayersTiedForMostPriority[k] = i;
        k++;
    }

    int potToDistribute = game.pot / countOfPlayersWhoTied;
    game.pot=0;
    for (int i=0; i<countOfPlayersWhoTied; i++) {
        game.players[indexOfplayersTiedForMostPriority[i]].Balance += potToDistribute;
    }
    return game;
}

int getMaxPriorityOfThePlayer(GAMESTATE game, int person) {
    int max = INT_MIN;
    max = max > IsRoyalFlush(game, person) ? max : IsRoyalFlush(game, person);
    max = max > IsFourofaKind(game, person) ? max : IsFourofaKind(game, person);
    max = max > IsFullHouse(game, person) ? max : IsFullHouse(game, person);
    max = max > IsFlush(game, person) ? max : IsFlush(game, person);
    max = max > IsStraight(game, person) ? max : IsStraight(game, person);
    max = max > IsThreeofaKind(game, person) ? max : IsThreeofaKind(game, person);
    max = max > IsTwoPair(game, person) ? max : IsTwoPair(game, person);
    max = max > IsOnePair(game, person) ? max : IsOnePair(game, person);
    max = max > IsHighCard(game, person) ? max : IsHighCard(game, person);
    return max;
}

int CheckPlayer (GAMESTATE game, int PlayerNumber)
{ //The better the card sequence, the higher is the priority number returned.
    int priority;

    if (IsRoyalFlush(game, PlayerNumber) == 1)
    {
        priority = 23;
    }

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
    DECK NewDeck;
    int i;
    for (i = 0; i< 5; i++)
    {
        NewDeck.cards[i] = game.communityCards.cards[i];
    }
    NewDeck.cards[5] = game.players[PlayerNumber].card1;
    NewDeck.cards[6] = game.players[PlayerNumber].card2;

    NewDeck = SortbyRank(NewDeck);

    if (IsFlush(game, PlayerNumber) == 1 && IsStraight(game, PlayerNumber) == 1 && NewDeck.cards[6].rank == 14 && NewDeck.cards[5].rank == 13 && NewDeck.cards[4].rank == 12 && NewDeck.cards[3].rank == 11 && NewDeck.cards[2].rank == 10)
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

int nUnfolded(GAMESTATE game) {
  int count = 0;
  for (int i = 0; i < game.numberPlayers; ++i) {
    if (game.players[i].action != FOLD) count++;
  }
  return count;
}
   
