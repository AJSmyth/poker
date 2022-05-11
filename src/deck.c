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


DECK AssignCards (PLAYER player[], DECK deck)
{
    int i,x;
    for (i = 0, x = 0; i <= 4; i++, x=x+2)
    {
        player[i].card1 = deck.cards[x]; //Assign 1st Card to Player[i]
        player[i].card2 = deck.cards[x+1]; //Assign 2nd Card ro Player[i]
    }

    for(int a = 0; a<= 51 - 10;a++)
    {
        deck.cards[a] = deck.cards[a+10]; //Changing the deck by removing first 10 cards
    }

    return deck;
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
