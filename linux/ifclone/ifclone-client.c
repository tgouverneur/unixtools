/*
 * ifclone-client
 *
 * Run this client on the server
 * or device where you want to
 * clone an interface remotely
 *
 * Gouverneur Thomas
 * 2009
 */

/*
 * Compile it with:
 *
 * gcc -o ifclone-client ifclone-client.c -lpcap
 */
#include <pcap.h>
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netinet/if_ether.h>
#include <string.h>
#include <netdb.h>
#include <unistd.h>
#include <arpa/inet.h>

int remote_sock = -1;
struct sockaddr_in addr;

void p_recv(u_char * useless, const struct pcap_pkthdr *pkthdr,
	    const u_char * packet)
{
    if (!packet)
	return;

    fprintf(stdout, "[<-] len=%d\n", pkthdr->len);
    fflush(stdout);
    unsigned int len = pkthdr->len;
    int n =
	sendto(remote_sock, &len, len, 0, (struct sockaddr *) &addr,
	       sizeof(addr));
    if (n < 0) {
	fprintf(stderr, "[!] Cannot send packet over!\n");
	shutdown(remote_sock, SHUT_RDWR); // Greetz to my LOVE.
	close(remote_sock);
	exit(1);
    }
    n = sendto(remote_sock, packet, pkthdr->len, 0,
	       (struct sockaddr *) &addr, sizeof(addr));
    if (n < 0) {
	fprintf(stderr, "[!] Cannot send packet over!\n");
	shutdown(remote_sock, SHUT_RDWR);
	close(remote_sock);
	exit(1);
    }
    fprintf(stdout, "[->] Sent!\n");
    fflush(stdout);

    return;
}

int main(int argc, char **argv)
{
    int i;
    char errbuf[PCAP_ERRBUF_SIZE];
    pcap_t *descr;
    const u_char *packet;
    struct pcap_pkthdr hdr;
    struct ether_header *eptr;

    if (argc != 4) {
	fprintf(stdout, "Usage: %s <if> <ip address> <port>\n", argv[0]);
	return 0;
    }

    /* open device for reading */
    descr = pcap_open_live(argv[1], BUFSIZ, 0, -1, errbuf);
    if (descr == NULL) {
	printf("pcap_open_live(): %s\n", errbuf);
	exit(1);
    }

    /* prepare the DGRAM socket */
    remote_sock = socket(AF_INET, SOCK_DGRAM, 0);
    if (remote_sock < 0) {
	fprintf(stderr, "Error, socket() failed: %s\n", strerror(errno));
	return 1;
    }

    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    inet_pton(AF_INET, argv[2], &(addr.sin_addr.s_addr));
    addr.sin_port = htons(atoi(argv[3]));

    pcap_loop(descr, 0, p_recv, NULL);

    fprintf(stdout, "\nDone processing packets... wheew!\n");
    shutdown(remote_sock, SHUT_RDWR);
    close(remote_sock);

    return 0;
}
