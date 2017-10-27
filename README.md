# Chat-app-multithreading-server-
A chat application using multithreading server coded in C language.

 
# First compilation : 
 
In main directory, execute the create-project-files.sh 
(./create-project-files.sh)

For other compilation, you can go in build directory and compile with make
 ( cd build/jalon04
   make
 )


# Start the server : 
 
 In build/jalon04 directory, execute the RE216-JALON04-SERVER file with a port number as arg
 ( ./RE216-JALON02-SERVER 8080 )
 
 
 # Start the clients : 
 
 In build/jalon04 directory, execute the RE216-JALON04-CLIENT file with an IP address and your server port number as args
 ( ./RE216-JALON02-SERVER localhost 8080 )
 
 
 
 # Broadcast
 
 type /msgall message to send a message to all users
 
 # Unicast
 
 type /username message to send a message to the user username
 
 # Multicast
 
 type /create channel to create a new channel named channel
 type /join channel to join a channel
 type /leave channel to leave a channel
 type /destroy channel to destroy a channel
 
 # End a client :
 
 Type /quit as a client to finish the connection to the server.
