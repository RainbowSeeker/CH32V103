/*
 * m 6ain.c
 *
 *  Created on: Jan 14, 2021
 *      Author: YY
 */
#include "../CODE/main.h"

#define WAIT_FOR_IMAGE()    \
mt9v03x_finish_flag=0;\
while (mt9v03x_finish_flag==0);


int main(void)
{
    DisableGlobalIRQ();
    board_init();   //��ر��������������ڳ�ʼ��MPU ʱ�� ���Դ���
    Slave_All_Init();
    EnableGlobalIRQ(0);
    lcd_showstr(0, 0, "dis:");
    lcd_showstr(0, 1, "cam:");
    while (1)
    {

        //�ȴ�һ֡ͼ��ɼ����
        WAIT_FOR_IMAGE();
        Cam_Handle_Loop(mt9v03x_image);
        lcd_showuint16(50, 0, distance);
        //        lcd_showuint16(90,5,slave_controlled_val);
        lcd_showint16(50,1,Cam_Error);
        lcd_showint16(0,6,data_return);
        lcd_showint16(0,7,slave_encoder_left);
        lcd_showint16(80,7,slave_encoder_right);
        //��ʾ��ֵ��ͼ��
//        lcd_displayimage032_zoom(mt9v03x_image[0],width,height,160-1,height-1,Sepval);
//        //����
//        for(uint8 t=height-1;t>=SepLine;t--)
//        {
//            if (Line[t].Midpoint<0) Line[t].Midpoint=0;
//            else if (Line[t].Midpoint>width-1) Line[t].Midpoint=width-1;
//            lcd_drawpoint(Line[t].Midpoint*159/187,t,RED);
//        }
//        for(uint8 t=height-1;t>=SepLine;t--)
//        {
//            if (Line[t].Lefborder<0) Line[t].Lefborder=0;
//            else if (Line[t].Lefborder>width-1) Line[t].Lefborder=width-1;
//            lcd_drawpoint(Line[t].Lefborder*159/187, t, BLUE);
//        }
//        for(uint8 t=height-1;t>=SepLine;t--)
//        {
//            if (Line[t].Rigborder<0) Line[t].Rigborder=0;
//            else if (Line[t].Rigborder>width-1) Line[t].Rigborder=width-1;
//            lcd_drawpoint(Line[t].Rigborder*159/187, t, PURPLE);
//        }
    }

}
void Slave_All_Init()
{
    Com_Init();
    delay_ms(300);
    lcd_init();
    lcd_showstr(0,0,"Initializing...");
    //adc_Init();
    Encoder_Init();
    MPU_Init();
    Camera_Init();
    timer_pit_interrupt_ms(TIMER_4,5);
    nvic_init(TIM4_IRQn,0,0,ENABLE);
    UserI2c_Init();
    lcd_showstr(0,1,"     OK...     ");
    lcd_clear(WHITE);
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
uint16 GetData_TOF_IIC()
{
    int dis;
    uint8_t rx[2];
    UserI2c_ReadnByte(rx,0x00,2);
    dis=rx[0]*256+rx[1];
    if (dis>=32767) dis = 32767;
    return dis;
}










