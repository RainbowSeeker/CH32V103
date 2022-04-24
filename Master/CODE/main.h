/*
 * main.h
 *
 *  Created on: Jan 20, 2021
 *      Author: 杨宇
 */

#ifndef USER_MAIN_H_
#define USER_MAIN_H_
#include "headfile.h"
#include "buzzer.h"
#include "adc.h"
#include "cam.h"
#include "com.h"
#include "menu.h"
#include "move.h"
#include "mpu.h"
#include "image_process.h"
#include "flash.h"
#include "useri2c.h"
#include "funnc.h"
#define delay_ms(__xms)    systick_delay_ms(__xms)

#define WAIT_FOR_IMAGE()   {mt9v03x_finish_flag=0;while(mt9v03x_finish_flag==0);}

typedef struct
{
    int  choose;    //0:正常模式 1:遥控模式
//    void (*init)(void);
    void (*loop)(void);
}run_mode_t;


extern uint16_t  distance;

void Master_All_Init();
void Print_Data();
int max(int x,int y);
int min(int x,int y);
void share_hand();
#endif /* USER_MAIN_H_ */
