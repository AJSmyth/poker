test: 
	gcc -std=c11 src/client.c src/game.c src/deck.c -o bin/poker
	gcc -std=c11 -pthread src/server.c src/game.c src/deck.c -o bin/server
clean:
	rm bin/poker

tar:
	tar -zcvf Poker_Beta.tar.gz ./README ./COPYRIGHT ./INSTALL ./bin ./doc/Poker_UserManual.pdf
	tar -zcvf Poker_Beta_src.tar.gz ./README ./COPYRIGHT ./INSTALL ./doc ./bin ./src ./Makefile
