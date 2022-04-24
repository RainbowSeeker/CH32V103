//
// Created by 杨宇 on 2021/5/17.
//

#include "buzzer.h"


//外部使用变量,改变则可beep对应周期
uint16 beep_time = 0;


//内部计数变量
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





