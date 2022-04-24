//
// Created by 杨宇 on 2021/8/2.
//

#include "funnc.h"
#include "buzzer.h"


uint8_t pack[5]={0,0,0,0,0};



static uint8_t dat_buff[MAX_LEN];
void parseUartData(uint8_t dat,uint8_t *tobuff)
{
    static int index = 0;
    dat_buff[index++] = dat;// 接收数据
    // 收到的数据不是以0xA5开头
    if (dat_buff[0] != 0xA5)
    {
        index = 0;
    }
    if (index > 2&&dat_buff[index-1] == 0x5A)
    {
        uint8_t checknum = 0;
        for (int t = 1; t < index - 2; ++t)
        {
            checknum += dat_buff[t];
        }
        if (dat_buff[index-2]==checknum)
        {
            memcpy(tobuff,&dat_buff[1],index-3);
        }
        index = 0;
    }
    if (index >= MAX_LEN) {
        index = 0;
    }
}

void analyzePack(uint8_t *from,uint8_t *to,uint8_t len)
{
    for (uint8_t t = 0; t < len; ++t)
    {
        from[t] = to[t];
    }
}