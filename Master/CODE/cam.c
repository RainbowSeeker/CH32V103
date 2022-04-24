/*
 * cam.c
 *
 *  Created on: Jan 20, 2021
 *      Author: yy
 */

#include "cam.h"
#include "main.h"




#define error(str)  \
{                   \
    while (1)       \
    {               \
        puts(str);  \
    }               \
}                   \



static int16 Radio(int16 line);
#define Range_Limit(input,bottom,top)   \
    {                                   \
        if((input)>=(top))              \
        {                               \
            (input)=(top);              \
        }                               \
        else if ((input)<=(bottom))     \
        {                               \
            (input)=(bottom);           \
        }                               \
    }                                   \


//摄像头误差の低通滤波器（适当防止误差突变）
//Filter_Info Error_Filter = {0,0,8}; //比例要除以10，故实际为0.8

int16  Cam_Error=0;
int16  remote_radio = 0;
//-------------------------------------------------------------
//内部使用变量


void Camera_Init()
{
    mt9v03x_init();
    Threshold_Cal();
    Schedule_Init();

}
void Cam_Handle_Loop()
{

    Image_Process(mt9v03x_image);
    GetMidLine();
    CenterLineHandle();
//    for(uint8 i=0;i<height;i++)
//    {
//        printf("Lef[%d]:%d  ",i,Line[i].Lefborder);
//        printf("Rig[%d]:%d  ",i,Line[i].Rigborder);
//        printf("Mid[%d]:%d\r\n",i,Line[i].Rigborder-Line[i].Lefborder);
//    }
}

static int16 Radio(int16 line)
{
    if (line>height-20)
    {
        return (10);//6
    }
    else if(line>height-50)       //中间部分
    {
        return (20);//20
    }else
    {
        return (remote_radio);
    }
}
void CenterLineHandle()
{
    if (tem_error)
    {
        Cam_Error = tem_error;
        return;
    }
    int16 Deviation=0;
    for(uint8 t=height-1;t>=SepLine&&t>3;t=t-3)
    {
        Deviation+=Radio(t)*(ABS_MID-Line[t].Midpoint);
    }
    Deviation=Deviation/50;
    //误差限幅
    Range_Limit(Deviation,-CAM_ERROR_RANGE,CAM_ERROR_RANGE);
    //对误差进行一阶低通滤波，适当抑制中线突变
//    First_Order_Low_Pass_Filter(&Error_Filter,Deviation);
//    Cam_Error=Error_Filter.output;
    Cam_Error = Deviation;
}


