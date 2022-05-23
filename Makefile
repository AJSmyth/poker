main0: 
	gcc -std=c11 src/server.c src/deck.c -o bin/poker

tar:
	tar -zcvf Chess_V1.0.tar.gz ./README ./COPYRIGHT ./INSTALL ./bin ./doc/Chess_UserManual.pdf
	tar -zcvf Chess_V1.0_src.tar.gz ./README ./COPYRIGHT ./INSTALL ./doc ./bin ./src ./Makefile