Project III: Tic-Tac-Toe | Rahul Badiani rb1217 & Suchit Kalluru srk170
Purpose: To create a on-line Tic-Tac-Toe game that allows players to join the server and be automatically matched to play a game

Two files being submitted: 
ttts.c - This is the code for the server of the game, it will control the game itself, communication with the client, and commuication with players.

ttt.c - This is the client; it connects to the server, handles the reading from player actions from STDIN, and writing to the players with STDOUT. 


Implementation: 
ttts.c: Server of the game

1) Since this is the program that takes care of the server, the program first checks to make sure that the paramters that the user inputs is properly formatted:
   "./ttts [port#]". 
   
2) Then the program will open a listener to the socket through the method 'openListener' which takes in the parameters [port#] and then the size of the Queue required for
   the connectoin. The method 'openListener' then will create a socket in the domain and then bind the socket to the requested port number. Then it will return the opened 
   socket back to the main function which will be saved in a variable for useage later. 

3) After the socket connection is created, the program then connects the socket to the client trying to connect to that specifc port number. 

4) The program then checks if the player types the key word 'PLAY', in which case it will start to collect and store the players name. After it will also connect any other   
   player that is trying to connect to that specific port number, and request their name as well (after making sure that the user types the keyword 'PLAY').
 
5) After both players are connected to the socket, and the port, they will be matched togheter for a game of tic-tac-toe through the method 'playGame' which takes in two
   sockets as its parameters, the sockets for both players. This playGame is within the protocol.c part of the project.

6) After the necessary move has been made/the 'DRAW' or 'RSGN' sequences have been played out, the program will then check the game board through the 'checkWin' method. The
   'checkWin' method will check the game board for either a win, full board(in which case its a draw), or if the game needs to continue. Depending on the value retuerned from
   this method the program will then take appropriate action, either ending the game in the case of a win/full board, or continuing the game if its there are moves to be played.
   
protocol.c: Server - Client Interactions of the game

1) The 'protocol.c' method is used for user-input execution. This would be the following commands: "MOVE", "DRAW", and "RSGN" as well as the other game related parts like win checking,
   game level error checking etc. 

2) In playGame an empty board is created and the game itself takes place. The function will read the users input, 
   either from socket 1 or socket 2 depending on whos turn it is, and then read the command that the user types in. It then responds to the corresponding command: 'MOVE'
   'DRAW' or 'RSGN'. Each command, 'MOVE', 'DRAW', or 'RSGN', is handeld by the 'protocol.c' file which will take the appropriate action given the input recieved. 

3) The "MOVE" command checks if the current move is valid, meaning that the space isn't occupied already, and if the space is within the board. Then if there is a violation
   the program will report to the user. 
   
4) The "DRAW" command contains the sequence of inputs required for a Draw in the game, and the various other possible occurances when a draw is requested. The program first 
   requests the second player of thier choice regarding the draw; either Accepting (A) or Rejecting (R). Then the program will check if the messages are correctly written,
   and then proceed with the correct responses for both. If the user accepts the draw the program will send messages to both players about the draw and terminate the game, 
   if the draw is rejected it will go back to the inital user who requested the draw to play thier move. 

5) The "RSGN" command outputs the loss message and the win message to the corresponding players, and then terminates. 
 
   
* Functionality Testing & Error Testing is described in the file "testPlan.txt" *
