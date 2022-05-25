test: 
	gcc -std=c11 src/client.c src/game.c src/deck.c -o bin/poker
clean:
	rm bin/poker

tar:
	tar -zcvf rename_later.tar.gz ./README ./COPYRIGHT ./INSTALL ./bin ./doc/Poker_UserManual.pdf
	tar -zcvf Poker_Alpha.tar.gz ./README ./COPYRIGHT ./INSTALL ./doc ./bin ./src ./Makefile
