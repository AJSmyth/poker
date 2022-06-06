#include<stdio.h>
#include<string.h>	//strlen
#include<sys/socket.h>
#include<arpa/inet.h>	//inet_addr
#include<unistd.h>	//write
#include <pthread.h>
#include "deck.h"
#include "game.h"
//#include "net.h"
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
	server.sin_port = htons( atoi(argv[1]));
	
	printf("Started socket on port %d", atoi(argv[1]));
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
	pthread_mutex_lock(&mutex);
	if (write(sock , &super_state , sizeof(super_state)) >= 0) {
		sprintf(out, "Sent player %d the game state.", local_id);
		puts(out);
	}
	pthread_mutex_unlock(&mutex);

	//recieve gamestate request and serve
	PacketType type;
	while(1){
		read(sock, &type, sizeof(type));
		sprintf(out, "\nPlayer %d %s", local_id, (type == GS_UPDATE) ? "attempted update" : (type == GS_REQUEST) ? "attempted request" : "unknown action");
		puts(out);
		if (type == GS_UPDATE) {
			pthread_mutex_lock(&mutex);
			if (read(sock , &super_state , sizeof(super_state)) >= 0) {
				sprintf(out, "\nUpdating gamestate from player %d.", local_id);
				puts(out);
			}
			pthread_mutex_unlock(&mutex);
		} else if (type == GS_REQUEST) {
			pthread_mutex_lock(&mutex);
			if (write(sock, &super_state, sizeof(super_state)) >= 0) {
				sprintf(out, "\nSending gamestate to player %d.", local_id);
				puts(out);
			}
			pthread_mutex_unlock(&mutex);
		}
	}
    return 0;
} 
