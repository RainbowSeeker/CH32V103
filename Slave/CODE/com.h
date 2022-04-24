//
// Created by ���� on 2021/3/30.
//
#ifndef SEEKFREE_CH32V103R8T6_OPENSOURCE_LIBRARY_COM_H
#define SEEKFREE_CH32V103R8T6_OPENSOURCE_LIBRARY_COM_H
#include "headfile.h"

#define E_START                 0       //׼��״̬
#define E_OK                    1       //�ɹ�
#define E_FRAME_HEADER_ERROR    2       //֡ͷ����
#define E_FRAME_RTAIL_ERROR     3       //֡β����

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

typedef enum
{
    Lef_Up  =1,
    Rig_Up  =2,
    Lef_Down=3,
    Rig_Down=4,
}Wheel_No;
typedef struct
{
    Wheel_No        no;
    PWMCH_enum      channel; //�������ͨ��
    PIN_enum        dirpin; //�����ת������ƽ�
    TIMERN_enum     timern;
    TIMER_PIN_enum  phaseA;
    TIMER_PIN_enum  phaseB;
    uint32          duty;
    int16           nowspeed;
}Motor;
void Encoder_Init();
void get_slave_data(uint8 data);
void data_analysis(uint8 *line);
void get_sensor_data(void);
void process_data(void);
void Com_Init();
#endif //SEEKFREE_CH32V103R8T6_OPENSOURCE_LIBRARY_COM_H
