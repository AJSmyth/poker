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

  PLAYER player[4];
  deck = ShuffleCards (deck);

  deck = AssignCards(player, deck);
  
  
  for(int i = 0;i<=4;i++) //CHECKING THE CARDS EACH PLAYER HAS
  {
      printf("%d Player suit is %d and rank is %d\n", i , player[i].card1.suit, player[i].card1.rank);
      printf("%d Player suit is %d and rank is %d\n", i , player[i].card2.suit, player[i].card2.rank);
  }

  for(int i = 0;i<=51;i++) //CHECKING THE REMAINING CARDS IN THE DECK
  {
      printf("Deck %d suit is %d and rank is %d\n", i , deck.cards[i].suit, deck.cards[i].rank);
  }
}
