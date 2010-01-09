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



#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
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
 

SOCKET     sock;                     /* Socket */
char*      recvBuf;                  /* Buffer for received data */


void DieWithError(char* errorMessage)
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
  struct addrinfo*  multicastAddr;     /* Multicast Address */
  struct addrinfo*  localAddr;         /* Local address to bind to */
  struct addrinfo   hints  = { 0 };    /* Hints for name lookup */
  int yes=1;

  if ( argc != 4 )
    {
      fprintf(stderr,"Usage: %s <Multicast IP> <Multicast Port> <Receive Buffer Size>\n", argv[0]);
      exit(EXIT_FAILURE);
    }

  multicastIP   = argv[1];      /* First arg:  Multicast IP address */
  multicastPort = argv[2];      /* Second arg: Multicast port */
  recvBufLen    = atoi(argv[3]);
 
  recvBuf = malloc(recvBufLen*sizeof(char));


  /* Resolve the multicast group address */
  hints.ai_family = PF_UNSPEC;
  hints.ai_flags  = AI_NUMERICHOST;
  if (getaddrinfo(multicastIP, NULL, &hints, &multicastAddr) != 0)
    DieWithError("getaddrinfo() failed");
    
  printf("Using %s\n", multicastAddr->ai_family == PF_INET6 ? "IPv6" : "IPv4");

  /* Get a local address with the same family (IPv4 or IPv6) as our multicast group */
  hints.ai_family   = multicastAddr->ai_family;
  hints.ai_socktype = SOCK_DGRAM;
  hints.ai_flags    = AI_PASSIVE; /* Return an address we can bind to */
  if ( getaddrinfo(NULL, multicastPort, &hints, &localAddr) != 0 )
    DieWithError("getaddrinfo() failed");
  

  /* Create socket for receiving datagrams */
  if ( (sock = socket(localAddr->ai_family, localAddr->ai_socktype, 0)) < 0 )
    DieWithError("socket() failed");
    
  /* lose the pesky "Address already in use" error message */
  if (setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,(char*)&yes,sizeof(int)) == -1) 
    DieWithError("setsockopt");
  
  /* Bind to the multicast port */
  if ( bind(sock, localAddr->ai_addr, localAddr->ai_addrlen) != 0 )
    DieWithError("bind() failed");
  
    
    
  /* Join the multicast group. We do this seperately depending on whether we
   * are using IPv4 or IPv6. 
   */
  if ( multicastAddr->ai_family  == PF_INET &&  
       multicastAddr->ai_addrlen == sizeof(struct sockaddr_in) ) /* IPv4 */
    {
      struct ip_mreq multicastRequest;  /* Multicast address join structure */

      /* Specify the multicast group */
      memcpy(&multicastRequest.imr_multiaddr,
	     &((struct sockaddr_in*)(multicastAddr->ai_addr))->sin_addr,
	     sizeof(multicastRequest.imr_multiaddr));

      /* Accept multicast from any interface */
      multicastRequest.imr_interface.s_addr = htonl(INADDR_ANY);

      /* Join the multicast address */
      if ( setsockopt(sock, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char*) &multicastRequest, sizeof(multicastRequest)) != 0 )
	DieWithError("setsockopt() failed");
    }
  else if ( multicastAddr->ai_family  == PF_INET6 &&
	    multicastAddr->ai_addrlen == sizeof(struct sockaddr_in6) ) /* IPv6 */
    {
      struct ipv6_mreq multicastRequest;  /* Multicast address join structure */

      /* Specify the multicast group */
      memcpy(&multicastRequest.ipv6mr_multiaddr,
	     &((struct sockaddr_in6*)(multicastAddr->ai_addr))->sin6_addr,
	     sizeof(multicastRequest.ipv6mr_multiaddr));

      /* Accept multicast from any interface */
      multicastRequest.ipv6mr_interface = 0;

      /* Join the multicast address */
      if ( setsockopt(sock, IPPROTO_IPV6, IPV6_ADD_MEMBERSHIP, (char*) &multicastRequest, sizeof(multicastRequest)) != 0 )
	DieWithError("setsockopt() failed");
    }
  else
    DieWithError("Neither IPv4 or IPv6");
  

  freeaddrinfo(localAddr);
  freeaddrinfo(multicastAddr);

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

      if(last_p < 0) /* first run */
	last_p = this_p;
      else           /* second and further runs */
	{
	  if(this_p - last_p > 1)
	    lost += this_p - (last_p+1);
	  last_p = this_p;
	}

        
      /* Print the received string */
      time(&timer);  /* get time stamp to print with recieved data */
      printf("Packets recvd %d, lost %d, loss ratio %f\n", rcvd, lost, (double)lost/(double)(rcvd+lost));
      printf("Time Received: %.*s : packet %d, %d bytes\n",
	     strlen(ctime(&timer)) - 1, ctime(&timer), this_p, bytes);
    }

  /* NOT REACHED */
  exit(EXIT_SUCCESS);
}

