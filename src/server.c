#include "deck.h"
#include <stdio.h>

int main(){
  DECK deck = INIT();

  /*CARD c1 = {SPADES, 3};
  CARD c2 = {HEARTS, 4};
  PLAYER p1;
  p1.card1 = c1;
  p1.card2 = c2;
  printf("%d %d", p1.card1.rank, p1.card2.suit);*/

  deck = ShuffleCards (deck);

  int n = 5; //Number of players

  PLAYER player[n-1];

  GAMESTATE game;
  game.stage = PREFLOP;
  AssignCards(player, deck, n-1, game);
  PREFLOP1 (player, deck, n-1);


  for(int i = 0;i<=4;i++)
  {
    printf("%d Player suit is %d and rank is %d\n", i , player[i].card1.suit, player[i].card1.rank);
    printf("%d Player suit is %d and rank is %d\n", i , player[i].card2.suit, player[i].card2.rank);
    printf("%d Player Bet is %d\n", i ,player[i].Bid);
  }
}