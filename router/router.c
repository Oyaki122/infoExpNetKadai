#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <sys/socket.h>

#define BUF_SIZE 1000
#define DST_SIZE 5

char *sendtoURL[DST_SIZE] = {"node1", "node2", "node3", "node4", "node5"};
struct addrinfo *sendtoAddr[DST_SIZE];

#define ROUTER_PORT 10000
int getNodeNumber(const char *nodeStr)
{
  return atoi(nodeStr + 4);
}
int main(int argc, char **argv)
{
  int udp_sock;
  struct sockaddr_in router_addr, client_addr;
  socklen_t client_addr_len = sizeof(client_addr);
  char buffer[BUF_SIZE + 1];
  int n;

  int destNode[DST_SIZE] = {5};
  double ratio[DST_SIZE] = {1};
  double ratioSum = 0;

  if ((argc - 1) % 2 != 0)
  {
    printf("Usage: %s [node_id1] [ratio1] [node_id2] [ratio2] ...\n", argv[0]);
    return 1;
  }

  int destInputNum = (argc - 1) / 2;
  for (int i = 0; i < destInputNum; i++)
  {
    destNode[i] = getNodeNumber(argv[2 * i + 1]);
    ratio[i] = atof(argv[2 * i + 2]) + ratioSum;
    ratioSum = ratio[i];
    printf("destNode: %d, ratio: %f\n", destNode[i], ratio[i]);
  }

  for (int i = 0; i < DST_SIZE; i++)
  {
    struct addrinfo hints, *info;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    getaddrinfo(sendtoURL[i], NULL, &hints, &info);
    info->ai_socktype = SOCK_DGRAM;
    info->ai_protocol = 0;
    ((struct sockaddr_in *)info->ai_addr)->sin_port = htons(ROUTER_PORT);
    sendtoAddr[i] = info;
  }

  udp_sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (udp_sock < 0)
  {
    perror("UDP socket creation failed");
    exit(1);
  }

  memset(&router_addr, 0, sizeof(router_addr));
  router_addr.sin_family = AF_INET;
  router_addr.sin_port = htons(ROUTER_PORT);
  router_addr.sin_addr.s_addr = INADDR_ANY;

  if (bind(udp_sock, (struct sockaddr *)&router_addr, sizeof(router_addr)) < 0)
  {
    perror("UDP bind for router failed");
    exit(1);
  }

  printf("Router waiting on port %d for data...\n", ROUTER_PORT);

  while (1)
  {
    n = recvfrom(udp_sock, buffer, BUF_SIZE, 0, (struct sockaddr *)&client_addr, &client_addr_len);

    if (n < 0)
    {
      perror("UDP receive failed");
      continue;
    }

    double randomDest = (double)rand() / (RAND_MAX + 1.0) * ratioSum;
    int dest;
    for (int i = 0; i < destInputNum; i++)
    {
      if (randomDest < ratio[i])
      {
        dest = destNode[i] - 1;
        break;
      }
    }

    int result = sendto(udp_sock, buffer, n, 0, sendtoAddr[dest]->ai_addr, sendtoAddr[dest]->ai_addrlen);
    if (result < 0)
    {
      perror("sendto failed");
      continue;
    }
    printf("Data forwarded to: %s\n", sendtoURL[dest]);
  }

  close(udp_sock);
  return 0;
}
