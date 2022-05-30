#include<stdio.h>
#include<string.h>	//strlen
#include<sys/socket.h>
#include<arpa/inet.h>	//inet_addr
#include<unistd.h>	//write
#include <pthread.h>
#include "deck.h"
#include "game.h"
#include <fcntl.h>

int player_count = 3;

int player_id = 0;

pthread_mutex_t mutex;

GAMESTATE super_state;

void *connection_handler(void *socket_desc);


int main(int argc , char *argv[])
{
	int socket_desc , client_sock , c;
	struct sockaddr_in server , client;
	
	//Create socket
	socket_desc = socket(AF_INET , SOCK_STREAM , 0);
	if (socket_desc == -1)
	{
		printf("Could not create socket.");
	}
	
	//Prepare the sockaddr_in structure
	server.sin_family = AF_INET;
	server.sin_addr.s_addr = INADDR_ANY;
	server.sin_port = htons( 9000 );
	
	//Bind
	if( bind(socket_desc,(struct sockaddr *)&server , sizeof(server)) < 0)
	{
		//print the error message
		perror("bind failed.");
		return 1;
	}
	puts("Waiting for players...");
	//Listen
	if(listen(socket_desc , player_count)){
		perror("listen failed.");
		return 1;
	}
	
	//Accept and incoming connection
	c = sizeof(struct sockaddr_in);
	pthread_t thread[player_count];
	int i = 0;
	
	pthread_mutex_init(&mutex, NULL);
	//accept connection from an incoming client
	while(client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)){
        //create thread for each accepted connection
		char out[25];
		sprintf(out, "Creating thread for new player.");
		puts(out);
        if( pthread_create( thread+i, NULL ,  connection_handler , (void*) &client_sock) < 0)
        {
            perror("could not create thread");
            return 1;
        }
		i++;
	}

	for(int i = 0; i < player_count; i++){
		pthread_join(thread[i], NULL);
	}
	
	return 0;
}

void *connection_handler(void *socket_desc)
{
    //Get the socket descriptor
    int sock = *(int*)socket_desc;
	//player id local to thread
	int local_id;
	//local game state
	GAMESTATE local_game;
	//lock game state initialization code
	pthread_mutex_lock(&mutex);
	//begin initial handshake and assign player id
	write(sock , &player_id , sizeof(player_id));
	char out[100];
	sprintf(out, "Sent player ID: %d", player_id);
	puts(out);

	local_id = player_id;
	player_id++;
	//get initial game state from player 0 AKA the host
	if(local_id == 0){
		puts("Waiting for initial game...");
		if(read(sock , &super_state , sizeof(super_state)) < 0){
			printf("failed to recieve initial game state.");
			close(sock);
			return 1;
		}
		local_game = super_state;
		player_count = super_state.numberPlayers;
		puts("Initial game recieved");
	}
	pthread_mutex_unlock(&mutex);
	//wait for all players to join
	bool done = false;
	while (!done) {
		// sprintf(out, "%d", player_id);
		// puts(out);
		pthread_mutex_lock(&mutex);
		done = player_id == player_count;
		pthread_mutex_unlock(&mutex);
	}

	//echo game state to all other players
	if(local_id != 0){
		sprintf(out, "Sent player %d the game state.", local_id);
		puts(out);
		write(sock , &super_state , sizeof(super_state));
		local_game = super_state;
	}
	//recieve gamestate and echo
	fcntl(sock, F_SETFL, fcntl(sock, F_GETFL, 0) | O_NONBLOCK);
	while(1){
		read(sock , &super_state , sizeof(super_state));
		if(super_state.playerTurn != local_game.playerTurn || super_state.stage != local_game.stage){
			sprintf(out, "Updating player %d with the new game state.", local_id);
			puts(out);
			write(sock , &super_state , sizeof(super_state));
			local_game = super_state;
		}
	}
    return 0;
} 