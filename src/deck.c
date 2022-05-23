#include "deck.h"
#include "game.h"
#include <stdlib.h>
#include <stdio.h>

DECK INIT(){
    DECK deck;
    int count = 0;
    for(SUIT s = 0; s <= 3; s++){
        for(int r = 1; r <= 13; r++){
            deck.cards[count].suit = s;
            deck.cards[count].rank = r;
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

    for (int X = 0 ; X < 53 ; X++) //Assign values from 0-52 in an Array
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

GAMESTATE AssignCards (GAMESTATE game)
{
    int currentcard = 0;
    for (int i = 0; i <= game.numberPlayers; i++)
    {
        if(game.players[i].action != FOLD){
            game.players[i].card1 = game.shuffleDeck.cards[x]; //Assign 1st Card to players
            game.players[i].card2 = game.shuffleDeck.cards[x+1]; //Assign 2nd Card to players
            x = x+2;
            game.shuffleDeck.BOTTOM = x;
        }
    }
    
    for (int a = 0; a<= 4;a++)
    {
        game.communityCards.cards[a] = game.shuffleDeck.cards[ game.shuffleDeck.BOTTOM + a];
    }

    for (int a = 0; a <= 4 ; a++) //Making P equal to players as a temporary variable
    {
        P[a] = game.players[a];
    }
    
    if (game.GameCount == 0)
    {
        game.players[0].role = 0; //Assigning small blind to game.players[0]
        game.players[0].Bid = 5; //Assigning default bid on small blind
        game.players[1].role = 1; //Assigning bigame blind to game.players[1]
        game.players[1].Bid = 10; //Assigning default bid on big blind
        for (int a = 2; a <= 4 ; a++)
        {
            game.players[a].role = 2; //Assigning other game.players ar normal players
            game.players[a].Bid = 0; //Assigning 0 bid for non-Blind players
        }
    }

    else 
    {
        for (int a = 0; a <= 4 ; a++) //Moving the players roles[Blinds] and assigning 
        {                             //default bids on binds, at the start of the round   
            if (a > 0)
            {
                game.players[a].role = P[a-1].role;
            }

            else 
            {
                game.players[a].role = P[4].role;
            }
        }
    }
    return game;
}

