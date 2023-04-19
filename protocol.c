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

void wrt(int fd, char *buf, int bufLen){
	if(write(fd, buf, bufLen) == -1){
		perror("write");
		exit(1); 
	}
}

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

int checkWin(char *board){
	//Checking the diagonal
	for(int i =0; i<3; i++){
		if(board[3*i]==board[3*i+1] && board[3*i] == board[3*i+2]){
			if(board[3*i]=='.'){}
			else{return 1;}
		}
		if(board[i] == board[i +3] && board[i] == board[i+6] ){
			if(board[i]=='.'){}
			else{return 1;}  
		}
	}
	
	
	//Checking the top right diagonal
	if(board[0] == board[4] && board[0] == board[8] ){
		if(board[0]=='.'){}
			else{return 1;}  
	}
	
	//Checking the top left diagonal
	if(board[2] == board[4] && board[2] == board[6] ){
		if(board[2]=='.'){}
			else{return 1;}  
	}
	
	//Checking for the draw
	int draw = 0; 
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


//If user types the move command
int move(int sock1, char *buf, char *board){
	
	char * hold[1000];
	
	int x = ((int) buf[9] - '0') - 1;
	int y = ((int) buf[11]- '0') - 1;
	
	//If the cell is already occupied
	if(board[3 * x + y] != '.'){
		strcpy(hold, "INVL|16|Position Filled|\n");
		wrt(sock1, hold, strlen(hold));
		return 1; 
	}
	
	//If the desired cell is out of the game board 
	if((3 * x + y) > 9 || (3 * x + y) <=0){
		strcpy(hold, "INVL|18|Outside Of Board|\n");
		wrt(sock1, hold, strlen(hold));
		return 1;
	}

	return 0; 
}

//If the user wants to draw
int draw(int sock1, int sock2, char *buf, char *board, char*hold){
	return 0;
}

//If the active user forefited
int resign(int sock1, int sock2){
	
	wrt(sock1, "OVER|2|L|\n", 10);
	wrt(sock2, "OVER|2|W|\n", 10);
	
	return 0; 
}

