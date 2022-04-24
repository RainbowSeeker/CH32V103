

#ifndef _YB_OLED_LIST_H_
#define _YB_OLED_LIST_H_

#include "main.h"


typedef enum
{
    KEY_U,  //��                 0
    KEY_D,  //��                 1
    KEY_L,  //��                 2
    KEY_R,  //��                 3
    KEY_A,  //ѡ��               4
} KEY;


//key״̬ö�ٶ���
typedef enum
{
    KEY_DOWN  =   0,         //��������ʱ��Ӧ��ƽ
    KEY_UP    =   1,         //��������ʱ��Ӧ��ƽ

} KEY_STATUS;

void            key_init(uint8 key_num);            // KEY��ʼ������
KEY_STATUS      key_get(KEY key);             //���key״̬��������ʱ������
KEY_STATUS      key_check(KEY key);           //���key״̬������ʱ������
void DisplayMenu();        //��ʾ�˵��б�
void Menu_Init();          //�˵���ʼ��
void Menu();               //һ���˵�
void Set_PID();    //�޸�pid����
void Camera();             //��ʾ����ͷͼ��
void Go(uint8 *jj);                 //����
void Move_Up_Down(uint8 *j, uint8 k);       //�����ƶ�
void Set_Speed();
void Set_Mode();
void Read_Para();
void Set_Priority();

#endif
