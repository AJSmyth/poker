#include<stdio.h>
#include<string.h>	//strlen
#include<sys/socket.h>
#include<arpa/inet.h>	//inet_addr
#include<unistd.h>	//write
#include <pthread.h>
#include "deck.h"
#include "game.h"

const int player_count = 9;

int player_id = 0;

pthread_mutex_t mutex;

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
	server.sin_port = htons( 9001 );
	
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
	pthread_t* thread;
	
	pthread_mutex_init(&mutex, NULL);
	//accept connection from an incoming client
	while(client_sock = accept(socket_desc, (struct sockaddr *)&client, (socklen_t*)&c)){
        //create thread for each accepted connection
		char out[25];
		sprintf(out, "Creating thread for player: %d", player_id);
		puts(out);
        if( pthread_create( &thread , NULL ,  connection_handler , (void*) &client_sock) < 0)
        {
            perror("could not create thread");
            return 1;
        }
	}
	
	return 0;
}

void *connection_handler(void *socket_desc)
{
    //Get the socket descriptor
    int sock = *(int*)socket_desc;
    
	write(sock , &player_id , sizeof(player_id));
	char out[25];
	sprintf(out, "Sent player ID: %d", player_id);
	puts(out);
	pthread_mutex_lock(&mutex);
	player_id++;
	pthread_mutex_unlock(&mutex);
    return 0;
} 