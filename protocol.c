#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "protocol.h"

int p_recv( player_t *p, char *msg ){

	int sock = p->sck; 
	char *buf= p->buffer; 
	
	int bytes = read(sock, &buf[p->len], BUFSIZE - p->len); 
	p->len += bytes; 

	buf[p->len+1] = '\0';
	
	buf[4] = '\0'; 

	int target; 
	if(strcmp(buf, "MOVE")==0 ) {
		target = 2; 
	}else{
		if(strcmp(buf, "DRAW")==0 ) {
			target = 1; 
		}else{
			if(strcmp(buf, "RSGN")==0 ) {
				target = 1; 
			}else{return 1; }
		}
	}

	char *temp = strtok(&buf[5], "|"); 
	if (temp ==NULL){return 1; }
	
	int x = atoi(temp); 
	int pos = 5 + strlen(temp) +1; 
	int fields=0; 

	while(x>strlen(&buf[pos] ) ){
		bytes= read(sock, &buf[p->len], BUFSIZE - p->len); 
		p->len += bytes; 
			
		buf[p->len +1] = '\0'; 
	}
		
	for(int i=pos; i<(pos+x); i++){
		if(buf[i] == '|'){
			fields++; 
		}
	}
	if(fields != target){ 
		return 1;
	}else{
		memmove(msg, buf, pos + x-1); 
		memmove(buf, &buf[pos+x], p->len - (pos+x)); 
		p->len -= (pos+x);  
		return 0; 
	}

	

	return 0;
}

int move(int sock1, char *msg, char *board){
	char hold[100]; 
	int x = ((int) msg[9] - '0') - 1;
	int y = ((int) msg[11]- '0') - 1;

	//If the cell is already occupied
	if(board[3 * x + y] != '.'){
		strcpy(hold, "INVL|16|position filled|\n");
		write(sock1, hold, strlen(hold));
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

