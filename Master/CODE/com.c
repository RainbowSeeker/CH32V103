//
// Created by 杨宇 on 2021/3/30.
//

#include "com.h"

uint8 RxBuff[LINE_LEN];    //主机用于接收数据的BUFF
uint8 TxBuff[LINE_LEN];


vuint8  Rx_Flag=0;
int16 data_return = 0;//根据主机要求返还对应值

int16 slave_encoder_left=0;               //从机左编码器值
int16 slave_encoder_right=0;              //从机右编码器值
int16 slave_error_val=0;
int16 slave_controlled_val=0;             //主机要求值

void Com_Init()
{
    spi_slave_init(SPI_2,SPI2_SCK_PB13,SPI2_MOSI_PB15,SPI2_MISO_PB14,A0,0,FPCLK_DIV_128);
    nvic_init(SPI2_IRQn,0,0,ENABLE);
    gpio_interrupt_init(B11, FALLING, IN_PULLUP);
    nvic_init(EXTI15_10_IRQn, 1, 0, ENABLE);
}
//-------------------------------------------------------------------------------------------------------------------
//  @brief      获取从机数据
//  @param      data            串口数据
//  @return     void
//  @since      v1.0
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void get_slave_data(uint8 data)
{
    static uint8 RxCount = 0;
    RxBuff[RxCount] = data;
    RxCount++;

    if(1 == RxCount)
    {
        //接收到的第一个字符不为0Xd8，帧头错误
        if(0Xd8 != RxBuff[0])
        {
            RxCount = 0;
        }
    }
    if(LINE_LEN == RxCount)
    {
        //接收到最后一个字节为0Xee
        if(0Xee == RxBuff[LINE_LEN-1])
        {
            Rx_Flag = E_OK;
        }
        RxCount = 0;
    }
}
//------------------------------------------------------------------------------------------------------------------
//  @brief      根据协议对从机发送过来的数据，进行数据解析
//  @param      *line                           串口接收到的数据BUFF
//  @return     void
//  @since      v1.0
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void data_analysis(uint8 *line)
{
    if(0Xb0 == line[1])    slave_encoder_left  = ((int16)line[2] << 8) | line[3];
    if(0Xb1 == line[4])    slave_encoder_right = ((int16)line[5] << 8) | line[6];
    if(0Xb2 == line[7])    slave_error_val     = ((int16)line[8] << 8) | line[9];
    if(0Xb3 == line[10])   slave_controlled_val= ((int16)line[11] << 8) | line[12];
}
//  @brief      获取传感器数据
//  @param      void
//  @return     void
//  @since      v1.0
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void get_sensor_data(void)
{
    int16 temp;
    temp =  timer_quad_get(TIMER_3);
    timer_quad_clear(TIMER_3);

    if(temp < 0) temp = -temp;              //强制取正
    if(gpio_get(B5) == 1) slave_encoder_left = temp;       //编码器方向引脚为高电平，获取到的值为正
    else   slave_encoder_left = -temp;      //编码器方向引脚为低电平，获取到的值为负

    temp =  timer_quad_get(TIMER_2);
    timer_quad_clear(TIMER_2);

    if(temp < 0) temp = -temp;              //强制取正
    if(gpio_get(B3) == 0) slave_encoder_right = temp;       //编码器方向引脚为高电平，获取到的值为正
    else   slave_encoder_right = -temp;      //编码器方向引脚为低电平，获取到的值为负


    if (fork_flag==2)
    {
        slave_error_val = 0x0fff;
        fork_flag = 0;
    } else
    {
        slave_error_val = Cam_Error;
    }

    slave_controlled_val = 0;
}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      根据协议处理数据
//  @param      void
//  @return     void
//  @since      v1.0
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void process_data(void)
{
    TxBuff[0] = 0Xd8;                         //帧头
    TxBuff[1] = 0Xb0;                         //功能字
    TxBuff[2] = slave_encoder_left>>8;        //数据高8位
    TxBuff[3] = slave_encoder_left&0Xff;      //数据低8位

    TxBuff[4] = 0Xb1;                         //功能字
    TxBuff[5] = slave_encoder_right>>8;       //数据高8位
    TxBuff[6] = slave_encoder_right&0Xff;     //数据低8位

    TxBuff[7] = 0Xb2;                         //功能字
    TxBuff[8] = slave_error_val>>8;             //数据高8位
    TxBuff[9] = slave_error_val&0Xff;           //数据低8位

    TxBuff[10] = 0Xb3;                         //功能字
    TxBuff[11] = slave_controlled_val>>8;      //数据高8位
    TxBuff[12] = slave_controlled_val&0Xff;    //数据低8位

    TxBuff[13] = 0Xee;                        //帧尾
}
