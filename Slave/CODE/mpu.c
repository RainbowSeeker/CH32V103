//
// Created by ���� on 2021/3/3.
//

#include "mpu.h"
#include "math.h"
// ��������


MPU_Info mpu;

void MPU_Init()
{
    icm20602_init_spi();
//    mpu6050_init();
    mpu.Pitch=90;
    mpu.Yaw  =0;
    mpu.Roll =0;
    mpu.dt=SAMPLING_TIME;
}
static void GetMPUVal()
{
//    get_icm20602_accdata_spi();
    get_icm20602_gyro_spi();
//    mpu6050_get_accdata();
//    mpu6050_get_gyro();
}


//һ�׻����˲�
void First_Order_Complementary_Filter(MPU_Info *filter)//�ɼ������ĽǶȺͽǼ��ٶ�
{
    GetMPUVal();
//    float ax,gy;
//    ax=atan2(-icm_acc_x,icm_acc_y)*57.3;         //���ٶȵõ��ĽǶ�
//    gy=(float)icm_gyro_z;           //�����ǵõ��Ľ��ٶ�
//    filter->Pitch = ACC_WEIGHT * ax + (1-ACC_WEIGHT) * (filter->Pitch - gy * filter->dt);

    filter->gyro_z = icm_gyro_y - OFFSET;


//    static uint8 intime=0;
//    intime++;
//    if (intime>=4)
//    {
//        int16 data[2];
//        data[0] = slave_encoder_left;
//        data[1] = slave_encoder_right;
//        vcan_sendware(&data,sizeof (data));
//    }
//
//    ax=atan(icm_acc_x/icm_acc_y)*57.3;         //���ٶȵõ��ĽǶ�
//    gy=(float)icm_gyro_z/7510.0;           //�����ǵõ��Ľ��ٶ�
//    filter->Yaw = ACC_WEIGHT * ax + (1-ACC_WEIGHT) * (filter->Yaw + gy * filter->dt);
//
//    ax=atan(icm_acc_y/icm_acc_z)*57.3;         //���ٶȵõ��ĽǶ�
//    gy=(float)icm_gyro_x/7510.0;           //�����ǵõ��Ľ��ٶ�
//    filter->Roll = ACC_WEIGHT * ax + (1-ACC_WEIGHT) * (filter->Roll + gy * filter->dt);


//    filter->Pitch=(atan2(mpu_acc_x,mpu_acc_z)/3.14)*180;      //���㸩����

//    icm_gyro_x/=10;
//
//    filter->Yaw+=icm_gyro_x*2/100;


}
