// routing_table.h

#ifndef ROUTINGTABLE_H
#define ROUTINGTABLE_H

#include <netinet/in.h>

typedef struct
{
  struct in_addr from;
  struct in_addr to;
} Route;

void initializer(void);

#endif
