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
            count++;
            printf("%d %d\n" , deck.cards[count].suit, deck.cards[count].rank);

        }
    }
    return deck;
}