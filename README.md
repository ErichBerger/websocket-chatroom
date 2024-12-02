# ASSIGNMENT 4

## ICS 460

## Erich Berger

## StarID: iu8988mn

## Building
- In the terminal, navigate into the folder assignment_4
- Enter `make` in the terminal and press ENTER
- Two programs should now be visible, chatclient and chatserver

## Running

### Server
- From one terminal, navigate into the folder assignment_4
- Enter `./chatserver <port>` in the terminal and press ENTER
    - note: my port number is 56004
- Keep this running during the next steps

### Client
- In another terminal, navigate into the folder assignment_4
- Enter `./chatclient localhost <port> <username>
    - 56004 for matching port, find usernames in Other section
- Test out DM and PM by repeating the above step in other terminals
- When done in each client, enter `EX` to exit
- The server needs to be exited by `CTRL-C`

## Misc
- Function descriptions are in the headers for each client and server
- Available usernames and passwords are:
    - alpha - password1
    - beta - password2
    - gamma - password3
- Additional users can be created
- One thing to note is that I added a second of delay when sending messages from the server. This solved (maybe not well) the issue of data coming in over the TCP connection as a stream, rather than a packet.
- Additional work to fix this would be to implement a persistent buffer to read into, but for the purposes of this project it seems to work without issue
- Implementing mutexes on the user list proved to cause errors, which I didn't have time to diagnose. This may affect performance if testing the program with a script/rapid commands, but things should be fairly atomic given the low amount of users.

## Files

### Server Related
- chatserver.c : contains main function
- serverheaders.h : defines functions and other important constants
- serverclientqueue.c : code for the linked list FIFO queue
- serverhandlers.c : the bulk of the logic for interacting with clients
- serverhelpers.c : assorted functions used throughout (send_message, receive_message, etc.)
- serverinit.c : initializes a socket and starts to listen
- serveruserlist.c : linked list that keeps track of logged in users

### Client Related
- chatclient.c : contains main function
- clientheaders.h : defines functions
- clienthelpers.c : various assorted functions 
- clientinit.c : initializing the connection to the server
- clientmessagelist.c : linked list for registering incoming messages

### Misc Files
- Makefile : used by make
- users.txt : contains persistent login info 


## References
- [Beej's Guide to Network Programming](https://www.beej.us/guide/bgnet/html/split/index.html): Using Internet Sockets. Brian “Beej Jorgensen” Hall, v3.1.12, Copyright © July 17, 2024
- Jacob Sorber -  [How to write a multithreaded webserver using condition variables (Part 3)](https://www.youtube.com/watch?v=P6Z5K8zmEmc&list=PL9IEJIKnBJjH_zM5LnovnoaKlXML5qh17&index=7)