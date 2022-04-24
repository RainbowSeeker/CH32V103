/*
 * adc.h
 *
 *  Created on: Jan 17, 2021
 *      Author: YY
 */

#ifndef USER_ADC_H_
#define USER_ADC_H_
#include "main.h"

#define ADC_CONVERT_BIT ADC_12BIT   //adc采集精度
#define ADC_TIMES_ONCE  8           //一次采8次
typedef struct
{
    ADCCH_enum  channel;
    uint16      lastvalue;
}ADC;

extern ADC adc[5];




void adc_Init();
void ADC_GetAllValue();
void ADC_ProcessError(int16 *error);
#endif /* USER_ADC_H_ */
