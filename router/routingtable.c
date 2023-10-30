#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>


#define LAN1_2 "172.20.0.10"
#define LAN1_3 "172.21.0.10"
#define LAN1_4 "172.22.0.10"
#define LAN1_5 "172.23.0.10"
#define LAN2_1 "172.20.0.20"
#define LAN2_3 "172.24.0.20"
#define LAN2_4 "172.25.0.20"
#define LAN2_5 "172.26.0.20"
#define LAN3_1 "172.21.0.30"
#define LAN3_3 "172.24.0.30"
#define LAN3_4 "172.27.0.30"
#define LAN3_5 "172.28.0.30"
#define LAN4_1 "172.22.0.40"
#define LAN4_2 "172.25.0.40"
#define LAN4_3 "172.27.0.40"
#define LAN4_5 "172.29.0.40"
#define LAN5_1 "172.23.0.50"
#define LAN5_2 "172.26.0.50"
#define LAN5_3 "172.28.0.50"
#define LAN5_4 "172.29.0.50"


typedef struct {
  struct in_addr from;
  struct in_addr to;
} Route;

char *routes[5][5] = {
    {NULL, LAN1_2, LAN1_3, LAN1_4, LAN1_5},
    {LAN2_1, NULL, LAN2_3, LAN2_4, LAN2_5},
    {LAN3_1, LAN3_3, NULL, LAN3_4, LAN3_5},
    {LAN4_1, LAN4_2, LAN4_3, NULL, LAN4_5},
    {LAN5_1, LAN5_2, LAN5_3, LAN5_4, NULL}
};

Route route_array[20];

void initializer(void) {
    int k = 0;
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 5; j++) {
            if (routes[i][j] != NULL) {
                inet_aton(routes[i][j], &route_array[k].from);
                inet_aton(routes[j][i], &route_array[k].to);
                k++;
            }
        }
    }
}