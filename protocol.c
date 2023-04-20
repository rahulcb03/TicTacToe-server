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



int move(struct *player1, struct *player2, char * msg, char *board){
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
	int wL = checkWin(board);
	//player wins
	if(wL==0){
		//handles the win message and loss message
		resign(player1, player2);
	}
	//game continues
	else if(wL==1){
		return 0; 
	}
	//draw case
	else if(wL==2){
		char msgD[100];
		//length = bar + L/W + length of "Game Is Drawn|"
		int lenMSG = 2 + 14; 
		//Conver the length to a string
		char msgBytes[] = toString(lenMSG);

		//Send Message to both players
		strcpy(msgD, "DRAW|");
		strcpy(msgD, msgBytes);
		strcpy(msgD, "|D|");
		strcpy(msgD, "Game Is Drawn|");
		wrt(player1->sck, msgD);
		wrt(player2->sck, msgD)
		return 1;
	}
}

//If the user wants to draw
int draw(int sock1, int sock2, char *buf, char *board, char*hold){
	return 0;
}

//If the active user forefited
int resign(struct *player1, struct *player2){
	char msgL[100];
	//length = bar + L/W + winners name + length of " has won|"
	int lenMSG = 2 + (str.len(player2->)) + 9; 
	//Conver the length to a string
	char msgBytes[] = toString(lenMSG);
	
	//Losers Message
	strcpy(msgL, "OVER|");
	strcpy(msgL, msgBytes);
	strcpy(msgL, "|L|");
	strcpy(msgL, player1->name);
	strcpy(msgL, "has won|");
	wrt(player1->sck, msgL);
	
	//Winners Message
	char msgW[100];
	strcpy(msgW, "OVER|");
	strcpy(msgW, msgBytes);
	strcpy(msgW, "|W|");
	strcpy(msgW, player1->name);
	strcpy(msgW, "has won|");
	wrt(player2->sck, msgW);

	return 0;
}

