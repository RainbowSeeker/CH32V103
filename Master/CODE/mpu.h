//
// Created by 杨宇 on 2021/3/3.
//

#ifndef SEEKFREE_CH32V103R8T6_OPENSOURCE_LIBRARY_MPU_H
#define SEEKFREE_CH32V103R8T6_OPENSOURCE_LIBRARY_MPU_H
#include "main.h"

#define ACC_WEIGHT      0.02f
#define SAMPLING_TIME   0.0004f
#define OFFSET          0


typedef struct
{
    float dt;       //注意：dt的取值为滤波器采样时间
    float Pitch;
    int16 Yaw;    //6050无法测得偏航角
    int16 Roll;
    int16 Yaw_Gyro;
}MPU_Info;

extern MPU_Info mpu;


void MPU_Init();

void First_Order_Complementary_Filter(MPU_Info *filter);//采集后计算的角度和角加速度

#endif //SEEKFREE_CH32V103R8T6_OPENSOURCE_LIBRARY_MPU_H
