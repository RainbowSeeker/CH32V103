//
// Created by ���� on 2021/5/17.
//

#include "buzzer.h"


//�ⲿʹ�ñ���,�ı����beep��Ӧ����
uint16 beep_time = 0;


//�ڲ���������
uint16 last_time = 0;
void Buzzer_Init()
{
    gpio_init(BEEP_PIN,GPO,0,GPIO_PIN_CONFIG);
}

void Beep()
{
    if (beep_time)
    {
        if (beep_time>last_time)    gpio_set(BEEP_PIN,1);
        beep_time--;
        if (0==beep_time)   gpio_set(BEEP_PIN,0);
        last_time = beep_time;
    }
}





