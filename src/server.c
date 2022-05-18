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

  int n = 5; //Number of players

  PLAYER player[n-1];

  GAMESTATE game;
  game.shuffleDeck = ShuffleCards (deck);
  //for(int a = 0; a<=51;a++)
  //{
  //printf("%d %d\n" , game.shuffleDeck.cards[a].suit, game.shuffleDeck.cards[a].rank);
  //}

  game.GameCount = 0;
  game.numberPlayers = 4;

  for(int a = 0; a<=4;a++)
  {
    game.players[a].action = NoAction;
  }

  game.stage = PREFLOP;

  game = AssignCards(game);
  game = PREFLOP1 (game);



  for(int i = 0;i<=4;i++)
  {
    printf("%d Player suit is %d and rank is %d\n", i , game.players[i].card1.suit, game.players[i].card1.rank);
    printf("%d Player suit is %d and rank is %d\n", i , game.players[i].card2.suit, game.players[i].card2.rank);
    printf("%d Player Bet is %d\n", i , game.players[i].Bid);
  }

  for (int a = 0; a<= 2; a++)
  {
  printf("Card %d in Flop is: suit = %d and rank = %d\n", a+1, game.players[a].card1.suit, game.players[a].card1.rank);
  }
return 0;
}
