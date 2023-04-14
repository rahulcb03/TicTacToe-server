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
	}
	freeaddrinfo(info_list);

	// info will be NULL if no method succeeded
	if (info == NULL) {
		fprintf(stderr, "Could not bind\n");
		return -1;
	}

	return sock;
}
#define BUFFLEN 200
int main(int argc, char ** argv){
	if(argc != 2){
		puts("Ussage: ./tts <int>");
		return EXIT_FAILURE; 
	}

	struct sockaddr_storage remote_host;
	socklen_t remote_host_len;
	
	int listener = openListener(argv[1] , QUEUE_SIZE); 
	if(listener <0){ return EXIT_FAILURE; }

	remote_host_len = sizeof(remote_host);
	
	int sock = accept(listener, (struct sockaddr *)&remote_host, &remote_host_len);
	
	if (sock < 0) {
		perror("accept");
	
	}
	
	char buff[BUFFLEN]; 
	int byte = read(sock, buff, BUFFLEN); 
	write(1,buff, byte); 

	return EXIT_SUCCESS; 

	
}
