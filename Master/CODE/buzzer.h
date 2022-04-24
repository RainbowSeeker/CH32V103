//
// Created by —Ó”Ó on 2021/5/17.
//

#ifndef SEEKFREE_CH32V103R8T6_OPENSOURCE_LIBRARY_BUZZER_H
#define SEEKFREE_CH32V103R8T6_OPENSOURCE_LIBRARY_BUZZER_H
#include "main.h"


#define BEEP_PIN    D2



extern uint16 beep_time;



void Buzzer_Init();
void Beep();

#endif //SEEKFREE_CH32V103R8T6_OPENSOURCE_LIBRARY_BUZZER_H
