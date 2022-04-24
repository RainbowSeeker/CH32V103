

#ifndef _YB_OLED_LIST_H_
#define _YB_OLED_LIST_H_

#include "main.h"


typedef enum
{
    KEY_U,  //上                 0
    KEY_D,  //下                 1
    KEY_L,  //左                 2
    KEY_R,  //右                 3
    KEY_A,  //选择               4
} KEY;


//key状态枚举定义
typedef enum
{
    KEY_DOWN  =   0,         //按键按下时对应电平
    KEY_UP    =   1,         //按键弹起时对应电平

} KEY_STATUS;

void            key_init(uint8 key_num);            // KEY初始化函数
KEY_STATUS      key_get(KEY key);             //检测key状态（不带延时消抖）
KEY_STATUS      key_check(KEY key);           //检测key状态（带延时消抖）
void DisplayMenu();        //显示菜单列表
void Menu_Init();          //菜单初始化
void Menu();               //一级菜单
void Set_PID();    //修改pid参数
void Camera();             //显示摄像头图像
void Go(uint8 *jj);                 //发车
void Move_Up_Down(uint8 *j, uint8 k);       //上下移动
void Set_Speed();
void Set_Mode();
void Read_Para();
void Set_Priority();

#endif
