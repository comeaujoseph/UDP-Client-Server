------------------------------------------------
Author:       Joseph Comeau
RedID:        815732093
Assignment:   Programming Assignment 2
Date:         April 4, 2016

Languages:    C++
Platforms:    Mac OS X, Ubuntu
------------------------------------------------

SUMMARY:
  The goal is to create UDP connectionless client-server pair. The client will send a UDP message to the 
  server, e.g. “Hello”. The server will start and run on port number 5432 and return the original message 
  concatenated with a humorous message as an ASCII string to the client on port 5433. The client will 
  then present a formatted string to the monitor.

  What is a connectionless protocol?
    The device at one end of the communication transmits data addressed to the other, without 
    first ensuring that the recipient is available and ready to receive the data. Internet Protocol (IP) 
    and User Datagram Protocol (UDP) are connectionless protocols.

  The steps of establishing a UDP socket communication on the server side are as follows:
    1. Create a socket with the socket() function;
    2. Bind the socket to an address using the bind() function;
    3. Send and receive data by means of recvfrom() and sendto()

  The steps of establishing a UDP socket communication on the client side are as follows:
    1. Create a socket using the socket() function;
    2. Send and receive data by means of the recvfrom() and sendto() functions.

  Note for this programming assignment, we also need to bind the client's socket to a specific
  address (see issue #3 at bottom of README).

MANUAL:
  In addition to the source code (client.c and server.c), a Makefile is provided which will 
  generate two executables:
    (1) server
    (2) client
  Note: It is preferrable to run the server and client scripts on two different machines.
  Steps:
    1.  In the same directory as the source code/Makfile run...
        make
      It should compile without any warnings/errors

    2.  Now you can start up the server script... (PORT USED IS 5433)
        ./server

    3.  Now start up the client script using the default settings, i.e. server IP address 192.168.1.217 and port 5432...
        ./client
      You can specify a certain ip address by doing the following...
        ./client -s IP_ADD
      By running the following command you can get a help menu that display optional flags you can use...
        ./client -h

      Note: The IP address must be the IP address of the machine that the server script is running on. Also, the 
      client will always attempt to communicate via the 5433 port. The default ip address is based on 
      the test environment I setup to ensure that the scripts were working correctly.

    4.  Once the client has been successfully setup, the user will be prompted to type a message that will
      be sent to the server

    5.  After entering a message, the server will recieve and append a funny saying to the message. It will 
      then send back the message with the appended saying and the client will then print to the console

ISSUES ENCOUNTERED:
  (1) You should not be passing the address of the socket structure length to sendto() - it requires 
      the actual length (i.e., "addr_len", not "&addr_len").

  (2) The reason you pass the address of the length to recvfrom() is that it is changed by that function 
      if the real address happens to be shorter.

  (3) No, you don't need to bind a socket before using sendto. If the socket is not already bound, sendto 
      will perform an implicit bind (for TCP sockets this implicit bind is done on connect).

