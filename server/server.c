#include "icslab2_net.h"
#include <time.h>

#define CLIENT_IP "172.29.0.40"  // Fixed client IP address to send data
#define ACK "END\n"
#define ACK_LOOP 100

int main(int argc, char** argv)
{
	int sock; 
	struct sockaddr_in serverAddr, clientAddr;
	int addrLen;
	char buf[BUF_LEN];
	int n;
	struct in_addr addr;
	int fd;
	char *input_txt;
	int i = 0;

	if (argc != 2) {
		printf("Usage: %s [input_txt]\n", argv[0]);
		return 1;
	}

	input_txt = argv[1];
	fd = open(input_txt, O_RDONLY);

	sock = socket(AF_INET, SOCK_DGRAM, 0);
	if (sock < 0) {
		perror("socket");
		return 1;
	}

	memset(&serverAddr, 0, sizeof(serverAddr));
	serverAddr.sin_family = AF_INET;
	serverAddr.sin_port = htons(UDP_SERVER_PORT);
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
	if (bind(sock, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
		perror("bind");
		return 1;
	}

	addrLen = sizeof(clientAddr);
	n = recvfrom(sock, buf, BUF_LEN, 0, (struct sockaddr *)&clientAddr, (socklen_t *)&addrLen);
	if (n < 0) {
		perror("recvfrom");
		return (1);
	}

	addr.s_addr = clientAddr.sin_addr.s_addr;
	printf("received from : ip address: %s, ", inet_ntoa(addr));
	printf("port#: %d\n", ntohs(clientAddr.sin_port));

	// Set the client address to the fixed IP address for sending data
	inet_pton(AF_INET, CLIENT_IP, &clientAddr.sin_addr);

	while ((n = read(fd, buf, BUF_LEN)) > 0) {
		if (sendto(sock, buf, n, 0, (struct sockaddr *)&clientAddr, addrLen) != n) {
			perror("sendto");
			return (1);
		}
		usleep(10);
	}

	printf("Message transmitted to client\n");

	for (i = 0; i < ACK_LOOP; i++) {
		if (sendto(sock, ACK, 4, 0, (struct sockaddr *)&clientAddr, addrLen) != 4) {
			perror("sendto");
			return (1);
		}
		usleep(1000);
	}

	printf("EOT transmitted to client\n");

	close(sock);
	close(fd);

	return 0;
}

