/* server.c
 * This sample demonstrates a multicast server that works with either
 * IPv4 or IPv6, depending on the multicast address given.
 *
 * Troubleshoot Windows: Make sure you have the IPv6 stack installed by running
 *     >ipv6 install
 *
 * Usage:
 *     server <Multicast Address> <Port> <packetsize> <defer_ms> [<TTL>>]
 *
 * Examples:
 *     >server 224.0.22.1 9210 6000 1000
 *     >server ff15::1 2001 65000 1
 *
 * Written by tmouse, July 2005
 * http://cboard.cprogramming.com/showthread.php?t=67469
 *
 * Modified to run multi-platform by Christian Beier <dontmind@freeshell.org>.
 */




#include <stdio.h>
#include <stdlib.h>
#ifdef UNIX
#include <unistd.h> /* for usleep() */
#endif
#include "msock.h"



static void DieWithError(char* errorMessage)
{
  fprintf(stderr, "%s\n", errorMessage);
  exit(EXIT_FAILURE);
}


int main(int argc, char *argv[])
{
  SOCKET sock;
  struct addrinfo *multicastAddr;
  char*     multicastIP;            /* Arg: IP Multicast address */
  char*     multicastPort;          /* Arg: Server port */
  char*     sendString;             /* Arg: String to multicast */
  int       sendStringLen;          /* Length of string to multicast */
  int       multicastTTL;           /* Arg: TTL of multicast packets */
  int       defer_ms;               /* miliseconds to defer in between sending */

  int i;
  
  if ( argc < 5 || argc > 6 )
    {
      fprintf(stderr, "Usage: %s <Multicast Address> <Port> <packetsize> <defer_ms> [<TTL>]\n", argv[0]);
      exit(EXIT_FAILURE);
    }


  multicastIP   = argv[1];             /* First arg:   multicast IP address */
  multicastPort = argv[2];             /* Second arg:  multicast port */
  sendStringLen = atoi(argv[3]);   
  defer_ms = atoi(argv[4]);
   
  /* just fill this with some byte */
  sendString = calloc(sendStringLen, sizeof(char));
  for(i = 0; i<sendStringLen; ++i)
    sendString[i]= 's';
     	
    	

  multicastTTL  = (argc == 6 ?         /* Fourth arg:  If supplied, use command-line */
		   atoi(argv[5]) : 1); /* specified TTL, else use default TTL of 1 */



  sock = mcast_send_socket(multicastIP, multicastPort, multicastTTL, &multicastAddr);
  if(sock == -1 )
      DieWithError("mcast_send_socket() failed");
  

  int nr=0;
  for (;;) /* Run forever */
    {
      int*  p_nr = (int*)sendString;
      *p_nr = htonl(nr);
 
      if ( sendto(sock, sendString, sendStringLen, 0,
		  multicastAddr->ai_addr, multicastAddr->ai_addrlen) != sendStringLen )
	DieWithError("sendto() sent a different number of bytes than expected");
        
      fprintf(stderr, "packet %d sent\n", nr);
      nr++;
#ifdef UNIX
      usleep(defer_ms*1000); 
#else 
      Sleep (defer_ms);
#endif 

      
    }

  /* NOT REACHED */
  return 0;
}

