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


//Writing to the STDOUT
void wrt(int fd, char *buf, int bufLen){
	if(write(fd, buf, bufLen) == -1){
		perror("write");
		exit(1); 
	}
}

//return 1 if invalid structure of user input
//return 0 is valid structure of user input
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



int move(struct *player1, char * msg, char *board){
	char hold[100]; 
	int x = ((int) msg[9] - '0') - 1;
	int y = ((int) msg[11]- '0') - 1;

	//If the cell is already occupied
	if(board[3 * x + y] != '.'){
		strcpy(hold, "INVL|16|position filled|\n");
		write(player1->sck, hold, strlen(hold));
		return 1;
	}
	
	//If the cell is out of board
	if(!(x<=3  && x >0 && y<=3 && y>0)){
		strcpy(hold, "INVL|29|Unplayable: outside of board|\n");
		write(player1->sck, hold, strlen(hold));
		return 1;
	}
	
	//If the cell entered is valid 
	board[3 * x + y] = msg[7];
	
	//Check Win
	return 0;
}

//If the user wants to draw
int draw(int sock1, int sock2, char *buf, char *board, char*hold){
	return 0;
}

//If the active user forefited
int resign(struct *player1, struct *player2){
	char msg[100];
	//length = bar + L/W + winners name + length of " has won|"
	int lenMSG = 2 + (str.len(player2->)) + 9; 
	
	
	//Losers Message
	strcpy(msg, ("OVER|%d|L|%s has won|",  lenMSG, player2->name));
	wrt(player1->sck, msg);
	
	//Winners Message
	char msgW[100];
	strcpy(msgW, ("OVER|%d|W|%s has won|",  lenMSG, player2->name));
	wrt(player2->sck, msgW);

	return 0;
}

