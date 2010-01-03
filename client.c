/* multicast_client.c
 * This sample demonstrates a Windows multicast client that works with either
 * IPv4 or IPv6, depending on the multicast address given.
 * Requires Windows XP+/Use MSVC and platform SDK to compile.
 * Troubleshoot: Make sure you have the IPv6 stack installed by running
 *     >ipv6 install
 *
 * Usage:
 *     multicast_client multicastip port
 *
 * Examples:
 *     >multicast_client 224.0.22.1 9210
 *     >multicast_client ff15::1 2001
 *
 * Written by tmouse, July 2005
 * http://cboard.cprogramming.com/showthread.php?t=67469
 */
 
#include <stdio.h>      /* for printf() and fprintf() */
#include <stdlib.h>     /* for atoi() and exit() */
#include <string.h>     /* for memset() */
#include <time.h>       /* for timestamps */
#include <sys/socket.h>
#include <netinet/in.h>
#include <netinet/tcp.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <stdlib.h>     /* for atoi() and exit() */



typedef int SOCKET;

void DieWithError(char* errorMessage)
{
    fprintf(stderr, "%s\n", errorMessage);
    exit(EXIT_FAILURE);
}


int main(int argc, char* argv[])
{
    SOCKET     sock;                     /* Socket */
    char*      multicastIP;              /* Arg: IP Multicast Address */
    char*      multicastPort;            /* Arg: Port */
    struct addrinfo*  multicastAddr;            /* Multicast Address */
    struct addrinfo*  localAddr;                /* Local address to bind to */
    struct addrinfo   hints          = { 0 };   /* Hints for name lookup */
    int yes=1;

    if ( argc != 3 )
    {
        fprintf(stderr,"Usage: %s <Multicast IP> <Multicast Port>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    multicastIP   = argv[1];      /* First arg:  Multicast IP address */
    multicastPort = argv[2];      /* Second arg: Multicast port */

    /* Resolve the multicast group address */
    hints.ai_family = PF_UNSPEC;
    hints.ai_flags  = AI_NUMERICHOST;
    if ( getaddrinfo(multicastIP, NULL, &hints, &multicastAddr) != 0 )
    {
        DieWithError("getaddrinfo() failed");
    }

    printf("Using %s\n", multicastAddr->ai_family == PF_INET6 ? "IPv6" : "IPv4");

    /* Get a local address with the same family (IPv4 or IPv6) as our multicast group */
    hints.ai_family   = multicastAddr->ai_family;
    hints.ai_socktype = SOCK_DGRAM;
    hints.ai_flags    = AI_PASSIVE; /* Return an address we can bind to */
    if ( getaddrinfo(NULL, multicastPort, &hints, &localAddr) != 0 )
    {
        DieWithError("getaddrinfo() failed");
    }

    /* Create socket for receiving datagrams */
    if ( (sock = socket(localAddr->ai_family, localAddr->ai_socktype, 0)) < 0 )
    {
        DieWithError("socket() failed");
    }




    // lose the pesky "Address already in use" error message
    if (setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&yes,sizeof(int)) == -1) 
      {
	DieWithError("setsockopt");
      } 


    /* Bind to the multicast port */
    if ( bind(sock, localAddr->ai_addr, localAddr->ai_addrlen) != 0 )
    {
        DieWithError("bind() failed");
    }
    
    
    /* Join the multicast group. We do this seperately depending on whether we
     * are using IPv4 or IPv6. WSAJoinLeaf is supposed to be IP version agnostic
     * but it looks more complex than just duplicating the required code. */
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
        {
            DieWithError("setsockopt() failed");
        }
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
        {
            DieWithError("setsockopt() failed");
        }
    }
    else
    {
        DieWithError("Neither IPv4 or IPv6");
    }

    freeaddrinfo(localAddr);
    freeaddrinfo(multicastAddr);

    for (;;) /* Run forever */
    {
        time_t timer;
        char   recvString[120000];      /* Buffer for received string */
        int    recvStringLen;        /* Length of received string */

        /* Receive a single datagram from the server */
        if ( (recvStringLen = recvfrom(sock, recvString, sizeof(recvString) - 1, 0, NULL, 0)) < 0 )
        {
            DieWithError("recvfrom() failed");
        }

        recvString[recvStringLen] = '\0';
        
        int* p_nr = (int*)recvString;

        /* Print the received string */
        time(&timer);  /* get time stamp to print with recieved data */
        printf("Time Received: %.*s : packet %d, %d bytes\n",
                strlen(ctime(&timer)) - 1, ctime(&timer), *p_nr, recvStringLen);
    }

    /* NOT REACHED */
    closesocket(sock);
    exit(EXIT_SUCCESS);
}

