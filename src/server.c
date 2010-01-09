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



#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#define SOCKET int

#ifdef __MINGW32__ 
#undef SOCKET
#undef socklen_t 
#define WINVER 0x0501 
#include <ws2tcpip.h> 
#define EWOULDBLOCK WSAEWOULDBLOCK
#define close closesocket
#define socklen_t int
typedef unsigned int in_addr_t;
#else
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/un.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <netdb.h>
#endif





SOCKET    sock;                   /* Socket */
struct addrinfo* multicastAddr;   /* Multicast address */


static void DieWithError(char* errorMessage)
{
  freeaddrinfo(multicastAddr);
  if(sock >= 0)
    close(sock);
  fprintf(stderr, "%s\n", errorMessage);
  exit(EXIT_FAILURE);
}


int main(int argc, char *argv[])
{
  char*     multicastIP;            /* Arg: IP Multicast address */
  char*     multicastPort;          /* Arg: Server port */
  char*     sendString;             /* Arg: String to multicast */
  size_t    sendStringLen;          /* Length of string to multicast */
  int       multicastTTL;           /* Arg: TTL of multicast packets */
  int       defer_ms;               /* miliseconds to defer in between sending */
  struct addrinfo hints = { 0 };    /* Hints for name lookup */
  int i;
  
  if ( argc < 5 || argc > 6 )
    {
      fprintf(stderr, "Usage: %s <Multicast Address> <Port> <packetsize> <defer_ms> [<TTL>]\n", argv[0]);
      exit(EXIT_FAILURE);
    }

#ifdef WIN32
  WSADATA trash;
  if(WSAStartup(MAKEWORD(2,0),&trash)!=0)
    DieWithError("Couldn't init Windows Sockets\n");
#endif

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


  /* Resolve destination address for multicast datagrams */
  hints.ai_family   = PF_UNSPEC;
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_flags    = AI_NUMERICHOST;
  int status;
  if ((status = getaddrinfo(multicastIP, multicastPort, &hints, &multicastAddr)) != 0 )
    {
      fprintf(stderr, "getaddrinfo: %s\n", gai_strerror(status));
      DieWithError("getaddrinfo() failed");
    }



  printf("Using %s\n", multicastAddr->ai_family == PF_INET6 ? "IPv6" : "IPv4");

  /* Create socket for sending multicast datagrams */
  if ( (sock = socket(multicastAddr->ai_family, multicastAddr->ai_socktype, 0)) < 0 )
    DieWithError("socket() failed");
    

  /* Set TTL of multicast packet */
  if ( setsockopt(sock,
		  multicastAddr->ai_family == PF_INET6 ? IPPROTO_IPV6        : IPPROTO_IP,
		  multicastAddr->ai_family == PF_INET6 ? IPV6_MULTICAST_HOPS : IP_MULTICAST_TTL,
		  (char*) &multicastTTL, sizeof(multicastTTL)) != 0 )
    DieWithError("setsockopt() failed");
    
    
  /* set the sending interface */
  /* FIXME does it have to be a ipv6 iface in case we're doing ipv6? */
  in_addr_t iface = INADDR_ANY;
    
  if(setsockopt (sock, 
		 multicastAddr->ai_family == PF_INET6 ? IPPROTO_IPV6 : IPPROTO_IP,
		 multicastAddr->ai_family == PF_INET6 ? IPV6_MULTICAST_IF : IP_MULTICAST_IF,
		 (char*)&iface, sizeof(iface)) != 0)  
    DieWithError("interface setsockopt()");


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
#ifndef __MINGW32__ 
      usleep(defer_ms*1000); 
#else 
      Sleep (defer_ms);
#endif 

      
    }

  /* NOT REACHED */
  return 0;
}

