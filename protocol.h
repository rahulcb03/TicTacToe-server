#ifndef PROTOCOL_H
#define PROTOCOL_H

#define BUFSIZE 200
// Define struct player
typedef struct player {
    int sck; 
    char name[50];
    char buffer[BUFSIZE]; 
    int len; 
}player_t;

// Declare method p_recv that uses struct player
int p_recv( player_t *p, char *msg);

#endif /* PROTOCOL_H */
