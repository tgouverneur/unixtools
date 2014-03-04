/*
 * ifclone-server
 *
 * Run this server where you want to
 * gather packet that are sent by the client.
 *
 * Gouverneur Thomas
 * 2009
 */
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/ioctl.h>
#include <netinet/in.h>
#include <pcap.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <linux/if_tun.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <net/if.h>


#define MAXIFNAME 30
// Define this if your client is a little-endian (MIPS i.e.)
#define REMOTE_LE


int main(int argc, char *argv[])
{

    int sd, rc, n, cliLen;
    struct sockaddr_in cliAddr, servAddr;
    u_char *packet;
    unsigned int len;
    char ifname[MAXIFNAME];

    if (argc != 4 && argc != 3) {
	fprintf(stdout, "[-] Usage: %s <ip address> <port> [if]\n",
		argv[0]);
	return 0;
    }

    if (argc == 4) {
	strncpy(ifname, argv[3], MAXIFNAME);
    } else {
	strncpy(ifname, "dump0", MAXIFNAME);
    }

    int fd = open("/dev/net/tun", O_RDWR | O_SYNC);
    if (fd < 0) {
	fprintf(stderr, "[!] Cannot open /dev/net/tun\n");
	return 42;
    }

    struct ifreq ifr;
    memset(&ifr, 0, sizeof(ifr));
    ifr.ifr_flags = IFF_TAP | IFF_NO_PI;
    strncpy(ifr.ifr_name, ifname, strlen(ifname));
    if (ioctl(fd, TUNSETIFF, (void *) &ifr) < 0) {
	fprintf(stderr, "[!] Unable to setup tap device %s", ifname);
	close(fd);
	return 42;
    }

    sd = socket(AF_INET, SOCK_DGRAM, 0);
    if (sd < 0) {
	printf("[!] Cannot open socket \n");
	exit(1);
    }

    /* bind local server port */
    servAddr.sin_family = AF_INET;
    servAddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servAddr.sin_port = htons(atoi(argv[2]));
    rc = bind(sd, (struct sockaddr *) &servAddr, sizeof(servAddr));
    if (rc < 0) {
	printf("[!] Cannot bind port number %d \n",
	       atoi(argv[2]));
	exit(1);
    }

    printf("[-] Waiting for data on port UDP %u\n",
	   atoi(argv[2]));

    /* server infinite loop */
    while (1) {

	memset(&len, 0x0, sizeof(len));

	/* receive message length */
	cliLen = sizeof(cliAddr);
	n = recvfrom(sd, &len, sizeof(len), 0,
		     (struct sockaddr *) &cliAddr, &cliLen);

	if (n < 0) {
	    printf("[!] Cannot receive data \n");
	    continue;
	}
#ifdef REMOTE_LE
	len = ntohl(len);
#endif

	packet = (u_char *) malloc(len + 1);
	n = recvfrom(sd, packet, len, 0,
		     (struct sockaddr *) &cliAddr, &cliLen);

	if (n < 0) {
	    printf("[!] Cannot receive data \n");
	    continue;
	}

	write(fd, packet, len);

	printf("[-] From %s:%u (%d)\n",
	       inet_ntoa(cliAddr.sin_addr),
	       ntohs(cliAddr.sin_port), len);

	free(packet);

    }
    return 0;
}

