#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include "../icslab2_net.h"

#define BUF_SIZE 1000
#define SERVER_IP "172.29.0.40"
#define CLIENT_IP "172.23.0.10"

// #define SERVER_IP "127.0.0.1"       // Default server IP address
#define SERVER_PORT UDP_SERVER_PORT  // Default server port
#define ROUTER_PORT 10000  // Default router port for client to connect
#define FORWARD_PORT 10000
int main(int argc, char **argv) {
  int client_udp_sock, server_udp_sock;
  struct sockaddr_in router_addr, client_addr, server_addr;
  socklen_t client_addr_len = sizeof(client_addr);
  char buffer[BUF_SIZE + 1];
  int n;

  // UDPクライアント定義
  client_udp_sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (client_udp_sock < 0) {
    perror("Client UDP socket creation failed");
    exit(1);
  }

  // Bind
  memset(&router_addr, 0, sizeof(router_addr));
  router_addr.sin_family = AF_INET;
  router_addr.sin_port = htons(ROUTER_PORT);
  router_addr.sin_addr.s_addr = INADDR_ANY;
  if (bind(client_udp_sock, (struct sockaddr *)&router_addr,
           sizeof(router_addr)) < 0) {
    perror("UDP bind for client failed");
    exit(1);
  }

  server_udp_sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (server_udp_sock < 0) {
    perror("Server UDP socket creation failed");
    exit(1);
  }

  // サーバーセットアップ
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(SERVER_PORT);
  inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr);

  struct sockaddr_in forward_addr;
  memset(&forward_addr, 0, sizeof(forward_addr));
  forward_addr.sin_family = AF_INET;
  forward_addr.sin_port = htons(FORWARD_PORT);
  inet_pton(AF_INET, CLIENT_IP, &forward_addr.sin_addr);

  printf("Router waiting on port %d for client data...\n", ROUTER_PORT);

  int packetCounter = 0;

  while (1) {
    n = recvfrom(client_udp_sock, buffer, BUF_SIZE, 0,
                 (struct sockaddr *)&client_addr, &client_addr_len);

    if (n < 0) {
      perror("UDP receive from client failed");
      continue;  // Keep waiting for the next client message
    }
    printf("Data received\n");
    buffer[n] = '\0';
    printf("Received data: %s\n", buffer);
    // sendto(server_udp_sock, buffer, n, 0, (struct sockaddr *)&server_addr,
    // sizeof(server_addr));

    // n = recvfrom(server_udp_sock, buffer, BUF_SIZE, 0, NULL, NULL);
    // Forward the response to the specified IP and port
    printf("Forward address: IP = %s, PORT = %d\n",
           inet_ntoa(server_addr.sin_addr), ntohs(server_addr.sin_port));
    printf("Forward address: IP = %s, PORT = %d\n",
           inet_ntoa(forward_addr.sin_addr), ntohs(forward_addr.sin_port));

    int result = sendto(client_udp_sock, buffer, n, 0,
                        (struct sockaddr *)&forward_addr, sizeof(forward_addr));
    if (result < 0) {
      perror("sendto failed");
      continue;
    }
    packetCounter++;
    // usleep(10);
    if (strncmp(buffer, "END\n", BUF_LEN) == 0) {
      printf("end sign. Prev packet: %d\n", packetCounter);
      packetCounter = 0;
    }
    // sendto(client_udp_sock, buffer, n, 0, (struct sockaddr *)&forward_addr,
    // sizeof(forward_addr));
    printf("Data forwarded\n");
  }

  // Close sockets outside the loop
  close(client_udp_sock);
  close(server_udp_sock);
  return 0;
}
