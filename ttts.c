#define _POSIX_C_SOURCE 200809L
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <netdb.h>
#include <signal.h>
#include "protocol.h"

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
		perror("getaddrinfo");
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

int checkName(player_t *head, char *name){
	player_t *ptr = head; 
	name[strlen(name) -1] = '\0'; 
	while(ptr != NULL){
		if(strcmp(ptr->name, name) ==0){ return 1; }
	}
	return 0; 

}
	

int main(int argc, char ** argv){
	if(argc != 2){
		puts("Ussage: ./ttts <int>");
		return EXIT_FAILURE; 
	}

	struct sockaddr_storage remote_host;
	socklen_t remote_host_len;

	char hold[100]; 
	
	//call openListener to listen to the port, returns a socket FD
	int listener = openListener(argv[1] , QUEUE_SIZE); 

	if(listener <0){ return EXIT_FAILURE; }

	remote_host_len = sizeof(remote_host);
	
	player_t *head = NULL;
	player_t *ptr = head; 
	player_t *p; 
	char msg[50]; 	
	
	while(1){
		//get the socket FD connected to the 1st client 
		int sock1 = accept(listener, (struct sockaddr *)&remote_host, &remote_host_len);
	
		if (sock1 < 0) {
			perror("accept");
	
		}
			
		if(ptr==NULL){
			head = (player_t *)malloc(sizeof(player_t) ); 
			head->len =0; 
			head->sck = sock1; 
			head->next = NULL; 
		}	
		else{
			ptr->next = (player_t *)malloc(sizeof(player_t) ); 
			ptr = ptr->next; 
			ptr->len =0; 	
			ptr->sck = sock1; 
			ptr->next = NULL; 
		}
		
		
		//check if client1 types PLAY 
		int cont; 
		do{
			cont =0; 
			if(p_recv(ptr, msg) ){
				//FIXME: disconnect 
			}				
				
			if(strcmp(msg, "PLAY") ==0){
				if( checkName(head, &msg[5+strlen(&msg[5])+1] )) {
					strcpy(hold, "INVL|12|name in use|"); 
					wrt(sock1, hold, strlen(hold) );
				      	cont =1; 	
					continue; 
				}
				
				strcpy(ptr->name, &msg[5+strlen(&msg[5])+1] ); 
				strcpy(hold, "WAIT|0|\n") ;
				write(sock1, hold, strlen(hold) ) ; 	
				p = ptr; 
			
			 
			}
			else{
				strcpy(hold, "INVL|19|ussage: PLAY <str>|");
				write(sock1, hold, strlen(hold) ) ; 
				cont=1; 
			}

		}while(cont); 
		
		//create a socket FD for the second client 
		int sock2 = accept(listener, (struct sockaddr *)&remote_host, &remote_host_len);
		
		if (sock2 < 0) {
			perror("accept");
	
		}
		
		ptr->next = (player_t *)malloc(sizeof(player_t) ); 
		ptr = ptr->next; 
		ptr->len =0; 	
		ptr->sck = sock2; 
		ptr->next = NULL; 

		//check if client2 types PLAY and the name is valid 
		do{
			cont =0; 
			if(p_recv(ptr, msg) ){
				//FIXME: disconnect 
			}				
				
			if(strcmp(msg, "PLAY") ==0){
				//do something with name herei
				if( checkName(head, &msg[5+strlen(&msg[5])+1] )) {
					strcpy(hold, "INVL|12|name in use|"); 
					wrt(sock1, hold, strlen(hold) );
				      	cont =1; 	
					continue; 
				}
				
				strcpy(ptr->name, &msg[5+strlen(&msg[5])+1] ); 
				strcpy(hold, "WAIT|0|\n") ;
				write(sock1, hold, strlen(hold) ) ; 	
			 
			}
			else{
				strcpy(hold, "INVL|19|ussage: PLAY <str>|");
				write(sock1, hold, strlen(hold) ) ; 
				cont=1; 
			}

		}while(cont); 

		//this will call play game which will handle the game 
		playGame(p, ptr); 	
	}

	return EXIT_SUCCESS; 

	
}

