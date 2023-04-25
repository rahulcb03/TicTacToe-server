Project III: Tic-Tac-Toe | Rahul Badiani rb1217 & Suchit Kalluru srk170
Purpose: To create a on-line Tic-Tac-Toe game that allows players to join the server and be automatically matched to play a game


Two files being submitted: 
ttts.c - This is the code for the server of the game; it will control the game itself, communication with the client, and communication with players.


ttt.c - This is the client; it connects to the server, handles the reading from player actions from STDIN, and writes to the players with STDOUT. 


Implementation: 
ttts.c: Server of the game


1) Since this is the program that takes care of the server, the program first checks to make sure that the parameters that the user inputs is properly 
   formatted: "./ttts [port#]". 
   
2) Then the program will open a listener to the socket through the method 'openListener', which takes in the parameters [port#] and then the size of the 
   Queue required for the connection. The method 'openListener' will then create a socket in the domain and then bind the socket to the requested port 
   number. Then it will return the opened socket back to the main function, which will be saved in a variable for usage later. 


3) After the socket connection is created, the program then connects the socket to the client trying to connect to that specific port number and will 
   return an error message to the user if the port number is inactive. 


4) The program then checks if the player types the keyword 'PLAY', in which case it will start to collect and store the player's name. The program will 
   also check if the name is used, and if it is will request the user to change their name. After it will also connect any other player that is trying 
   to connect to that specific port number and request their name as well (after making sure that the user types the keyword 'PLAY'); the name checking 
   will also occur for the second player.
 
5) After both players are connected to the socket and the port, they will be matched together for a game of tic-tac-toe through the method 'playGame', 
   which takes in two sockets as its parameters, the sockets for both players. This playGame is within the protocol.c part of the project. In this 
   process, the program also uses multithreading for multiple games to be able to happen at the same time. 


protocol.c: Server - Client Interactions of the game


1) The 'protocol.c' method is used for user-input execution. This would be the following commands: "MOVE", "DRAW", and "RSGN" as well as the other 
   game-related parts like win checking, game-level error checking, etc. 


2) In playGame an empty board is created, and the game itself takes place. The function will read the user's input, either from socket 1 or socket 2, 
   depending on whose turn it is, and then read the command that the user types in. The playGame function then calls the "p_recv" function, which 
   creates a node associated with each player and then stores values specific to that player. For instance, "p_recv" will add the user's name into the 
   name field and then keep track of their responses in the buffer etc. Then back in the playGame function, the program correspondingly responds to the 
   command: 'MOVE', 'DRAW' or 'RSGN'.


3) The "MOVE" command checks if the current move is valid, meaning that the space isn't occupied already and if the space is within the board. Then if 
   there is a violation, the program will report to the user, and if there isn't any, the program will add the move to the game board and then report 
   the current state of the game board back to both players.  


4) After the game board is updated, the program will call the 'checkWin' method, which will check the game board for any win(horizontal, vertical, 
   diagonally) or if there was a draw made. A win is dictated by a return of the number 1, a draw by the number 2, and a continued game by the number 
   0. Depending on the number, the program will report either a draw or a win to both users with a message proclaiming the correct user as the winner 
   of the game. 


5) The game also accounts for a "DRAW" command, which contains the sequence of inputs required for a Draw in the game, and the various other possible 
   occurrences when a draw is requested. The program first requests the second player of their choice regarding the draw; either Accepting (A) or 
   Rejecting (R). Then the program will check if the messages are correctly written and then proceed with the correct responses for both. If the user 
   accepts the draw, the program will send messages to both players about the draw and terminate the game; if the draw is rejected, it will go back to 
   the initial user who requested the draw to play their move. 


5) The program also checks for the Resignations("RSGN") command, makes sure that it is formatted correctly, and then outputs the loss message and 
   the winning message to the corresponding players, and then terminates. 
 
 
* Functionality Testing & Error Testing is described in the file "testPlan.txt" *
