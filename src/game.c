#include "game.h"
#include "deck.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>


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
                Winner = TieBreaker (i, j, A[i],  game);
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

int TieBreaker (int a, int b, int Priority, GAMESTATE game)
{ 
    //PLAYER P = game.players[a];
    int x;
    if(Priority <= 14)
    {
        // both players have high card
        DECK NewDeck_P1;
        DECK NewDeck_P2;

        for (x = 0; x< 5; x++)
        {
            NewDeck_P1.cards[x] = game.communityCards.cards[x];
            NewDeck_P2.cards[x] = game.communityCards.cards[x]; 
        }

        NewDeck_P1.cards[5] = game.players[a].card1;
        NewDeck_P1.cards[6] = game.players[a].card2;

        NewDeck_P2.cards[5] = game.players[b].card1;
        NewDeck_P2.cards[6] = game.players[b].card2;
        
        NewDeck_P1 = SortbyRank(NewDeck_P1);
        NewDeck_P2 = SortbyRank(NewDeck_P2);

        for (int i = 6; i > 1; i--)
        {
            // Go through the deck from low to high and compare the top 5 cards
            // Need to make sure that deck is in ascending order
            if(NewDeck_P1.cards[i].rank > NewDeck_P2.cards[i].rank)
            {
                return a;
            }
            else if (NewDeck_P1.cards[i].rank < NewDeck_P2.cards[i].rank)
            {
                return b;
            }
            else
            {
                continue;
            }
        }

        //players have the same high 5 cards
        return -1;
    }

    if (Priority == 15)
    {
        DECK NewDeck_P1;
        DECK NewDeck_P2;
        CARD Deck1_HighCard;
        CARD Deck2_HighCard;

        int x;
        // both players have high card

        for (x = 0; x< 5; x++)
        {
            NewDeck_P1.cards[x] = game.communityCards.cards[x];
            NewDeck_P2.cards[x] = game.communityCards.cards[x]; 
        }

        NewDeck_P1.cards[5] = game.players[a].card1;
        NewDeck_P1.cards[6] = game.players[a].card2;

        NewDeck_P2.cards[5] = game.players[b].card1;
        NewDeck_P2.cards[6] = game.players[b].card2;
        
        NewDeck_P1 = SortbyRank(NewDeck_P1);
        NewDeck_P2 = SortbyRank(NewDeck_P2);

        for (int a = 6 ; a >= 1 ; a--)
        {
            if (NewDeck_P1.cards[a].rank == NewDeck_P1.cards[a-1].rank)
            {
                Deck1_HighCard = NewDeck_P1.cards[a];
                break;
            }
        }

        for (int a = 6 ; a >= 1 ; a--)
        {
            if (NewDeck_P2.cards[a].rank == NewDeck_P2.cards[a-1].rank)
            {
                Deck2_HighCard = NewDeck_P2.cards[a];
                break;
            }
        }
        if (Deck1_HighCard.rank > Deck2_HighCard.rank)
        {
            return a;
        }

        else if (Deck1_HighCard.rank < Deck2_HighCard.rank)
        {
            return b;
        }

        else if (Deck1_HighCard.rank == Deck2_HighCard.rank)
        {
            for (int X = 6; X>1; X--)
            {
                // Go through the deck from low to high and compare the top 5 cards
                // Need to make sure that deck is in ascending order
            
                if(NewDeck_P1.cards[X].rank > NewDeck_P2.cards[X].rank)
                {
                    return a;
                }

                else if (NewDeck_P1.cards[X].rank < NewDeck_P2.cards[X].rank)
                {
                    return b;
                }
            }
        }
        return -1;
    }

    if (Priority == 16)
    {
        DECK NewDeck_P1;
        DECK NewDeck_P2;
        CARD Deck1_PairCard1;
        CARD Deck1_PairCard2;
        CARD Deck2_PairCard1;
        CARD Deck2_PairCard2;

        int x, BREAK;

        for (x = 0; x< 5; x++)
        {
            NewDeck_P1.cards[x] = game.communityCards.cards[x];
            NewDeck_P2.cards[x] = game.communityCards.cards[x]; 
        }

        NewDeck_P1.cards[5] = game.players[a].card1;
        NewDeck_P1.cards[6] = game.players[a].card2;

        NewDeck_P2.cards[5] = game.players[b].card1;
        NewDeck_P2.cards[6] = game.players[b].card2;
        
        NewDeck_P1 = SortbyRank(NewDeck_P1);
        NewDeck_P2 = SortbyRank(NewDeck_P2);

        for (int X = 6 ;X >= 0 ; X--)
        { //Find which 2 nunbers make pairs in the two pair in Deck 1
            if (NewDeck_P1.cards[X].rank == NewDeck_P1.cards[X-1].rank)
            {
                Deck1_PairCard1 = NewDeck_P1.cards[X];
                X = X-1;

                for (X ; X >= 0 ; X--)
                {
                    if (NewDeck_P1.cards[X].rank == NewDeck_P1.cards[X-1].rank)
                    {
                        Deck1_PairCard2 = NewDeck_P1.cards[X];
                        BREAK = 1;
                        break;
                    }
                }
            }
            if (BREAK == 1)
            {
                BREAK = 0;
                break;
            }
        }

        for (int X = 6 ; X >= 0 ; X--)
        { //Find which 2 nunbers make pairs in the two pair in Deck 2
            if (NewDeck_P2.cards[X].rank == NewDeck_P2.cards[X-1].rank)
            {
                Deck2_PairCard1 = NewDeck_P2.cards[X];
                X = X-1;

                for (X; X >= 0 ; X--)
                {
                    if (NewDeck_P2.cards[X].rank == NewDeck_P2.cards[X-1].rank)
                    {
                        Deck2_PairCard2 = NewDeck_P2.cards[X];
                        BREAK = 1;
                        break;
                    }
                }
            }
            if (BREAK == 1)
            {
                BREAK = 0;
                break;
            }
        }

        if (Deck1_PairCard1.rank > Deck2_PairCard1.rank )
        {
            return a;
        }

        else if  (Deck1_PairCard1.rank < Deck2_PairCard1.rank )
        {
            return b;
        }

        else if (Deck1_PairCard1.rank  == Deck2_PairCard1.rank )
        {
            if (Deck1_PairCard2.rank  > Deck2_PairCard2.rank )
            {
                return a;
            }

            else if (Deck1_PairCard2.rank  < Deck2_PairCard2.rank )
            {
                return b;
            }

            else if (Deck1_PairCard2.rank  == Deck2_PairCard2.rank )
            {
                for (int Y = 6; Y>1; Y--)
                {
                    // Go through the deck from low to high and compare the top 5 cards
                    // Need to make sure that deck is in ascending order
            
                    if(NewDeck_P1.cards[Y].rank > NewDeck_P2.cards[Y].rank)
                    {
                        return a;
                    }

                    else if (NewDeck_P1.cards[Y].rank < NewDeck_P2.cards[Y].rank)
                    {
                        return b;
                    }
                }
            }
        }
        return -1;
    }

    if (Priority == 17)
    {
        DECK NewDeck_P1;
        DECK NewDeck_P2;
        CARD Deck1_HighCard;
        CARD Deck2_HighCard;

        int x;
        // both players have high card

        for (x = 0; x< 5; x++)
        {
            NewDeck_P1.cards[x] = game.communityCards.cards[x];
            NewDeck_P2.cards[x] = game.communityCards.cards[x]; 
        }

        NewDeck_P1.cards[5] = game.players[a].card1;
        NewDeck_P1.cards[6] = game.players[a].card2;

        NewDeck_P2.cards[5] = game.players[b].card1;
        NewDeck_P2.cards[6] = game.players[b].card2;
        
        NewDeck_P1 = SortbyRank(NewDeck_P1);
        NewDeck_P2 = SortbyRank(NewDeck_P2);

        for (int a = 6 ; a >= 1 ; a--)
        {
            if (NewDeck_P1.cards[a].rank == NewDeck_P1.cards[a-1].rank)
            {
                Deck1_HighCard = NewDeck_P1.cards[a];
                break;
            }
        }

        for (int a = 6 ; a >= 1 ; a--)
        {
            if (NewDeck_P2.cards[a].rank == NewDeck_P2.cards[a-1].rank)
            {
                Deck2_HighCard = NewDeck_P2.cards[a];
                break;
            }
        }
        if (Deck1_HighCard.rank > Deck2_HighCard.rank)
        {
            return a;
        }

        else if (Deck1_HighCard.rank < Deck2_HighCard.rank)
        {
            return b;
        }

        else if (Deck1_HighCard.rank == Deck2_HighCard.rank)
        {
            for (int X = 6; X>1; X--)
            {
                // Go through the deck from low to high and compare the top 5 cards
                // Need to make sure that deck is in ascending order
            
                if(NewDeck_P1.cards[X].rank > NewDeck_P2.cards[X].rank)
                {
                    return a;
                }

                else if (NewDeck_P1.cards[X].rank < NewDeck_P2.cards[X].rank)
                {
                    return b;
                }
            }
        }
        return -1;
    }

    if (Priority == 18 || Priority == 22)
    {
        DECK NewDeck_P1;
        DECK NewDeck_P2;
        CARD Deck1_HighCard;
        CARD Deck2_HighCard;

        int x;
        // both players have high card

        for (x = 0; x< 5; x++)
        {
            NewDeck_P1.cards[x] = game.communityCards.cards[x];
            NewDeck_P2.cards[x] = game.communityCards.cards[x]; 
        }

        NewDeck_P1.cards[5] = game.players[a].card1;
        NewDeck_P1.cards[6] = game.players[a].card2;

        NewDeck_P2.cards[5] = game.players[b].card1;
        NewDeck_P2.cards[6] = game.players[b].card2;
        
        NewDeck_P1 = SortbyRank(NewDeck_P1);
        NewDeck_P2 = SortbyRank(NewDeck_P2);

        if (NewDeck_P1.cards[6].rank == NewDeck_P1.cards[5].rank + 1 && NewDeck_P1.cards[5].rank == NewDeck_P1.cards[4].rank + 1 && NewDeck_P1.cards[4].rank == NewDeck_P1.cards[3].rank + 1 && NewDeck_P1.cards[3].rank == NewDeck_P1.cards[2].rank + 1)
        {
            Deck1_HighCard = NewDeck_P1.cards[6];
        }

        else if (NewDeck_P1.cards[2].rank == NewDeck_P1.cards[1].rank + 1 && NewDeck_P1.cards[5].rank == NewDeck_P1.cards[4].rank + 1 && NewDeck_P1.cards[4].rank == NewDeck_P1.cards[3].rank + 1 && NewDeck_P1.cards[3].rank == NewDeck_P1.cards[2].rank + 1)
        {
            Deck1_HighCard = NewDeck_P1.cards[5];
        }

        else if (NewDeck_P1.cards[1].rank == NewDeck_P1.cards[0].rank + 1 && NewDeck_P1.cards[5].rank == NewDeck_P1.cards[4].rank + 1 && NewDeck_P1.cards[4].rank == NewDeck_P1.cards[3].rank + 1 && NewDeck_P1.cards[3].rank == NewDeck_P1.cards[2].rank + 1)
        {
            Deck1_HighCard = NewDeck_P1.cards[4];
        }

        if (NewDeck_P2.cards[6].rank == NewDeck_P2.cards[5].rank + 1 && NewDeck_P2.cards[5].rank == NewDeck_P2.cards[4].rank + 1 && NewDeck_P2.cards[4].rank == NewDeck_P2.cards[3].rank + 1 && NewDeck_P2.cards[3].rank == NewDeck_P2.cards[2].rank + 1)
        {
            Deck2_HighCard = NewDeck_P2.cards[6];
        }

        else if (NewDeck_P2.cards[2].rank == NewDeck_P2.cards[1].rank + 1 && NewDeck_P2.cards[5].rank == NewDeck_P2.cards[4].rank + 1 && NewDeck_P2.cards[4].rank == NewDeck_P2.cards[3].rank + 1 && NewDeck_P2.cards[3].rank == NewDeck_P2.cards[2].rank + 1)
        {
            Deck2_HighCard = NewDeck_P2.cards[5];
        }

        else if (NewDeck_P2.cards[1].rank == NewDeck_P2.cards[0].rank + 1 && NewDeck_P2.cards[5].rank == NewDeck_P2.cards[4].rank + 1 && NewDeck_P2.cards[4].rank == NewDeck_P2.cards[3].rank + 1 && NewDeck_P2.cards[3].rank == NewDeck_P2.cards[2].rank + 1)
        {
            Deck2_HighCard = NewDeck_P2.cards[4];
        }

        if (Deck1_HighCard.rank > Deck2_HighCard.rank )
        {
            return a;
        }

        else if (Deck1_HighCard.rank < Deck2_HighCard.rank)
        {
            return b;
        }

        return -1;
    }

    if (Priority == 19)
    {
        DECK NewDeck_P1;
        DECK NewDeck_P2;
        DECK NewDeck_P1_Rank;
        DECK NewDeck_P2_Rank;

        CARD Deck1_HighCard;
        CARD Deck2_HighCard;

        int x;
        // both players have high card

        for (x = 0; x< 5; x++)
        {
            NewDeck_P1.cards[x] = game.communityCards.cards[x];
            NewDeck_P2.cards[x] = game.communityCards.cards[x]; 
        }

        NewDeck_P1.cards[5] = game.players[a].card1;
        NewDeck_P1.cards[6] = game.players[a].card2;

        NewDeck_P2.cards[5] = game.players[b].card1;
        NewDeck_P2.cards[6] = game.players[b].card2;
        
        NewDeck_P1 = SortbySuit(NewDeck_P1);
        NewDeck_P2 = SortbySuit(NewDeck_P2);

        //Take the cards of the saame suit into a new Deck, and sort them by rank for each player
        if (NewDeck_P1.cards[6].suit == NewDeck_P1.cards[5].suit && NewDeck_P1.cards[5].suit == NewDeck_P1.cards[4].suit && NewDeck_P1.cards[4].suit == NewDeck_P1.cards[3].suit && NewDeck_P1.cards[3].suit == NewDeck_P1.cards[2].suit)
        {
            NewDeck_P1_Rank.cards[0] = NewDeck_P1.cards[2];
            NewDeck_P1_Rank.cards[1] = NewDeck_P1.cards[3];
            NewDeck_P1_Rank.cards[2] = NewDeck_P1.cards[4];
            NewDeck_P1_Rank.cards[3] = NewDeck_P1.cards[5];
            NewDeck_P1_Rank.cards[4] = NewDeck_P1.cards[6];
            NewDeck_P1_Rank = SortbyRank (NewDeck_P1_Rank);
        }

        else if (NewDeck_P1.cards[2].suit == NewDeck_P1.cards[1].suit && NewDeck_P1.cards[5].suit == NewDeck_P1.cards[4].suit && NewDeck_P1.cards[4].suit == NewDeck_P1.cards[3].suit && NewDeck_P1.cards[3].suit == NewDeck_P1.cards[2].suit)
        {
            NewDeck_P1_Rank.cards[0] = NewDeck_P1.cards[2];
            NewDeck_P1_Rank.cards[1] = NewDeck_P1.cards[3];
            NewDeck_P1_Rank.cards[2] = NewDeck_P1.cards[4];
            NewDeck_P1_Rank.cards[3] = NewDeck_P1.cards[5];
            NewDeck_P1_Rank.cards[4] = NewDeck_P1.cards[1];
            NewDeck_P1_Rank = SortbyRank (NewDeck_P1_Rank);
        }

        else if (NewDeck_P1.cards[2].suit == NewDeck_P1.cards[1].suit && NewDeck_P1.cards[1].suit == NewDeck_P1.cards[0].suit && NewDeck_P1.cards[4].suit == NewDeck_P1.cards[3].suit && NewDeck_P1.cards[3].suit == NewDeck_P1.cards[2].suit)
        {
            NewDeck_P1_Rank.cards[0] = NewDeck_P1.cards[2];
            NewDeck_P1_Rank.cards[1] = NewDeck_P1.cards[3];
            NewDeck_P1_Rank.cards[2] = NewDeck_P1.cards[4];
            NewDeck_P1_Rank.cards[3] = NewDeck_P1.cards[0];
            NewDeck_P1_Rank.cards[4] = NewDeck_P1.cards[1];
            NewDeck_P1_Rank = SortbyRank (NewDeck_P1_Rank);
        }
        
        if (NewDeck_P2.cards[6].suit == NewDeck_P2.cards[5].suit && NewDeck_P2.cards[5].suit == NewDeck_P2.cards[4].suit && NewDeck_P2.cards[4].suit == NewDeck_P2.cards[3].suit && NewDeck_P2.cards[3].suit == NewDeck_P2.cards[2].suit)
        {
            NewDeck_P2_Rank.cards[0] = NewDeck_P2.cards[2];
            NewDeck_P2_Rank.cards[1] = NewDeck_P2.cards[3];
            NewDeck_P2_Rank.cards[2] = NewDeck_P2.cards[4];
            NewDeck_P2_Rank.cards[3] = NewDeck_P2.cards[5];
            NewDeck_P2_Rank.cards[4] = NewDeck_P2.cards[6];
            NewDeck_P2_Rank = SortbyRank (NewDeck_P2_Rank);
        }

        else if (NewDeck_P2.cards[2].suit == NewDeck_P2.cards[1].suit && NewDeck_P2.cards[5].suit == NewDeck_P2.cards[4].suit && NewDeck_P2.cards[4].suit == NewDeck_P2.cards[3].suit && NewDeck_P2.cards[3].suit == NewDeck_P2.cards[2].suit)
        {
            NewDeck_P2_Rank.cards[0] = NewDeck_P2.cards[2];
            NewDeck_P2_Rank.cards[1] = NewDeck_P2.cards[3];
            NewDeck_P2_Rank.cards[2] = NewDeck_P2.cards[4];
            NewDeck_P2_Rank.cards[3] = NewDeck_P2.cards[5];
            NewDeck_P2_Rank.cards[4] = NewDeck_P2.cards[1];
            NewDeck_P2_Rank = SortbyRank (NewDeck_P2_Rank);
        }

        else if (NewDeck_P2.cards[2].suit == NewDeck_P2.cards[1].suit && NewDeck_P2.cards[1].suit == NewDeck_P2.cards[0].suit && NewDeck_P2.cards[4].suit == NewDeck_P2.cards[3].suit && NewDeck_P2.cards[3].suit == NewDeck_P2.cards[2].suit)
        {
            NewDeck_P2_Rank.cards[0] = NewDeck_P2.cards[2];
            NewDeck_P2_Rank.cards[1] = NewDeck_P2.cards[3];
            NewDeck_P2_Rank.cards[2] = NewDeck_P2.cards[4];
            NewDeck_P2_Rank.cards[3] = NewDeck_P2.cards[0];
            NewDeck_P2_Rank.cards[4] = NewDeck_P2.cards[1];
            NewDeck_P2_Rank = SortbyRank (NewDeck_P2_Rank);
        }

        //Now, NewDeck_P2_Rank and NewDeck_P1_Rank are the same suit decks of different players sorted by rank
        //We need to find the winning hand now by comparing the high cards of eadch deck

        for (int i = 4; i >= 0; i--)
        {
            // Go through the deck from low to high and compare the top 5 cards
            // Need to make sure that deck is in ascending order
            if(NewDeck_P1_Rank.cards[i].rank > NewDeck_P2_Rank.cards[i].rank)
            {
                return a;
            }
            else if (NewDeck_P1_Rank.cards[i].rank < NewDeck_P2_Rank.cards[i].rank)
            {
                return b;
            }
            else
            {
                continue;
            }
        }

        //players have the same high 5 cards
        return -1;
    }

    if (Priority == 20)
    {
        DECK NewDeck_P1;
        DECK NewDeck_P2;
        CARD Deck1_PairCard1;
        CARD Deck1_PairCard2;
        CARD Deck2_PairCard1;
        CARD Deck2_PairCard2;

        int x, BREAK;

        for (x = 0; x< 5; x++)
        {
            NewDeck_P1.cards[x] = game.communityCards.cards[x];
            NewDeck_P2.cards[x] = game.communityCards.cards[x]; 
        }

        NewDeck_P1.cards[5] = game.players[a].card1;
        NewDeck_P1.cards[6] = game.players[a].card2;

        NewDeck_P2.cards[5] = game.players[b].card1;
        NewDeck_P2.cards[6] = game.players[b].card2;
        
        NewDeck_P1 = SortbyRank(NewDeck_P1);
        NewDeck_P2 = SortbyRank(NewDeck_P2);

        for (int X = 6 ;X >= 0 ; X--)
        { //Find which 2 nunbers make pairs in the two pair in Deck 1
            if (NewDeck_P1.cards[X].rank == NewDeck_P1.cards[X-1].rank)
            {
                Deck1_PairCard1 = NewDeck_P1.cards[X];
                
                if (NewDeck_P2.cards[X-1].rank == NewDeck_P2.cards[X-2].rank)
                {
                    X = X-1;
                }
                X = X-1;

                for (X ; X >= 0 ; X--)
                {
                    if (NewDeck_P1.cards[X].rank == NewDeck_P1.cards[X-1].rank)
                    {
                        Deck1_PairCard2 = NewDeck_P1.cards[X];
                        BREAK = 1;
                        break;
                    }
                }
            }
            if (BREAK == 1)
            {
                BREAK = 0;
                break;
            }
        }

        for (int X = 6 ; X >= 0 ; X--)
        { //Find which 2 nunbers make pairs in the two pair in Deck 2
            if (NewDeck_P2.cards[X].rank == NewDeck_P2.cards[X-1].rank)
            {
                Deck2_PairCard1 = NewDeck_P2.cards[X];
                
                if (NewDeck_P2.cards[X-1].rank == NewDeck_P2.cards[X-2].rank)
                {
                    X = X-1;
                }
                X = X-1;

                for (X; X >= 0 ; X--)
                {
                    if (NewDeck_P2.cards[X].rank == NewDeck_P2.cards[X-1].rank)
                    {
                        Deck2_PairCard2 = NewDeck_P2.cards[X];
                        BREAK = 1;
                        break;
                    }
                }
            }
            if (BREAK == 1)
            {
                BREAK = 0;
                break;
            }
        }

        if (Deck1_PairCard1.rank > Deck2_PairCard1.rank )
        {
            return a;
        }

        else if  (Deck1_PairCard1.rank < Deck2_PairCard1.rank )
        {
            return b;
        }

        else if (Deck1_PairCard1.rank  == Deck2_PairCard1.rank )
        {
            if (Deck1_PairCard2.rank  > Deck2_PairCard2.rank )
            {
                return a;
            }

            else if (Deck1_PairCard2.rank  < Deck2_PairCard2.rank )
            {
                return b;
            }

            else if (Deck1_PairCard2.rank  == Deck2_PairCard2.rank )
            {
                for (int Y = 6; Y>1; Y--)
                {
                    // Go through the deck from low to high and compare the top 5 cards
                    // Need to make sure that deck is in ascending order
            
                    if(NewDeck_P1.cards[Y].rank > NewDeck_P2.cards[Y].rank)
                    {
                        return a;
                    }

                    else if (NewDeck_P1.cards[Y].rank < NewDeck_P2.cards[Y].rank)
                    {
                        return b;
                    }
                }
            }
        }
        return -1;
    }

    if (Priority == 21)
    {
        DECK NewDeck_P1;
        DECK NewDeck_P2;
        CARD Deck1_PairCard;
        CARD Deck2_PairCard;

        int x, BREAK;

        for (x = 0; x< 5; x++)
        {
            NewDeck_P1.cards[x] = game.communityCards.cards[x];
            NewDeck_P2.cards[x] = game.communityCards.cards[x]; 
        }

        NewDeck_P1.cards[5] = game.players[a].card1;
        NewDeck_P1.cards[6] = game.players[a].card2;

        NewDeck_P2.cards[5] = game.players[b].card1;
        NewDeck_P2.cards[6] = game.players[b].card2;
        
        NewDeck_P1 = SortbyRank(NewDeck_P1);
        NewDeck_P2 = SortbyRank(NewDeck_P2);

        //Finding the number of the Four of a kind for first player
        if (NewDeck_P1.cards[6].rank == NewDeck_P1.cards[5].rank && NewDeck_P1.cards[5].rank == NewDeck_P1.cards[4].rank && NewDeck_P1.cards[4].rank == NewDeck_P1.cards[3].rank)
        {
            Deck1_PairCard = NewDeck_P1.cards[6];
        }
        else if (NewDeck_P1.cards[3].rank == NewDeck_P1.cards[2].rank && NewDeck_P1.cards[5].rank == NewDeck_P1.cards[4].rank && NewDeck_P1.cards[4].rank == NewDeck_P1.cards[3].rank)
        {
            Deck1_PairCard = NewDeck_P1.cards[5];
        }
        else if (NewDeck_P1.cards[3].rank == NewDeck_P1.cards[2].rank && NewDeck_P1.cards[2].rank == NewDeck_P1.cards[1].rank && NewDeck_P1.cards[4].rank == NewDeck_P1.cards[3].rank)
        {
            Deck1_PairCard = NewDeck_P1.cards[4];
        }
        else if (NewDeck_P1.cards[3].rank == NewDeck_P1.cards[2].rank && NewDeck_P1.cards[2].rank == NewDeck_P1.cards[1].rank && NewDeck_P1.cards[1].rank == NewDeck_P1.cards[0].rank)
        {
            Deck1_PairCard = NewDeck_P1.cards[3];
        }

        //Finding the number of the Four of a kind for second player
        if (NewDeck_P2.cards[6].rank == NewDeck_P2.cards[5].rank && NewDeck_P2.cards[5].rank == NewDeck_P2.cards[4].rank && NewDeck_P2.cards[4].rank == NewDeck_P2.cards[3].rank)
        {
            Deck2_PairCard = NewDeck_P2.cards[6];
        }
        else if (NewDeck_P2.cards[3].rank == NewDeck_P2.cards[2].rank && NewDeck_P2.cards[5].rank == NewDeck_P2.cards[4].rank && NewDeck_P2.cards[4].rank == NewDeck_P2.cards[3].rank)
        {
            Deck2_PairCard = NewDeck_P2.cards[5];
        }
        else if (NewDeck_P2.cards[3].rank == NewDeck_P2.cards[2].rank && NewDeck_P2.cards[2].rank == NewDeck_P2.cards[1].rank && NewDeck_P2.cards[4].rank == NewDeck_P2.cards[3].rank)
        {
            Deck2_PairCard = NewDeck_P2.cards[4];
        }
        else if (NewDeck_P2.cards[3].rank == NewDeck_P2.cards[2].rank && NewDeck_P2.cards[2].rank == NewDeck_P2.cards[1].rank && NewDeck_P2.cards[1].rank == NewDeck_P2.cards[0].rank)
        {
            Deck2_PairCard = NewDeck_P2.cards[3];
        }

        if (Deck1_PairCard.rank > Deck2_PairCard.rank )
        {
            return a;
        }

        else if (Deck1_PairCard.rank  < Deck2_PairCard.rank )
        {
            return b;
        }

        else if (Deck1_PairCard.rank  == Deck2_PairCard.rank )
        {

            for (int X = 6; X>1; X--)
            {
                // Go through the deck from low to high and compare the top 5 cards
                // Need to make sure that deck is in ascending order
            
                if(NewDeck_P1.cards[X].rank > NewDeck_P2.cards[X].rank)
                {
                    return a;
                }

                else if (NewDeck_P1.cards[X].rank < NewDeck_P2.cards[X].rank)
                {
                    return b;
                }
            }
        }
    }
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
   
