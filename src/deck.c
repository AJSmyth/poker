#include "deck.h"
#include "game.h"
#include <stdlib.h>
#include <stdio.h>

DECK INIT(){
    DECK deck;
    int count = 0;
    for(SUIT s = 0; s <= 3; s++){
        for(int r = 2; r <= 14; r++){
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

    randomSwap(a,51); //Randomise the order of numbers in the array 

    for (int i = 0 ; i <= 51 ; i++) //Repeat for 52 cards
    {
        shuffle.cards[i] = deck.cards[a[i]]; //Shuffle the cards
    }
    return shuffle;
}

GAMESTATE AssignCards (GAMESTATE game)
{
    //assign bottom 10 cards to players and increment the deck pointer
    int currentcard = 0;
    for (int i = 0; i <= game.numberPlayers; i++)
    {
        if(game.players[i].action != FOLD){
            game.players[i].card1 = game.shuffleDeck.cards[currentcard]; //Assign 1st Card to players
            game.players[i].card2 = game.shuffleDeck.cards[currentcard+1]; //Assign 2nd Card to players
            currentcard = currentcard+2;
        }
    }
    game.shuffleDeck.BOTTOM = currentcard;
    game.communityCards.BOTTOM = 0;

    for (int a = 0; a<= 4;a++)
    {
        game.communityCards.cards[a] = game.shuffleDeck.cards[game.shuffleDeck.BOTTOM + a];
    }
    
    if (game.GameCount == 0)
    {
        game.players[0].role = SMALLBLIND; //Assigning small blind to game.players[0]
        game.players[0].Bid = 5; //Assigning default bid on small blind
        game.players[0].Balance = game.players[0].Balance - 5;//they have lost their starting bid
        game.players[1].role = BIGBLIND; //Assigning bigame blind to game.players[1]
        game.players[1].Bid = 10; //Assigning default bid on big blind
        game.players[1].Balance = game.players[1].Balance - 10;//they have lost their starting bid
        game.pot=15;
        for (int a = 2; a < game.numberPlayers; a++)
        {
            game.players[a].role = NORMAL; //Assigning other game.players ar normal players
            game.players[a].Bid = 0; //Assigning 0 bid for non-Blind players
        }
    }

    else 
    {
        for (int a = 0; a <= game.numberPlayers ; a++) //Moving the players roles[Blinds] and assigning 
        {                             //default bids on binds, at the start of the round   
            if(game.players[a].role == SMALLBLIND){
                game.players[a].role = NORMAL;
                if(a+1<game.numberPlayers){
                    game.players[a+1].role = SMALLBLIND; 
                    game.players[a+1].Balance = game.players[a+1].Balance - 5; 
                    game.players[a+1].Bid = 5;
                    if(a+2<game.numberPlayers){
                        game.players[a+2].role = BIGBLIND; 
                        game.players[a+2].Balance = game.players[a+2].Balance - 10; 
			game.players[a+1].Bid = 10;
                    }else{
                        game.players[1].role = BIGBLIND;
                        game.players[1].Balance = game.players[1].Balance - 10;
			game.players[1].Bid = 10;
                    }
                }else{
                    game.players[0].role = SMALLBLIND;
                    game.players[0].Balance = game.players[0].Balance - 5; 
                    game.players[1].role = BIGBLIND;
                    game.players[1].Balance = game.players[1].Balance - 10; 
                }
                break;
            }
        }
        game.pot=15;
    }
    game.currCall = 10;
    game.playerTurn = 2;
    return game;
}

