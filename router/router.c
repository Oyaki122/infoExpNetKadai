
#include "../icslab2_net.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <time.h>

#define BUF_SIZE 1000
#define LAN4_5 "172.29.0.40"
#define SERVER_IP "127.0.0.1"       // Default server IP address
#define SERVER_PORT UDP_SERVER_PORT // Default server port
#define ROUTER_PORT 10000           // Default router port for client to connect

int main(int argc, char **argv)
{
  int client_udp_sock, server_udp_sock;
  struct sockaddr_in router_addr, client_addr, server_addr;
  socklen_t client_addr_len = sizeof(client_addr);
  char buffer[BUF_LEN];
  int n;

  // UDPクライアント定義
  client_udp_sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (client_udp_sock < 0)
  {
    perror("Client UDP socket creation failed");
    exit(1);
  }

  // Bind
  memset(&router_addr, 0, sizeof(router_addr));
  router_addr.sin_family = AF_INET;
  router_addr.sin_port = htons(ROUTER_PORT);
  router_addr.sin_addr.s_addr = INADDR_ANY;
  if (bind(client_udp_sock, (struct sockaddr *)&router_addr, sizeof(router_addr)) < 0)
  {
    perror("UDP bind for client failed");
    exit(1);
  }

  server_udp_sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (server_udp_sock < 0)
  {
    perror("Server UDP socket creation failed");
    exit(1);
  }

  // サーバーセットアップ
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(SERVER_PORT);
  inet_pton(AF_INET, LAN4_5, &server_addr.sin_addr);

  printf("Router waiting on port %d for client data...\n", ROUTER_PORT);

  // Relay data between client and server
  while (1)
  {
    n = recvfrom(client_udp_sock, buffer, BUF_LEN, 0, (struct sockaddr *)&client_addr, &client_addr_len);
  
    {
      if (n < 0)
      {
        perror("UDP receive from client failed");
        break;
      }
      printf("data comeon");
      // Send data to UDP server
      sendto(server_udp_sock, buffer, n, 0, (struct sockaddr *)&server_addr, sizeof(server_addr));

      // Assuming a response for every send
      n = recvfrom(server_udp_sock, buffer, BUF_LEN, 0, NULL, NULL);
      if (n < 0)
      {
        perror("UDP receive from server failed");
        continue; // Just wait for the next client message
      }

      // Send the response back to the client
      sendto(client_udp_sock, buffer, n, 0, (struct sockaddr *)&client_addr, client_addr_len);
    }

    close(client_udp_sock);
    close(server_udp_sock);
  }
}
