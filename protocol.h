#ifndef PROTOCOL_H
#define PROTOCOL_H

#define BUFSIZE 200
// Define struct player
typedef struct player {
	int sck; 
	char name[50];
	char buffer[BUFSIZE]; 
   	int len;
	struct player *next;  
}player_t;

// Declare method 
int p_recv( player_t *p, char *msg);

void  wrt(int fd, char *buf, int bufLen); 

void playGame(player_t *player1, player_t *player2); 


#endif /* PROTOCOL_H */
