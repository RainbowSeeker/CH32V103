/*
 * adc.c
 *
 *  Created on: Jan 17, 2021
 *      Author: YY
 */
#include "adc.h"

ADC adc[5]={{ADC_IN0_A0,0},
            {ADC_IN4_A4,0},
            {ADC_IN15_C5,0},
            {ADC_IN14_C4,0},
            {ADC_IN8_B0,0}};

void adc_Init()
{
//    for (uint8 t = 0; t < 5; ++t)
//    {
        adc_init(adc[0].channel);
        adc_init(adc[4].channel);
//    }
}

void ADC_GetAllValue()
{
//    for (uint8 t = 0; t < 5; ++t)
//    {
        adc[0].lastvalue=adc_mean_filter(adc[0].channel,ADC_CONVERT_BIT,ADC_TIMES_ONCE);
        adc[4].lastvalue=adc_mean_filter(adc[4].channel,ADC_CONVERT_BIT,ADC_TIMES_ONCE);
//    }
    adc[0].lastvalue = adc[0].lastvalue*300/4095;
    adc[4].lastvalue = adc[4].lastvalue*300/4095;
}

void ADC_ProcessError(int16 *error)
{
    *error=(adc[0].lastvalue-adc[4].lastvalue);
}
