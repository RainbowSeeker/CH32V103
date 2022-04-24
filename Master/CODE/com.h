//
// Created by ���� on 2021/3/30.
//
#ifndef SEEKFREE_CH32V103R8T6_OPENSOURCE_LIBRARY_COM_H
#define SEEKFREE_CH32V103R8T6_OPENSOURCE_LIBRARY_COM_H
#include "main.h"

#define E_START                 0       //׼��״̬
#define E_OK                    1       //�ɹ�

#define LINE_LEN                14     //���ݳ���
#define TX_LEN                  5

extern vuint8  Rx_Flag;                      //�������ݱ�־λ
extern uint8   RxBuff[LINE_LEN];            //�������ڽ������ݵ�BUFF
extern uint8   TxBuff[LINE_LEN];
extern int16 data_return;//��������Ҫ�󷵻���Ӧֵ


extern int16 slave_encoder_left;               //�ӻ��������ֵ
extern int16 slave_encoder_right;              //�ӻ��ұ�����ֵ
extern int16 slave_error_val;
extern int16 slave_controlled_val;



void get_slave_data(uint8 data);
void data_analysis(uint8 *line);
void get_sensor_data(void);
void process_data(void);
void Com_Init();
#endif //SEEKFREE_CH32V103R8T6_OPENSOURCE_LIBRARY_COM_H
