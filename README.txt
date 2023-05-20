Tic-Tac-Toe Online| Rahul Badiani & Suchit Kalluru 
Purpose: To create a on-line Tic-Tac-Toe game that allows players to join the server and be automatically matched to play a game


protocol.c - Handles the server - client interaction part of the game 
ttts.c: Server of the game

Currently only the servers side is provided. 

ttts.c is the server side for the tic tac toe game, it handles the logic and comunicating with the client. Takes in one argument which is an open port numbe to listen on. 

Uses sockets.io to communicate with the client and transmits data through TCP. When starting a game the client must provide a unique name that is not in use for any other game. The client will then be prompted to wait until another player has connected. The server allows for multiple concurent games using multithreading. The possible commands for the client to send after the game has started is MOVE, DRAW, or RSGN. 

The Protocol for communcation: each command starts with a 4 letter word (PLAY, MOVE, DRAW), then the number of remaining bytes that follows and the required amount of fields. Eaach field is seperated by '|'
examples: 
      PLAY|4|Bob|             //sent to server indicating start game and the clients name is Bob
      MOVE|6|X|2,2|           //sent to server indicating move X to 2,2
      MOVD|16|X|2,2|....X....|//sent to client indicating a move of X to 2,2
      DRAW|2|S|               //sent to server suggesting draw to oppponent
      RSGN|0|                 //sent to server indicating the client has resgined
      ect...
    
Usage: the following commands will compile and link the server with its dependinces and then run it. The server will contninously run until the proccess is killed or stopped. 

$ make 
$ ./ttts 15000

 
* Functionality Testing & Error Testing is described in the file "testPlan.txt" *
