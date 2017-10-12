# Chat-app-multithreading-server-
A chat application using multithreading server coded in C language.


First compilation : 

In main directory, type execute the create-project-files.sh 

(./create-project-files.sh)

For other compilation, you can go in build directory and compile with make

( cd build/jalon02
  make
 )


Start the server : 

In build/jalon02 directory, execute the RE216-JALON02-SERVER file with a port number as arg

( ./RE216-JALON02-SERVER 8080 )


Start the clients : 

In build/jalon02 directory, execute the RE216-JALON02-CLIENT file with an IP address and your server port number as args

( ./RE216-JALON02-SERVER localhost 8080 )


End a client :

Type /quit as a client to finish the connection to the server.
