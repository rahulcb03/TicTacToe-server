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

	buf[p->len] = '\0';
	
	buf[4] = '\0'; 
	
	//target refers to the fields 
	int target, expctBytes; 
	if(strcmp(buf, "MOVE")==0 ) {
		target = 2; 
		expctBytes = 6; 
	}else{
		if(strcmp(buf, "DRAW")==0 ) {
			target = 1;
		       	expctBytes = 2; 	
		}else{
			if(strcmp(buf, "RSGN")==0 ) {
				target = 1; 
				expctBytes = 0; 
			}else{
				if(strcmp(buf, "PLAY") ==0 ){
					target = 1; 
					expctBytes = -1; 
				}else{return 1;}
			}
		}
	}

	char *temp = strtok(&buf[5], "|"); 
	if (temp ==NULL){return 1; }
	
	int x = atoi(temp); 
	int pos = 5 + strlen(temp) +1; 
	int fields=0; 
	
	if(expctBytes != -1 && x!= expctBytes) {return 1; }

	while(x>strlen(&buf[pos] ) ){
		bytes= read(sock, &buf[p->len], BUFSIZE - p->len); 
		p->len += bytes; 

		for(int i= pos; i<(pos+x) && i<p->len; i++){
			if(buf[i] == '|'){ fields++; }
		}
		if(fields>target) {return 1; }
		if(fields<target && x<strlen(&buf[pos]) ){ return 1;}
			
		buf[p->len] = '\0'; 
	}
		
	memmove(msg, buf, pos + x-1); 
	memmove(buf, &buf[pos+x], p->len - (pos+x)); 
	p->len -= (pos+x);  
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


void playGame( player_t *player1 , player_t *player2){
	//this will handle the actuall playing of the game

	//First write to both sockets to BEGN sock1: X, sock2: O
	

	char hold[100]; 
	int length = 2+strlen(player2->name)+1; 
	snprintf(hold, 100, "BEGN|%d|X|%s\n", length, player2->name); 
	wrt(player1->sck, hold, strlen(hold) ); 

	length = 2+strlen(player1->name) +1; 
	snprintf(hold, 100, "BEGN|%d|X|%s\n", length, player1->name); 
	wrt(player2->sck, hold, strlen(hold)); 

	
	char board[] = "........."; //empty board
	int count =0 ;
	int overFlag=0, contFlag; 
	int x, y, res; 
	player_t *swap1, *swap2; 
	

	char msg[100]; 	
	do{ 
		
		if(count%2 == 0){
			//X turn 
			swap1 = player1; 
			swap2 = player2;
		}else{
			//O turn 
			swap1 = player2; 
			swap2 = player1; 
		}

		do{
			contFlag =0; 		
			//read command from sock1
			if(p_recv(swap1, msg)){
				//FIXME: this should end the connection and the game
				strcpy(hold, "INVL|14|format error|"); 
				wrt(swap1->sck, hold, strlen(hold) ); 
				contFlag =1; 
			}	

											
			//if the command is MOVE 
			if(strcmp(msg, "MOVE") ==0){
				x = ( (int) msg[9] - '0' ) - 1; 
			 	y = ((int) msg[11] - '0' ) -1; 

				//if space is occupied send INVL to sock1 (flag =1)
				if(board[3 * x + y ] != '.'){
					strcpy(hold, "INVL|16|position filled|\n"); 
					wrt(swap1->sck, hold, strlen(hold) ); 
					contFlag = 1; 
					continue; 
				}
				
				//set the board at the cords given 
					
				board[3*x + y] = msg[7]; 
				
				//check if win or draw
				res  = checkWin(board); 
				if(res == 1){	
					length = strlen(player1->name) + 11; 
					snprintf(hold, 50, "OVER|%d|W|%s has won|\n", length, player1->name); 
					wrt(swap1->sck, hold, strlen(hold) );
					hold[8] = 'L'; 
					wrt(swap2->sck, hold, strlen(hold)); 
					overFlag = 1; 
				}else{
					if(res == 2){ 	
						strcpy(hold, "OVER|7|D|draw|\n"); 
						wrt(swap1->sck, hold, strlen(hold) ); 
						wrt(swap2->sck, hold, strlen(hold) );				
						overFlag = 1; 
					}else{
						strcpy(hold, "MOVD|16|");
					       	strcat(hold, &msg[7]); 
						strcat(hold, board); 	
						strcat(hold, "\n"); 
						wrt(swap1->sck, hold, strlen(hold) );
						wrt(swap2->sck , hold, strlen(hold) ); 	

					}
				}
			}
			if(strcmp(msg, "RSGN" )==0 ){
				length = strlen(player1->name) + 16; 
				snprintf(hold, 50, "OVER|%d|L|%s has resigned|\n", length, player1->name); 
				wrt(swap1->sck, hold, strlen(hold) );
				hold[8] = 'L'; 
				wrt(swap2->sck, hold, strlen(hold)); 
				overFlag = 1; 
			}


			//if DRAW S
			if(strcmp(msg,"DRAW" ) == 0 && msg[7] == 'S' ){
				wrt(swap2->sck, "DRAW|2|S|\n" , 10); 
				int drawFlag; 
				do{
					drawFlag =0; 
					if(p_recv(swap2, msg) ){
						strcpy(hold, "INVL|14|format error|"); 
						wrt(swap2->sck, hold, strlen(hold) ); 
						drawFlag =1; 
					}
					if(strcmp(msg, "DRAW") != 0 ){
						strcpy(hold, "INVL|11|enter DRAW|\n");
						wrt(swap2->sck, hold, strlen(hold) ) ; 
						drawFlag =1; 
						continue; 
					}
					if(msg[7] == 'R' ){
						strcpy(hold, "DRAW|2|R|\n" );
						wrt(swap1->sck, hold, strlen(hold) ) ; 
						contFlag =1; 
					}
					if(msg[7] == 'A') {
						strcpy(hold, "OVER|7|D|draw|\n" );
						wrt(swap1->sck, hold, strlen(hold) ); 
						wrt(swap2->sck, hold, strlen(hold) ); 
						overFlag = 1; 
					}
					if(msg[7] != 'A' && msg[7] != 'R' ){
						strcpy(hold, "INVL|13|enter A or S|\n");
						wrt(swap2->sck, hold, strlen(hold) ) ; 
						drawFlag =1; 
						continue; 
					}
				}while(drawFlag);
			       		
					
			}


		}while(contFlag); 			
		count++; 	
		
	}while(!overFlag); 
}


