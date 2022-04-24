//
// Created by ���� on 2021/5/9.
//

#ifndef SEEKFREE_CH32V103R8T6_OPENSOURCE_LIBRARY_FLASH_H
#define SEEKFREE_CH32V103R8T6_OPENSOURCE_LIBRARY_FLASH_H
#include "headfile.h"



#define FLASH_STORAGE_SIZE      18

typedef struct
{
    uint32 header;
    int8   add_wz;
    int16  vx_speed;
    int16  vy_speed;
    int16  speed_kp;
    int16  speed_ki;
    int16  dir_kp;
    int16  dir_kd;
    int16  fork_kp;
    int16  fork_kd;
    uint16 out_time;
    int16  out_speed;
    uint16 rotime[3];
    int16  cirspeed;
    int16  garspeed;
    int16  ramspeed;
    int16  forkspeed;
    int16  rospeed;
    int16  sepval;
    int16  maxspeed;
    uint8  order[6];
    uint16 inrotime;
    uint16 ingotime;
    int16  one;
    int16  two;
}flash_storage_t;

typedef enum
{
    HEADER  =  0,
    SEP_VAL     ,   //ͼ������ֵ
    SPEED       ,   //ǰ16λΪx�����ٶȣ���16λΪy�����ٶ�
    DIR_PD      ,
    FORK_PD     ,
    SPEED_PI    ,
    OUT_GARAGE  ,
    ONETWO      ,
    ROTA_IN     ,   //���������ת�Ƕ�
    ROTA_OUT    ,   //���������ת�Ƕ�
    ADD_WZ      ,
    MAX_SPEED   ,
    PRIORITY_0  ,
    PRIORITY_1  ,
    PRIORITY_2  ,
    RAM_FORK_SPEED   ,
    CIR_GARA_SPEED   ,
    INGAR_TIME  ,
}info_addr_t;



extern flash_storage_t  flash;


void Flash_Init();
void Flash_IN(flash_storage_t  *fpoint);
extern flash_storage_t memory[4];
#endif //SEEKFREE_CH32V103R8T6_OPENSOURCE_LIBRARY_FLASH_H
