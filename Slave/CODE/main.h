/*
 * main.h
 *
 *  Created on: Jan 20, 2021
 *      Author: —Ó”Ó
 */

#ifndef USER_MAIN_H_
#define USER_MAIN_H_
#include "headfile.h"
#include "adc.h"
#include "cam.h"
#include "com.h"
#include "mpu.h"
#include "image_process.h"
#include "useri2c.h"
#define delay_ms(__xms)    systick_delay_ms(__xms)


extern int16 StopReg;
void Master_All_Init();
void Slave_All_Init();
void Print_Data();
int max(int x,int y);
int min(int x,int y);
uint16 GetData_TOF_IIC();
#endif /* USER_MAIN_H_ */
