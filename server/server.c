#include "icslab2_net.h"
#include <time.h>

#define ACK "END\n"
#define ACK_LOOP 100

#define DST_SIZE 4

int main(int argc, char** argv)
{
	int     sock;                  /* ソケットディスクリプタ */
	struct sockaddr_in  serverAddr; /* サーバ＝自分用アドレス構造体 */
	struct sockaddr_in  clientAddr; /* クライアント＝相手用アドレス構造体 */
	struct sockaddr_in  sendtoAddr[DST_SIZE];
	char *sendAddr[] = {"172.21.0.30", "172.24.0.30", "172.27.0.30", "172.28.0.30"};
	int     addrLen;                /* clientAddrのサイズ */
	char    buf[BUF_LEN];          /* 受信バッファ */
	int     n;                      /* 受信バイト数 */

	struct in_addr addr;            /* アドレス表示用 */

	int fd;
	char *input_txt;

	int i;                          /* ループ用変数 */

	for (i = 0; i < DST_SIZE; i++) {
		sendtoAddr[i].sin_family = SOCK_DGRAM;
		sendtoAddr[i].sin_port = htons(UDP_SERVER_PORT);
		inet_pton(AF_INET, sendAddr[i], &sendtoAddr[i].sin_addr.s_addr);
	}

	/* コマンドライン引数の処理 */
	if (argc != 2) {
		printf("Usage: %s [input_txt]\n", argv[0]);
		return 1;
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
	memset(&serverAddr, 0, sizeof(serverAddr));     /* ゼロクリア */
	serverAddr.sin_family = AF_INET;                /* Internetプロトコル */

	/* STEP 2: 待ち受けるポート番号を 10000 (= UDP_SERVER_PORT)に設定 */
	serverAddr.sin_port =  htons(UDP_SERVER_PORT);
	serverAddr.sin_addr.s_addr = htonl(INADDR_ANY); /* どのIPアドレス宛でも */

	/* STEP 3:ソケットとアドレスをbindする */
	if (bind(sock, (struct sockaddr *)&serverAddr, sizeof(serverAddr)) < 0) {
		perror("bind");
		return  1;
	}

	/* STEP 4: クライアントからのデータグラムを受けとる */
	addrLen = sizeof(clientAddr);

	//for ( ; ; ) {
		/* クライアントから情報を取得 */
		n = recvfrom(sock, buf, BUF_LEN, 0,
					(struct sockaddr *)&clientAddr, (socklen_t *)&addrLen);
		if (n < 0) {
			perror("recvfrom");
			return (1);
		}

		/* （後回し） STEP 4'xxx. 受信パケットの送信元IPアドレスとポート番号を表示 */
		addr.s_addr = clientAddr.sin_addr.s_addr;
		printf("received from : ip address: %s, ", inet_ntoa(addr));
		printf("port#: %d\n", ntohs(clientAddr.sin_port));

		while ((n = read(fd, buf, BUF_LEN)) > 0) {
			if (sendto(sock, buf, n, 0, (struct sockaddr *)&clientAddr, addrLen) != n) {
				perror("sendto");
				return (1);
			}
			usleep(10);
		}

		printf("Message transmitted to client\n");

		/* ack to tell end of transmission */
		for (i = 0; i < ACK_LOOP; i++) {
			if (sendto(sock, ACK, 4, 0, (struct sockaddr *)&clientAddr, addrLen) != 4) {
				perror("sendto");
				return (1);
			}
			usleep(1000);
		}

		printf("EOT transmitted to client\n");
	//}

	close(sock);                               /* ソケットのクローズ */
	close(fd);                                 /* ファイルの終了 */

	return 0;
}
