/**
  ******************************************************************************
  * @file    useri2c.h
  * @author  kevin_guo
  * @version V1.0.0
  * @date    12-15-2018
  * @brief   This file contains definitions for useri2c.c
  ******************************************************************************
  * @attention
  ******************************************************************************
  */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __USERI2C_H
#define __USERI2C_H
#include "main.h"
extern uint16_t  distance;
/* Exported types ------------------------------------------------------------*/
/* Exported constants --------------------------------------------------------*/
/* Exported macros -----------------------------------------------------------*/
//i2c device address
#define I2CDEVID 0xA4

#define SDA_PIN     C7
#define SCL_PIN     B12
//IO方向设置
#define SDA_SET_IN()  {gpio_init(SDA_PIN,GPI,1,IN_PULLUP);}
#define SDA_SET_OUT() {gpio_init(SDA_PIN,GPO,1,GPIO_PIN_CONFIG);}

//IO操作函数
#define SCL_OUT_L    gpio_set(SCL_PIN,0) //SCL=0
#define SCL_OUT_H    gpio_set(SCL_PIN,1) //SCL=1
#define SDA_OUT_L    gpio_set(SDA_PIN,0) //SDA=0
#define SDA_OUT_H    gpio_set(SDA_PIN,1) //SDA=1
#define SDA_READ_VAL gpio_get(SDA_PIN)  	//输入SDA
/* Exported functions ------------------------------------------------------- */
void UserI2c_Init(void);
void UserI2c_Start(void);
void UserI2c_Stop(void);
unsigned char UserI2c_Wait_Ack(void);
void UserI2c_Send_Byte(u8 txd);
unsigned char UserI2c_Read_Byte(void);
unsigned char UserI2c_WritenByte(unsigned char *txbuf, unsigned char startaddr, unsigned char cnt);
unsigned char UserI2c_ReadnByte(unsigned char *rxbuf, unsigned char startaddr, unsigned char cnt);

void Sensor_Get_Range(void);
void Sensor_Get_AveRange(void);
void Sensor_Get_SendDir(void);
void Sensor_Get_filter(void);
void Sensor_Get_I2cAddr(void);
void Sensor_Get_Offset(void);

void Sensor_Set_SendDir(void);
void Sensor_Set_filter(void);
void Sensor_Set_UartInterval(void);
void Sensor_Set_MaxDistance(void);
void Sensor_Set_I2cAddr(void);
void Sensor_Set_Offset(void);
#endif /* __USERI2CIIC_H */

/************************END OF FILE*************************/

