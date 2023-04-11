
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/socket.h>
#include <netdb.h>
#include <string.h>
int connect_inet(char *host, char *service){

	struct addrinfo hints, *info_list, *info;
	int sock, error;

	// look up remote host
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC; // in practice, this means give us IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM; // indicate we want a streaming socket
					 //
	error = getaddrinfo(host, service, &hints, &info_list);
	
	if (error) {
		fprintf(stderr, "error looking up %s:%s: %s\n", host, service,
		gai_strerror(error));
		return -1;
	}
	
	for (info = info_list; info != NULL; info = info->ai_next) {
		sock = socket(info->ai_family, info->ai_socktype, info->ai_protocol);

		if (sock < 0) continue;
			error = connect(sock, info->ai_addr, info->ai_addrlen);

		if (error) {
			close(sock);
			continue;
		}
		break;
	}
	freeaddrinfo(info_list);
	
	if (info == NULL) {
		fprintf(stderr, "Unable to connect to %s:%s\n", host, service);
		return -1;
	}
	
	return sock;
}

void wrt(int fd, char *buff, int len ){
	if(write(fd, buff, len) <=0){
		perror("write"); 
		exit(1); 
	}
	return; 
}

#define BUFLEN 256

int main(int argc, char **argv){
	int sock, bytes;
	char buf[BUFLEN];
	
	if (argc != 3) {
		printf("Specify host and service\n");
		exit(EXIT_FAILURE);
	}

	//calls connect_inet to connect to server
	sock = connect_inet(argv[1], argv[2]);
	
	if (sock < 0) exit(EXIT_FAILURE);
	
	//keeps getting input until the retrun from the server is not INVL
	do{
		scanf("%s", buf); 
		wrt(sock, buf, strlen(buf) );
       		bytes = read(sock, buf, BUFLEN); 
		wrt(1, buf, bytes); 
		buf[4] ='\0'; 
	}while(strcmp(buf, "INVL") == 0); 
	
	//will read from the server to get the BEGN command
	bytes = read( sock, buf, BUFLEN); 
	wrt(1, buf, bytes);
       	buf[bytes] ='\0';	
	
	//do more stuff
	
	//Check if the user is the 'X' player and then read from the Standard output

	//Use strtok to get the token related to the X/O
	char* X_or_O = strtok(buf, "|");
	X_or_O = strtok(buf, "|");
	X_or_O = strtok(buf, "|");

	if((*X_or_O) == "X"){
		//Read from the STDIN of the player that is playing with X
		do{
			scanf("%s", buf);
			wrt(sock, buf, strlen(buf));
			bytes = read(sock, buf, BUFLEN);
			wrt(1, buf, bytes);
			buf[4] = '\0';
		}while(strcmp(buf, "INVL")==0);
	}



	close(sock);
	
	return EXIT_SUCCESS;
}
