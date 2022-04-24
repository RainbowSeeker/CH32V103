//
// Created by —Ó”Ó on 2021/8/2.
//

#ifndef SEEKFREE_CH32V103R8T6_OPENSOURCE_LIBRARY_FUNNC_H
#define SEEKFREE_CH32V103R8T6_OPENSOURCE_LIBRARY_FUNNC_H
#include "headfile.h"

#define MAX_LEN         20

extern uint8_t pack[5];


void parseUartData(uint8_t dat,uint8_t *tobuff);
void analyzePack(uint8_t *from,uint8_t *to,uint8_t len);
#endif //SEEKFREE_CH32V103R8T6_OPENSOURCE_LIBRARY_FUNNC_H
