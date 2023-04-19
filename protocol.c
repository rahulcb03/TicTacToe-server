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

//If user types the move command
int move(int sock1, char *buf, char *board, char *hold){
	
	int x = ((int) buf[9] - '0') - 1;
	int y = ((int) buf[11]- '0') - 1;
	
	//If the cell is already occupied
	if(board[3 * x + y] != '.'){
		strcpy(hold, "INVL|16|position filled|\n");
		wrt(sock1, hold, strlen(hold));
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


