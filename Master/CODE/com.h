//
// Created by 杨宇 on 2021/3/30.
//
#ifndef SEEKFREE_CH32V103R8T6_OPENSOURCE_LIBRARY_COM_H
#define SEEKFREE_CH32V103R8T6_OPENSOURCE_LIBRARY_COM_H
#include "main.h"

#define E_START                 0       //准备状态
#define E_OK                    1       //成功

#define LINE_LEN                14     //数据长度
#define TX_LEN                  5

extern vuint8  Rx_Flag;                      //接收数据标志位
extern uint8   RxBuff[LINE_LEN];            //主机用于接收数据的BUFF
extern uint8   TxBuff[LINE_LEN];
extern int16 data_return;//根据主机要求返还对应值


extern int16 slave_encoder_left;               //从机左编码器值
extern int16 slave_encoder_right;              //从机右编码器值
extern int16 slave_error_val;
extern int16 slave_controlled_val;



void get_slave_data(uint8 data);
void data_analysis(uint8 *line);
void get_sensor_data(void);
void process_data(void);
void Com_Init();
#endif //SEEKFREE_CH32V103R8T6_OPENSOURCE_LIBRARY_COM_H
