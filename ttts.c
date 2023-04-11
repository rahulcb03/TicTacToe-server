#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netdb.h>
#define QUEUE_SIZE 8

int openListener(char *port, int qLen){

	struct addrinfo hint, *info_list, *info;
	int error, sock;

	// initialize hints
	memset(&hint, 0, sizeof(struct addrinfo));
	hint.ai_family = AF_UNSPEC;
	hint.ai_socktype = SOCK_STREAM;
	hint.ai_flags = AI_PASSIVE;

	// obtain information for listening socket
	error = getaddrinfo(NULL, port, &hint, &info_list);

	if (error) {
		fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(error));
		return -1;
	}

	// attempt to create socket
	for (info = info_list; info != NULL; info = info->ai_next) {

		sock = socket(info->ai_family, info->ai_socktype, info->ai_protocol);

		// if we could not create the socket, try the next method
		if (sock == -1) continue;

		// bind socket to requested port
		error = bind(sock, info->ai_addr, info->ai_addrlen);
		if (error) {
			close(sock);
			continue;
		}

		// enable listening for incoming connection requests
		error = listen(sock, qLen);
		if (error) {
			close(sock);
			continue;
		}

		// if we got this far, we have opened the socket
		break;
	}freeaddrinfo(info_list);

	// info will be NULL if no method succeeded
	if (info == NULL) {
		fprintf(stderr, "Could not bind\n");
		return -1;
	}

	return sock;
}

void playGame( int sock1, int sock2){
	//this will handle the actuall playing of the game
}

#define BUFFLEN 200
int main(int argc, char ** argv){
	if(argc != 2){
		puts("Ussage: ./tts <int>");
		return EXIT_FAILURE; 
	}

	struct sockaddr_storage remote_host;
	socklen_t remote_host_len;
	char buff[BUFFLEN];

	char holder[100]; 
	
	//call openListener to listen to the port, returns a socket FD
	int listener = openListener(argv[1] , QUEUE_SIZE); 

	if(listener <0){ return EXIT_FAILURE; }

	remote_host_len = sizeof(remote_host);
	
	//get the socket FD connected to the 1st client 
	int sock1 = accept(listener, (struct sockaddr *)&remote_host, &remote_host_len);
	
	if (sock1 < 0) {
		perror("accept");
	
	}
	
		
	//check if client1 types PLAY 
	do{
		read(sock1, buff, BUFFLEN); 
		buff[4] = '\0'; 

		if(strcmp(buff, "PLAY") ==0){
			//FIXME: do something with name here
			 strcpy(holder, "WAIT|0|") ;
			write(sock1, holder, strlen(holder) ) ; 	
			 
		}
		else{
			strcpy(holder, "INVL|19|ussage: PLAY <str>|");
			write(sock1, holder, strlen(holder) ) ; 
		}

	}while(strcmp(buff, "PLAY") != 0); 
	
	//create a socket FD for the second client 
	int sock2 = accept(listener, (struct sockaddr *)&remote_host, &remote_host_len);
		
	if (sock2 < 0) {
		perror("accept");
	
	}

	//check if client2 types PLAY and the name is valid 
	do{
		read(sock2, buff, BUFFLEN); 
		buff[4] = '\0'; 

		if(strcmp(buff, "PLAY") ==0){
			//FIXME: do something with name here
			strcpy(holder, "WAIT|0|");	
			write(sock2, holder, strlen(holder) ) ; 	
			
		}
		else{
			strcpy(holder,  "INVL|19|ussage: PLAY <str>|");
			write(sock2, holder, strlen(holder) ) ; 
		}

	}while(strcmp(buff, "PLAY") != 0); 

	//this will call play game which will handle the game 
	playGame(sock1, sock2); 	

	return EXIT_SUCCESS; 

	
}
