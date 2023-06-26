#ifndef PROTOCOL_H
#define PROTOCOL_H

#define BUFSIZE 200
// Define struct player
typedef struct player {
	int sck; 
	char name[50];
	char buffer[BUFSIZE]; 
   	int len;
	char piece; 
	struct player *next;  
}player_t;

typedef struct threadArgs{
	player_t **h;
	player_t *node; 
}threadArgs;
// Declare method 
int p_recv( player_t *p, char *msg);

void  wrt(int fd, char *buf, int bufLen); 

void* playGame(void *args); 


#endif /* PROTOCOL_H */

