#include <netdb.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>

#include "icslab2_net.h"

#define MIN(x, y) (((x) < (y)) ? (x) : (y))

#define CLIENT_IP "172.29.0.50"  // Fixed client IP address to send data
#define ACK "END\n"
#define ACK_LOOP 100

#define DST_SIZE 5

int main(int argc, char **argv) {
  int sock;                      /* ソケットディスクリプタ */
  struct sockaddr_in serverAddr; /* サーバ＝自分用アドレス構造体 */
  struct sockaddr_in clientAddr; /* クライアント＝相手用アドレス構造体 */
                                 // struct sockaddr_in sendtoAddr[DST_SIZE + 1];
  struct addrinfo *sendtoAddr[DST_SIZE];
  char *sendtoURL[DST_SIZE + 1] = {"node1", "node2", "node3", "node4", "node5"};

  int addrLen;       /* clientAddrのサイズ */
  char buf[BUF_LEN]; /* 受信バッファ */
  int n;             /* 受信バイト数 */

  struct in_addr addr; /* アドレス表示用 */

  int fd;
  char *input_txt;

  int i; /* ループ用変数 */

  int destNode[DST_SIZE] = {5};  // 送信先ノード
  double ratio[DST_SIZE] = {1};  // 送信先ノードへの割合
  int sentDestCounter[DST_SIZE] = {0};  // 送信先ノードへの送信パケット数

  for (i = 0; i <= DST_SIZE; i++) {
    struct addrinfo hints, *info;
    memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_DGRAM;
    getaddrinfo(sendtoURL[i], NULL, &hints, &info);
    ((struct sockaddr_in *)info->ai_addr)->sin_port = htons(UDP_SERVER_PORT);
    sendtoAddr[i] = info;
  }

  /* コマンドライン引数の処理 */
  if (argc % 2 != 0) {
    printf(
        "Usage: %s [input_txt] [node_id1] [ratio1] [node_id2] [ratio2] ...\n",
        argv[0]);
    return 1;
  }

  int destInputNum = MIN((argc - 2) / 2, DST_SIZE);
  double ratioSum = 0;
  for (int i = 0; i < destInputNum; i++) {
    destNode[i] = atoi(argv[2 * i + 2]);
    ratio[i] = atof(argv[2 * i + 3]) + ratioSum;
    ratioSum = ratio[i];
    printf("destNode: %d, ratio: %f\n", destNode[i], ratio[i]);
  }

  /* 転送ファイルの取得 */
  input_txt = argv[1];
  fd = open(input_txt, O_RDONLY);

  /* STEP 1: UDPソケットをオープンする */
  if ((sock = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("socket");
    return 1;
  }

  /* STEP 2: クライアントからの要求を受け付けるIPアドレスとポートを設定する */
  memset(&serverAddr, 0, sizeof(serverAddr)); /* ゼロクリア */
  serverAddr.sin_family = AF_INET;            /* Internetプロトコル */

  /* STEP 2: 待ち受けるポート番号を 10000 (= UDP_SERVER_PORT)に設定 */
  serverAddr.sin_port = htons(UDP_SERVER_PORT);
  serverAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* どのIPアドレス宛でも */

  /* STEP 3:ソケットとアドレスをbindする */
  if (bind(sock, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
    perror("bind");
    return 1;
  }

  /* STEP 4: クライアントからのデータグラムを受けとる */
  addrLen = sizeof(clientAddr);

  // for ( ; ; ) {
  /* クライアントから情報を取得 */
  n = recvfrom(sock, buf, BUF_LEN, 0, (struct sockaddr *)&clientAddr,
               (socklen_t *)&addrLen);
  if (n < 0) {
    perror("recvfrom");
    return (1);
  }

  // Set the client address to the fixed IP address for sending data
  inet_pton(AF_INET, CLIENT_IP, &clientAddr.sin_addr);
  /* （後回し） STEP 4'xxx. 受信パケットの送信元IPアドレスとポート番号を表示 */
  printf("received from : ip address: %s, ", inet_ntoa(addr));
  printf("port#: %d\n", ntohs(clientAddr.sin_port));

  int sentPacket = 0, sentByte = 0;
  while ((n = read(fd, buf, BUF_LEN)) > 0) {
    double randomDest = (double)rand() / (RAND_MAX + 1.0) * ratioSum;
    int dest;
    for (int i = 0; i < destInputNum; i++) {
      if (randomDest < ratio[i]) {
        dest = destNode[i];
        sentDestCounter[i]++;
        break;
      }
    }
    if (sendto(sock, buf, n, 0, (sendtoAddr[dest - 1])->ai_addr,
               sizeof(struct sockaddr)) != n) {
      perror("sendto");
      return (1);
    }
    sentPacket++;
    sentByte += n;
    usleep(10);
  }

  printf("Message transmitted to client. Packet num: %d, %dByte\n", sentPacket,
         sentByte);

  /* 終了したというシグナルを送り元に対して返す */
  for (i = 0; i < ACK_LOOP; i++) {
    if (sendto(sock, ACK, 4, 0, (struct sockaddr *)&clientAddr, addrLen) != 4) {
      perror("sendto");
      return (1);
    }
    usleep(1000);
  }

  printf("EOT transmitted to client\n");
  //}
  //
  for (int i = 0; i < destInputNum; i++) {
    printf("destNode: %d, sentPacket: %d\n", destNode[i], sentDestCounter[i]);
  }

  close(sock); /* ソケットのクローズ */
  close(fd);   /* ファイルの終了 */

  return 0;
}
