/*
 * main.c
 *
 *  Created on: Jan 14, 2021
 *      Author: YY
 */
#include "../CODE/main.h"

run_mode_t runMode;
int main(void)
{
    DisableGlobalIRQ();
    board_init();   //��ر��������������ڳ�ʼ��MPU ʱ�� ���Դ���
    Master_All_Init();
    EnableGlobalIRQ(0);
    share_hand();
    while (1)
    {
        runMode.loop();
    }

}
void normloop()
{
    //�ȴ�һ֡ͼ��ɼ����
    WAIT_FOR_IMAGE();
    //ͼ����
    Cam_Handle_Loop();
    //��ȡ������
    Chassis_Control_Swtich();
    //���
    Check_Error();
    //������λ
    Para_Reset();
    //��ӡ����
    Print_Data();
}
void funloop()
{
    int32_t dutysum = 0;
    for (uint8_t t = 0; t < 4; ++t) {
        dutysum += ABS(motor[t].duty);
    }
    if (dutysum >= 30000)
    {
        while (1)
        {
            for (uint8_t t = 0; t < 4; ++t) {
                Setduty(&motor[t],0);
            }
        }
    }
    delay_ms(1);
}
void Master_All_Init()
{
    delay_ms(300);  //��ʱ300ms���ȴ��������������ϵ�ɹ�
    Buzzer_Init();
    gpio_init(B2,GPI,1,IN_PULLUP);

    runMode.choose  = !gpio_get(B2);
    if (runMode.choose)
    {
        runMode.loop = funloop;
        mychassis.funny = runMode.choose;
        mychassis.chassis_loop = Chassis_FunnyPIT;

        lcd_init(); //��ʼ��TFT��Ļ
        lcd_showstr(0,0,"Initializing...");
        Encoder_Init();
        PID_Init();
        Chassis_Init(&mychassis);
        lcd_clear(WHITE);
        lcd_showstr(50,1,"Funny Mode");
        data_return = 0xff;
    } else      //����ģʽ
    {
        runMode.loop = normloop;
        mychassis.funny = runMode.choose;
        mychassis.chassis_loop = Chassis_NormPIT;

        gpio_init(B1,GPI,1,IN_PULLUP);
        into_gar                        = !gpio_get(B1);
        lcd_init(); //��ʼ��TFT��Ļ
        lcd_showstr(0,0,"Initializing...");
        Encoder_Init();
        Flash_Init();
        Camera_Init();
        Menu_Init();//warning:menu��ʼ��������pid,chassis,share_hand֮ǰ
        PID_Init();
        Chassis_Init(&mychassis);
        lcd_showstr(0,1,"     OK...     ");
        lcd_clear(WHITE);
        if (flash.two)
        {
            remote_radio = 16;
        }
    }
}
void vcan_sendware(void *wareaddr, uint32_t waresize)
{
#define CMD_WARE     3
    uint8_t cmdf[2] = {CMD_WARE, ~CMD_WARE};    //���ڵ��� ʹ�õ�ǰ����
    uint8_t cmdr[2] = {~CMD_WARE, CMD_WARE};    //���ڵ��� ʹ�õĺ�����

    uart_putbuff(UART_1, cmdf, sizeof(cmdf));    //�ȷ���ǰ����
    uart_putbuff(UART_1, (uint8_t *)wareaddr, waresize);    //��������
    uart_putbuff(UART_1, cmdr, sizeof(cmdr));    //���ͺ�����

}
void Print_Data()
{
//    lcd_showint16(0,1,Track_Type);
//        lcd_showuint16(0,0,TIM4->CNT);
//        TIM4->CNT = 0;

//    lcd_showuint16(0,0,mychassis.rotate.rotime);
//    lcd_showint16(0 ,6,Cam_Error);
//    int16 data[4];
//    for (uint8 t = 0; t < 4; ++t)
//    {
//        data[t] = motor[t].nowspeed;
//    }
//    data[0] = mychassis.wheel[0].speed_set;
//    data[1] = mychassis.wheel[1].speed_set;
//    data[2] = mychassis.wheel[2].speed_set;
//    data[3] = mychassis.wheel[3].speed_set;
//    vcan_sendware(data,sizeof(data));
//    for (uint8 t = 0; t < 4; ++t)
//    {
//        lcd_showint16(0,t,motor[t].nowspeed);
////        lcd_showint16(80,t,mychassis.wheel[t].speed_set);
////        lcd_showint16(0,t+4,mychassis.wheel[t].speed_pid.result);
//    }
}
int max(int x,int y)
{
    if (x>y)    return x;
    else return y;
}
int min(int x,int y)
{
    if (x<y)    return x;
    else return y;
}
/*!
 * ������ӻ�������������
 */
void share_hand()
{
    beep_time = 5;
    delay_ms(100);
    Com_Init();
    Motor_Init();
}









