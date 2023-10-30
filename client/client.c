/*  -*- coding: utf-8-unix; -*-                                     */
/*  FILENAME     :  tcp_echo_client.c                               */
/*  DESCRIPTION  :  TCP Echo Client                                 */
/*  USAGE        :  tcp_echo_client.out [dst_ip_addr] [port]        */
/*  DATE         :  Sep 01, 2020                                    */
/*                                                                  */

#include "../icslab2_net.h"
#include <sys/socket.h>
#include <time.h>

int main(int argc, char **argv) {
  char *server_ipaddr_str = "127.0.0.1"; /* サーバIPアドレス（文字列） */
  unsigned int port = UDP_SERVER_PORT; /* ポート番号 */
  char *filename = "test.txt";

  int sock;                      /* ソケットディスクリプタ */
  struct sockaddr_in serverAddr; /* サーバ＝相手用のアドレス構造体 */
  char buf[BUF_LEN];             /* 受信バッファ */
  int n;                         /* 読み込み／受信バイト数 */

  struct in_addr addr; /* アドレス表示用 */

  /* コマンドライン引数の処理 */
  if (argc == 2 && strncmp(argv[1], "-h", 2) == 0) {
    printf("Usage: %s [dst_ip_addr] [port] [target_file]\n", argv[0]);
    return 0;
  }

  if (argc > 1) /* 宛先を指定のIPアドレスにする。 portはデフォルト */
    server_ipaddr_str = argv[1];
  if (argc > 2) /* 宛先を指定のIPアドレス、portにする */
    port = (unsigned int)atoi(argv[2]);
  if (argc > 3)
    filename = argv[3];

  FILE *file = fopen(filename, "w");

  /* STEP 1: 宛先サーバのIPアドレスとポートを指定する */
  memset(&serverAddr, 0, sizeof(serverAddr)); /* 0クリア */
  serverAddr.sin_family = AF_INET;            /* Internetプロトコル */
  serverAddr.sin_port = htons(port);          /* サーバの待受ポート */
  /* IPアドレス（文字列）から変換 */
  inet_pton(AF_INET, server_ipaddr_str, &serverAddr.sin_addr.s_addr);

  /* 確認用：IPアドレスを文字列に変換して表示 */
  addr.s_addr = serverAddr.sin_addr.s_addr;
  printf("ip address: %s\n", inet_ntoa(addr));
  printf("port#: %d\n", ntohs(serverAddr.sin_port));

  /* STEP 2x: UDPソケットをオープンする */
  sock = socket(AF_INET, SOCK_DGRAM, 0);
  if (sock < 0) {
    perror("socket");
    return 1;
  }

  struct timespec start_time, end_time;
  clock_gettime(CLOCK_REALTIME, &start_time);

  char nullData = 0;
  if (sendto(sock, &nullData, sizeof(char), 0, (struct sockaddr *)&serverAddr,
             sizeof(struct sockaddr)) != 1) {
    perror("request send");
    return -1;
  }

  uint32_t total = 0, recvCounter;
  /* STEP 4xxx: サーバからデータグラムを受けとる */
  while ((n = read(sock, buf, BUF_LEN - 1)) > 0) {
    buf[n] = '\0';
    printf("received %d bytes\n", n);
    fwrite(buf, sizeof(char), n, file);
    total += n;
  }

  struct sockaddr clientAddr;
  socklen_t addrLen = sizeof(struct sockaddr); /* serverAddrのサイズ */
  int endCounter = 0;
  while ((n = recvfrom(sock, buf, BUF_LEN, 0, (struct sockaddr *)&clientAddr,
                       &addrLen)) > 0) {
    if (strncmp(buf, "end\n", BUF_LEN)) {
      endCounter++;
      if(endCounter >= 10) break;
    }
    fwrite(buf, sizeof(char), n, file);
    total += n;
  }
  clock_gettime(CLOCK_REALTIME, &end_time);

  double elapsed_time_ms = (end_time.tv_sec - start_time.tv_sec) * 1e3 +
                           (end_time.tv_nsec - start_time.tv_nsec) / 1e6;

  printf("total: %d bytes\n", total);
  printf("elapsed time: %6.5lf ms\n", elapsed_time_ms);
  printf("throughput: %6.1lf kbps\n",
         total * 8 / elapsed_time_ms * 1000 / 1000);
  printf("received %d packets\n", recvCounter);

  /* STEP 6: ソケットのクローズ */
  close(sock);
  fclose(file);
  printf("closed\n");

  return 0;
}

/* Local Variables: */
/* compile-command: "gcc tcp_echo_client.c -o tcp_echo_client.out " */
