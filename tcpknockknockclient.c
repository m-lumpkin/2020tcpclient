//Morgan Lumpkin & Miqaa'iyl Fahiym

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include "Practical.h"

#define MAX_LEN 1024

int main(int argc, char **argv)
{
	int sock;
	char* server;
	char* servPort;

	if (argc != 3) {
		fprintf(stderr, "Usage: %s server port/service\n", argv[0]);
		exit(0);
	}

	server = argv[1];
	servPort = argv[2];

#if OLD_STYLE_SOCKET_CODE
	int port_num = atoi(servPort);

	if (port_num <= 0) {
		struct servent *p_servent;
		p_servent = getservbyname(servPort, "tcp");
		if (p_servent == NULL) {
			DieWithSystemMessage("getservbyname() failed");
		}
		port_num = p_servent->s_port;
	}

	struct hostent *hp;
	hp = gethostbyname(server);
	if (NULL == hp) {
		DieWithSystemMessage("gethostbyname() failed");
	}

	struct sockaddr_in servAddr;
	memset(&servAddr, 0, sizeof(servAddr));
	servAddr.sin_family = AF_INET;
	memcpy(&servAddr.sin_addr, hp->h_addr, hp->h_length);
	servAddr.sin_port = htons(port_num);

	sock = socket(servAddr.sin_family, SOCK_STREAM, 0);
	if (sock < 0) {
		DieWithSystemMessage("socket() failed");
	}

	struct sockaddr *p_servAddr = (struct sockaddr *) &servAddr;
	socklen_t servAddrLen = sizeof(servAddr);
#else
	struct addrinfo addrCriteria;
	memset(&addrCriteria, 0, sizeof(addrCriteria));
	addrCriteria.ai_family = AF_UNSPEC;
	addrCriteria.ai_socktype = SOCK_STREAM;
	addrCriteria.ai_protocol = IPPROTO_TCP;

	struct addrinfo *servAddrInfo;
	int rtnVal = getaddrinfo(server, servPort, &addrCriteria, &servAddrInfo);
	if (rtnVal != 0) {
		DieWithUserMessage("getaddrinfo() failed", gai_strerror(rtnVal));
	}

	sock = socket(servAddrInfo->ai_family, servAddrInfo->ai_socktype,
		servAddrInfo->ai_protocol);
	if (sock < 0) {
		DieWithSystemMessage("socket() failed");
	}

	struct sockaddr *p_servAddrInfo->ai_addr;
	socklen_t servAddrLen = servAddrInfo->ai_addrlen;
#endif
	if (connect(sock, p_servAddr, servAddrLen) < 0) {
		DieWithSystemMessage("connect() failed");
	}
	char buf[BUFSIZE];
	const char* joke[3] = {"Knock knock!", "Robin.", "Robin you! Hand over the cash."};
	int n = 0;
	int netlen=0;
	int msglen;
	ssize_t numBytes;
	char *pMsgLen=(char *)&msglen;
	int j=0;
	while (n<3) {
		memset(buf, 0, BUFSIZE);
		strcpy(buf, joke[n]);

		SendMsg(sock, buf, strlen(buf));

		printf("%s\n", joke[n]);

		msglen = RecvMsg(sock, buf, BUFSIZE);
		if(msglen > BUFSIZE) DieWithUserMessage("Receive message",
		 		"message too long to fit in the buffer");

		fputs("Server: ", stdout);
		fflush(stdout);
		write(1, buf, msglen);
		fputc('\n', stdout);

		n++;
	}
	close(sock); //close socket after all messages are sent and received

	return 0;
}
