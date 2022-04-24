//
// Created by ���� on 2021/3/3.
//

#ifndef SEEKFREE_CH32V103R8T6_OPENSOURCE_LIBRARY_MPU_H
#define SEEKFREE_CH32V103R8T6_OPENSOURCE_LIBRARY_MPU_H
#include "main.h"

#define ACC_WEIGHT      0.02f
#define SAMPLING_TIME   0.0004f
#define OFFSET          0


typedef struct
{
    float dt;       //ע�⣺dt��ȡֵΪ�˲�������ʱ��
    float Pitch;
    int16 Yaw;    //6050�޷����ƫ����
    int16 Roll;
    int16 Yaw_Gyro;
}MPU_Info;

extern MPU_Info mpu;


void MPU_Init();

void First_Order_Complementary_Filter(MPU_Info *filter);//�ɼ������ĽǶȺͽǼ��ٶ�

#endif //SEEKFREE_CH32V103R8T6_OPENSOURCE_LIBRARY_MPU_H
