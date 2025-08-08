#ifndef NODE_LIST_H
#define NODE_LIST_H

#include "stdint.h"

typedef struct Node {
    uint8_t data;
    struct Node* next;
} Node;

#endif
