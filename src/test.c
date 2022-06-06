#include "game.h"
#include <stdlib.h>
#include <time.h>
#include <stdio.h>
#include <limits.h>

int test_for_tiebreaker(void);

int main() {
    test_for_tiebreaker();
    return 0;
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