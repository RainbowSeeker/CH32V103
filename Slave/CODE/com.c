//
// Created by ���� on 2021/3/30.
//

#include "com.h"
#include "cam.h"

uint8 RxBuff[LINE_LEN];    //�������ڽ������ݵ�BUFF
uint8 TxBuff[LINE_LEN];


vuint8  Rx_Flag=0;
int16 data_return = 0;//��������Ҫ�󷵻���Ӧֵ

int16 slave_encoder_left=0;               //�ӻ��������ֵ
int16 slave_encoder_right=0;              //�ӻ��ұ�����ֵ
int16 slave_error_val=0;
int16 slave_controlled_val=0;             //����Ҫ��ֵ


Motor motor[4]={

        {Lef_Up,PWM4_CH3_B8,C2,TIMER_3,TIMER3_CHA_B4 ,TIMER3_CHB_B5,0000,0},
        {Rig_Up,PWM4_CH4_B9,C3,TIMER_2,TIMER2_CHA_A15,TIMER2_CHB_B3,0000,0},
        {Rig_Down,PWM4_CH2_B7,C1,TIMER_3,TIMER3_CHA_B4 ,TIMER3_CHB_B5,0000,0},
        {Lef_Down,PWM4_CH1_B6,C0,TIMER_2,TIMER2_CHA_A15,TIMER2_CHB_B3,0000,0},

        };
void Encoder_Init()
{
    timer_quad_init(motor[2].timern,motor[2].phaseA,motor[2].phaseB);
    timer_quad_init(motor[3].timern,motor[3].phaseA,motor[3].phaseB);
}
void Com_Init()
{
    gpio_init(B9,GPO,1,GPIO_PIN_CONFIG);
    spi_init(SPI_2,SPI2_SCK_PB13,SPI2_MOSI_PB15,SPI2_MISO_PB14,C5,0,FPCLK_DIV_128);
}
//-------------------------------------------------------------------------------------------------------------------
//  @brief      ��ȡ�ӻ�����
//  @param      data            ��������
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
        //���յ��ĵ�һ���ַ���Ϊ0Xd8��֡ͷ����
        if(0Xd8 != RxBuff[0])
        {
            RxCount = 0;
        }
    }
    if(LINE_LEN == RxCount)
    {
        //���յ����һ���ֽ�Ϊ0Xee
        if(0Xee == RxBuff[LINE_LEN-1])
        {
            Rx_Flag = E_OK;
        }
        RxCount = 0;
    }
}
//------------------------------------------------------------------------------------------------------------------
//  @brief      ����Э��Դӻ����͹��������ݣ��������ݽ���
//  @param      *line                           ���ڽ��յ�������BUFF
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
//  @brief      ��ȡ����������
//  @param      void
//  @return     void
//  @since      v1.0
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void get_sensor_data(void)
{
    int16 temp;
    temp =  timer_quad_get(TIMER_2);
    timer_quad_clear(TIMER_2);

    if(temp < 0) temp = -temp;              //ǿ��ȡ��
    if(gpio_get(B3) == 0) slave_encoder_left = -temp;       //��������������Ϊ�ߵ�ƽ����ȡ����ֵΪ��
    else   slave_encoder_left = temp;      //��������������Ϊ�͵�ƽ����ȡ����ֵΪ��

    temp =  timer_quad_get(TIMER_3);
    timer_quad_clear(TIMER_3);

    if(temp < 0) temp = -temp;              //ǿ��ȡ��
    if(gpio_get(B5) == 1) slave_encoder_right = -temp;       //��������������Ϊ�ߵ�ƽ����ȡ����ֵΪ��
    else   slave_encoder_right = temp;      //��������������Ϊ�͵�ƽ����ȡ����ֵΪ��



    slave_error_val = Cam_Error;

    if (0xff==data_return)
    {
        slave_controlled_val = mpu.gyro_z;
    }
    else
    {
        if (fork_flag==1)
        {
            slave_controlled_val = 0x0fff;
        }else if (fork_flag==2)
        {
            slave_controlled_val = 0x1fff;
            fork_flag = 0;
        }
        else if (data_return==0&&distance<=10000)
        {
            slave_controlled_val = 0x2fff;
        }else
        {
            slave_controlled_val = 0;
        }
    }



}

//-------------------------------------------------------------------------------------------------------------------
//  @brief      ����Э�鴦������
//  @param      void
//  @return     void
//  @since      v1.0
//  Sample usage:
//-------------------------------------------------------------------------------------------------------------------
void process_data(void)
{
    TxBuff[0] = 0Xd8;                         //֡ͷ
    TxBuff[1] = 0Xb0;                         //������
    TxBuff[2] = slave_encoder_left>>8;        //���ݸ�8λ
    TxBuff[3] = slave_encoder_left&0Xff;      //���ݵ�8λ

    TxBuff[4] = 0Xb1;                         //������
    TxBuff[5] = slave_encoder_right>>8;       //���ݸ�8λ
    TxBuff[6] = slave_encoder_right&0Xff;     //���ݵ�8λ

    TxBuff[7] = 0Xb2;                         //������
    TxBuff[8] = slave_error_val>>8;             //���ݸ�8λ
    TxBuff[9] = slave_error_val&0Xff;           //���ݵ�8λ

    TxBuff[10] = 0Xb3;                         //������
    TxBuff[11] = slave_controlled_val>>8;      //���ݸ�8λ
    TxBuff[12] = slave_controlled_val&0Xff;    //���ݵ�8λ

    TxBuff[13] = 0Xee;                        //֡β
}
