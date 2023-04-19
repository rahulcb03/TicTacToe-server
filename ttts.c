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

#define QUEUE_SIZE 8
#define BUFSIZE 200

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

void wrt(int fd, char *buf, int bufLen){
	if(write(fd, buf, bufLen) == -1){
		perror("write");
		exit(1); 
	}
}

//return 1 if invalid form
//return 0 is valid 
int checkForm(char *buf, int bytes ){
	buf[bytes] = '\0';
	
	buf[4] = '\0'; 

	if(strcmp(buf, "MOVE") != 0 && strcmp(buf, "RSGN") != 0 && strcmp (buf, "DRAW") != 0 ){ return 1; }

	char *temp = strtok(&buf[4], "|"); 
	if (temp ==NULL){return 1; }
	
	int x = atoi(temp); 
	int pos = 5 + strlen(temp) +1; 
	if( x != strlen(&buf[pos]) ) {return 1; }

	return 0;
}

//return 0 if nothing 
//return 1 if Win
//return 2 if Draw 
int checkWin(char *board){
	//Win Scenarios
	for(int i =0 ; i<3; i++){
		if(board[3*i] == board[3*i +1] && board[3*i] == board[3*i +2] ){
			if(board[3*i]=='.'){}
			else{return 1;} 
		}
		if(board[i] == board[i +3] && board[i] == board[i+6] ){
			if(board[i]=='.'){}
			else{return 1;}  
		}
	}
	if(board[0] == board[4] && board[0] == board[8] ){
		if(board[0]=='.'){}
			else{return 1;}  
	}
	if(board[2] == board[4] && board[2] == board[6] ){
		if(board[2]=='.'){}
			else{return 1;}  
	}
	
	int draw = 0; 
	//Checking for the draw
	for(int i=0; i<9; i++){
		if(board[i] == '.'){
			//If there is still a spot open then it isn't a draw
			draw =1; 	
		}
	}
	
	if(draw ==0){
		return 2;	
	}
	//Nothing
	return 0; 
}
	
void playGame( int sk1, int sk2){
	//this will handle the actuall playing of the game

	//First write to both sockets to BEGN sock1: X, sock2: O
	
	wrt(sk1, "BEGN|2|X|", 9); 
	wrt(sk2, "BEGN|2|0|", 9); 

	
	char board[9] = "........."; //empty board
	int count =0 ;
	int overFlag, contFlag; 
	int bytes, x, y, res, sock1, sock2; 
	char hold[100]; 

	char buf[BUFSIZE]; 
		
	do{ 
		
		if(count%2 == 0){
			sock1 = sk1; 
			sock2 = sk2;
		}else{
			sock1 = sk2; 
			sock2 = sk1; 
		}

		do{
			contFlag =0; 		
			//read command from sock1
			bytes = read(sock1, buf, BUFSIZE); 

			//check if command is valid formating 
			//check form should return 1 or 0
			//should seperate the fields by \0
			if ( checkForm(buf, bytes) ){
				strcpy(hold, "INVL|17|incorrect format|\n"); 
				wrt(sock1, hold, strlen(hold) ); 
				contFlag=1; 
				continue; 
			}
					
			//if the command is MOVE 
			if(strcmp(buf, "MOVE") ==0){
				x = ( (int) buf[9] - '0' ) - 1; 
			 	y = ((int) buf[11] - '0' ) -1; 

				//if space is occupied send INVL to sock1 (flag =1)
				if(board[3 * x + y ] != '.'){
					strcpy(hold, "INVL|16|position filled|\n"); 
					wrt(sock1, hold, strlen(hold) ); 
					contFlag = 1; 
					continue; 
				}
				
				//set the board at the cords given 
					
				board[3*x + y] = buf[7]; 
				
				//check if win or draw
				res  = checkWin(board); 
				if(res == 1){	
					strcpy(hold, "OVER|2|W|\n"); 
					wrt(sock1, hold, strlen(hold) );
					strcpy(hold, "OVER|2|L|\n");
					wrt(sock2, hold, strlen(hold)); 
					overFlag = 1; 
				}else{
					if(res == 2){ 	
						strcpy(hold, "OVER|2|D|\n"); 
						wrt(sock1, hold, strlen(hold) ); 
						wrt(sock2, hold, strlen(hold) );
						overFlag = 1; 
					}else{
						strcpy(hold, "MOVD|12|"); 
						wrt(sock1, hold, strlen(hold) );
						wrt(sock2 , hold, strlen(hold) ); 
						wrt(sock1, &buf[7], 1); 
						wrt(sock2, &buf[7], 1);
						wrt(sock1, "|", 1);
						wrt(sock2, "|", 1); 
						wrt(sock1, board, strlen(board) ); 
						wrt(sock2, board, strlen(board) ); 

					}
				}
			}
			if(strcmp(buf, "RSGN" ) ){
				wrt(sock1, "OVER|2|L|\n", 10); 
				wrt(sock2 , "OVER|2|W|\n", 10); 
				overFlag = 1; 

			}


			//if DRAW S
			if(strcmp(buf,"DRAW" ) == 0 && buf[7] == 'S' ){
				wrt(sock2, "DRAW|2|S|\n" , 10); 
				int drawFlag; 
				do{
					drawFlag =0; 
					bytes = read(sock2, buf, BUFSIZE); 
					if ( checkForm(buf, bytes) ){
						strcpy(hold, "INVL|17|incorrect format|\n"); 
						wrt(sock1, hold, strlen(hold) ); 
						drawFlag=1; 
						continue; 
					}
					if(strcmp(buf, "DRAW") != 0 ){
						strcpy(hold, "INVL|11|enter DRAW|\n");
						wrt(sock2, hold, strlen(hold) ) ; 
						drawFlag =1; 
						continue; 
					}
					if(buf[7] == 'R' ){
						strcpy(hold, "DRAW|2|R|\n" );
						wrt(sock1, hold, strlen(hold) ) ; 
						contFlag =1; 
					}
					if(buf[7] == 'A') {
						strcpy(hold, "OVER|2|D|\n" );
						wrt(sock1, hold, strlen(hold) ); 
						wrt(sock2, hold, strlen(hold) ); 
						overFlag = 1; 
					}
					if(buf[7] != 'A' && buf[7] != 'R' ){
						strcpy(hold, "INVL|13|enter A or S|\n");
						wrt(sock2, hold, strlen(hold) ) ; 
						contFlag =1; 
						continue; 
					}
				}while(drawFlag);
			       		
					
			}


		}while(contFlag); 			
		
		
	}while(!overFlag); 
}

int main(int argc, char ** argv){
	if(argc != 2){
		puts("Ussage: ./ttts <int>");
		return EXIT_FAILURE; 
	}

	struct sockaddr_storage remote_host;
	socklen_t remote_host_len;
	char buff[BUFSIZE];

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
		read(sock1, buff, BUFSIZE); 
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
		read(sock2, buff, BUFSIZE); 
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
