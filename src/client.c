/* client.c
 * This sample demonstrates a multicast client that works with either
 * IPv4 or IPv6, depending on the multicast address given.

 * Troubleshoot Windows: Make sure you have the IPv6 stack installed by running
 *     >ipv6 install
 *
 * Usage:
 *     client <Multicast IP> <Multicast Port> <Receive Buffer Size>
 *
 * Examples:
 *     >client 224.0.22.1 9210 70000
 *     >client ff15::1 2001 10000
 *
 * Written by tmouse, July 2005
 * http://cboard.cprogramming.com/showthread.php?t=67469
 * 
 * Modified to run multi-platform by Christian Beier <dontmind@freeshell.org>.
 */



#ifdef UNIX
#include <unistd.h>
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "msock.h" 

#define MULTICAST_SO_RCVBUF 300000

SOCKET     sock;                     /* Socket */
char*      recvBuf;                  /* Buffer for received data */


static void DieWithError(char* errorMessage)
{
  fprintf(stderr, "%s\n", errorMessage);
  if(sock >= 0)
    close(sock);
  if(recvBuf)
    free(recvBuf);

  exit(EXIT_FAILURE);
}


int main(int argc, char* argv[])
{
  char*      multicastIP;              /* Arg: IP Multicast Address */
  char*      multicastPort;            /* Arg: Port */
  int        recvBufLen;               /* Length of receive buffer */


  if ( argc != 4 )
    {
      fprintf(stderr,"Usage: %s <Multicast IP> <Multicast Port> <Receive Buffer Size>\n", argv[0]);
      exit(EXIT_FAILURE);
    }

#ifdef WIN32
  WSADATA trash;
  if(WSAStartup(MAKEWORD(2,0),&trash)!=0)
    DieWithError("Couldn't init Windows Sockets\n");
#endif

  multicastIP   = argv[1];      /* First arg:  Multicast IP address */
  multicastPort = argv[2];      /* Second arg: Multicast port */
  recvBufLen    = atoi(argv[3]);
 
  recvBuf = (char*)malloc(recvBufLen*sizeof(char));


  sock = mcast_recv_socket(multicastIP, multicastPort, MULTICAST_SO_RCVBUF);
  if(sock < 0)
      DieWithError("mcast_recv_socket() failed");

  int rcvd=0;
  int lost=0;
    
  int last_p=-1;
  for (;;) /* Run forever */
    {
      time_t timer;
      int bytes = 0;

      /* Receive a single datagram from the server */
      if ((bytes = recvfrom(sock, recvBuf, recvBufLen, 0, NULL, 0)) < 0)
	DieWithError("recvfrom() failed");
        
      ++rcvd;
      int this_p = ntohl(*(int*)recvBuf);

      if(last_p >= 0) /* only check on the second and later runs */
	{
	  if(this_p - last_p > 1)
	    lost += this_p - (last_p+1);
	}
      last_p = this_p;
        
      /* Print the received string */
      time(&timer);  /* get time stamp to print with recieved data */
      printf("Packets recvd %d, lost %d, loss ratio %f\n", rcvd, lost, (double)lost/(double)(rcvd+lost));
      printf("Time Received: %.*s : packet %d, %d bytes\n",
	     (int)strlen(ctime(&timer)) - 1, ctime(&timer), this_p, bytes);
    }

  /* NOT REACHED */
  exit(EXIT_SUCCESS);
}

