/**
  ******************************************************************************
  * @file    useri2c.c
  * @author  kevin_guo
  * @version V1.0.0
  * @date    12-15-2018
  * @brief   This file provides functions to useri2c
  ******************************************************************************
  * @attention
  ******************************************************************************
  */

/* Includes ------------------------------------------------------------------*/
#include "useri2c.h"
uint16_t  distance=0;
/* extern variables ----------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
unsigned char i2caddr,i2caddr_set;
unsigned short int range_val;
unsigned short int averange_val;
unsigned char senddir_flag;
unsigned char filter_flag;
unsigned short int uart_interval;
unsigned short int maxdistance;
signed short offset;
/* Private function prototypes -----------------------------------------------*/
void User_DelayuS(unsigned int nCount);
/* Private function ----------------------------------------------------------*/
/*******************************************************************************
* Function Name  : User_DelayuS
* Description    : Inserts a delay time.
* Input          : nCount: the delay time =nCount*basetime.
* Output         : None
* Return         : None
*******************************************************************************/
void User_DelayuS(unsigned int nCount)
{
	unsigned short int nopcnt;

  for(; nCount != 0; nCount--)
	{
		nopcnt=14;//=14 i2c sclk=100kHz
		while(nopcnt--)
		{
			asm("nop");
		}
	}
}
/*******************************************************************************
* Function Name  : UserI2c_Start
* Description    : sck stable in H,sda falling edge
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UserI2c_Start(void)
{
	SDA_SET_OUT();     //sda�����
	SDA_OUT_H;
	User_DelayuS(1);
	SCL_OUT_H;
	User_DelayuS(3);
 	SDA_OUT_L;//START:when CLK is high,DATA change form high to low
	User_DelayuS(3);
	SCL_OUT_L;//ǯסI2C���ߣ�׼�����ͻ��������
}
/*******************************************************************************
* Function Name  : UserI2c_Stop
* Description    : sck stable in H,sda rising edge
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UserI2c_Stop(void)
{
	SCL_OUT_L;
	SDA_SET_OUT();//sda�����
	SDA_OUT_L;//STOP:when CLK is high DATA change form low to high
 	User_DelayuS(10);
	SCL_OUT_H;
 	User_DelayuS(10);
	SDA_OUT_H;//����I2C���߽����ź�
	User_DelayuS(10);
}
/*******************************************************************************
* Function Name  : UserI2c_Wait_Ack
* Description    : the 9th clock pulse period wait ack
* Input          : None
* Output         : None
* Return         : =0��ack
*								 : =1��ack
*******************************************************************************/
unsigned char UserI2c_Wait_Ack(void)
{
	unsigned short int ucErrTime=0;
  unsigned char RetValue;

	SDA_SET_IN();      //SDA����Ϊ����
	SDA_OUT_H;
	SCL_OUT_L;
	User_DelayuS(1);
	SCL_OUT_H;
  ucErrTime = 10000;
  while( ucErrTime-- > 0 )
  {
    if(SDA_READ_VAL )
    {
      RetValue = 0;
    }
    else
    {
      RetValue = 1; //SDA�͵�ƽ
			break;
    }
  }
	User_DelayuS(1);
	SCL_OUT_L;//ʱ�����0
	SDA_SET_OUT();//sda�����
	SDA_OUT_L;
	User_DelayuS(1);
	return RetValue;
}
/*******************************************************************************
* Function Name  : UserI2c_Send_Ack
* Description    : the 9th clock pulse period, the receiver pulls sda low to
* 							 : acknowledge the receipt of the eight data bits.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UserI2c_Send_Ack(void)
{
	SCL_OUT_L;
	SDA_SET_OUT();
	SDA_OUT_L;
	User_DelayuS(1);
	SCL_OUT_H;
	User_DelayuS(1);
	SCL_OUT_L;
}
/*******************************************************************************
* Function Name  : UserI2c_Send_Nack
* Description    : no acknowledge the receipt of the eight data bits.
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UserI2c_Send_Nack(void)
{
	SCL_OUT_L;
	SDA_SET_OUT();
	SDA_OUT_H;
	User_DelayuS(1);
	SCL_OUT_H;
	User_DelayuS(1);
	SCL_OUT_L;
}
/*******************************************************************************
* Function Name  : UserI2c_Send_Byte
* Description    : write 1 byte data to i2c bus
* Input          : txdata-data
* Output         : None
* Return         : None
*******************************************************************************/
void UserI2c_Send_Byte(unsigned char txdata)
{
	unsigned char t;

	SDA_SET_OUT();
	SCL_OUT_L;//����ʱ�ӿ�ʼ���ݴ���
	User_DelayuS(10);
	for(t=0;t<8;t++)
	{
		if(txdata&0x80)
			SDA_OUT_H;
		else
			SDA_OUT_L;
		txdata<<=1;
		User_DelayuS(2);
		SCL_OUT_H;
		User_DelayuS(2);
		SCL_OUT_L;
	}
}
/*******************************************************************************
* Function Name  : UserI2c_Read_Byte
* Description    : read 1 byte data from i2c bus
* Input          : None
* Output         : None
* Return         : receoved data
*******************************************************************************/
unsigned char UserI2c_Read_Byte(void)
{
	unsigned char i,receive=0;

	SDA_SET_IN();//SDA����Ϊ����
	SDA_OUT_H;
	SCL_OUT_L;
	User_DelayuS(30);
	for(i=0;i<8;i++ )
	{
		receive<<=1;
		SCL_OUT_H;
		User_DelayuS(3);
		if(SDA_READ_VAL)
			receive++;
		SCL_OUT_L;
		User_DelayuS(3);
	}
	SCL_OUT_L;

	return receive;
}

/*******************************************************************************
* Function Name  : SensorWritenByte
* Description    : Sensor read api.
* Input          : *txbuf-the buffer which write to
*                : startaddr-read start address
*                : cnt-write data size
* Output         : None
* Return         : None
*******************************************************************************/
unsigned char UserI2c_WritenByte(unsigned char *txbuf, unsigned char startaddr, unsigned char cnt)
{
	unsigned char i = 0;

	UserI2c_Start();
  UserI2c_Send_Byte( i2caddr|0x00 );
  if( RESET == UserI2c_Wait_Ack() )
  {
		UserI2c_Stop();
    return RESET;
  }
  UserI2c_Send_Byte(startaddr);
  if( RESET == UserI2c_Wait_Ack() )
  {
		UserI2c_Stop();
    return RESET;
  }
	for ( i = 0; i < cnt; i++)
	{
		UserI2c_Send_Byte(txbuf[cnt-i-1]);
		if(RESET == UserI2c_Wait_Ack())
		{
			UserI2c_Stop();
			return RESET;
		}
	}
  UserI2c_Stop();

  return SET;
}
/*******************************************************************************
* Function Name  : SensorReadnByte
* Description    : Sensor read api.
* Input          : *rxbuf-the buffer which stores read content.
*                : startaddr-the address read start
*                : cnt-read data size
* Output         : None
* Return         : None
*******************************************************************************/
unsigned char UserI2c_ReadnByte(unsigned char *rxbuf, unsigned char startaddr, unsigned char cnt)
{
  unsigned char i = 0;

  UserI2c_Start();
  UserI2c_Send_Byte( i2caddr|0x00 );
  if( RESET == UserI2c_Wait_Ack() )
  {
		UserI2c_Stop();
    return RESET;
  }
  UserI2c_Send_Byte(startaddr);
  if( RESET == UserI2c_Wait_Ack() )
  {
		UserI2c_Stop();
    return RESET;
  }
	UserI2c_Stop();
  UserI2c_Start();
  UserI2c_Send_Byte( i2caddr|0x01 );
  if( RESET == UserI2c_Wait_Ack() )
  {
		UserI2c_Stop();
    return RESET;
  }
	for ( i = 0; i < cnt; i++)
	{
		rxbuf[cnt-i-1] = UserI2c_Read_Byte();
		if((i+1)==cnt)
			UserI2c_Send_Nack();
		else
			UserI2c_Send_Ack();
	}
  UserI2c_Stop();

  return SET;
}

/*******************************************************************************
* Function Name  : UserI2c_Init
* Description    : i2c��ʼ��
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void UserI2c_Init(void)
{
    gpio_init(SDA_PIN,GPO,1,GPIO_PIN_CONFIG);
    gpio_init(SCL_PIN,GPO,1,GPIO_PIN_CONFIG);
	i2caddr=I2CDEVID;
}
/*******************************************************************************
* Function Name  : Sensor_Get_Range
* Description    : ��ȡʵʱ����
* Input          : None
* Output         : averange_val
* Return         : None
*******************************************************************************/
void Sensor_Get_Range(void)
{
	unsigned short int read_val;
	if(SET==UserI2c_ReadnByte((unsigned char *)&read_val, 0x00,2))
	{
		range_val=read_val;
	}
}
/*******************************************************************************
* Function Name  : Sensor_Get_AveRange
* Description    : ��ȡ�˲�����
* Input          : None
* Output         : averange_val
* Return         : None
*******************************************************************************/
void Sensor_Get_AveRange(void)
{
	unsigned short int read_val;
	if(SET==UserI2c_ReadnByte((unsigned char *)&read_val, 0x04,2))
	{
		averange_val=read_val;
	}
}
/*******************************************************************************
* Function Name  : Sensor_Get_SendDir
* Description    : ��ȡ�������ͻ��Ǳ�����ȡ
* Input          : senddir_flag
* Output         : None
* Return         : None
*******************************************************************************/
void Sensor_Get_SendDir(void)
{
	UserI2c_ReadnByte((unsigned char *)&senddir_flag, 0x09,1);
}
/*******************************************************************************
* Function Name  : Sensor_Get_filter
* Description    : ��ȡ�ľ��������Ƿ��˲�
* Input          : senddir_flag
* Output         : None
* Return         : None
*******************************************************************************/
void Sensor_Get_filter(void)
{
	UserI2c_ReadnByte((unsigned char *)&filter_flag, 0x08,1);
}
/*******************************************************************************
* Function Name  : Sensor_Get_MaxDistance
* Description    : ��ȡ���õ�����������
* Input          : maxdistance
* Output         : None
* Return         : None
*******************************************************************************/
void Sensor_Get_MaxDistance(void)
{
	UserI2c_ReadnByte((unsigned char *)&maxdistance, 0x0c,2);
}
/*******************************************************************************
* Function Name  : Sensor_Get_Offset
* Description    : ��ȡ���õľ���ƫ��
* Input          : offset
* Output         : None
* Return         : None
*******************************************************************************/
void Sensor_Get_Offset(void)
{
	UserI2c_ReadnByte((unsigned char *)&offset, 0x06,2);
}
/*******************************************************************************
* Function Name  : Sensor_Get_I2cAddr
* Description    : ��ȡI2C��ַ
* Input          : i2caddr
* Output         : None
* Return         : None
*******************************************************************************/
void Sensor_Get_I2cAddr(void)
{
	UserI2c_ReadnByte((unsigned char *)&i2caddr, 0x0f,1);
}
/*******************************************************************************
* Function Name  : Sensor_Set_SendDir
* Description    : �����������ͻ��Ǳ�����ȡ
* Input          : senddir_flag
* Output         : None
* Return         : None
*******************************************************************************/
void Sensor_Set_SendDir(void)
{
	UserI2c_WritenByte((unsigned char *)&senddir_flag, 0x09,1);
}
/*******************************************************************************
* Function Name  : Sensor_Set_filter
* Description    : ���ö�ȡ�ľ��������Ƿ��˲�
* Input          : filter_flag
* Output         : None
* Return         : None
*******************************************************************************/
void Sensor_Set_filter(void)
{
	UserI2c_WritenByte((unsigned char *)&filter_flag, 0x08,1);
}
/*******************************************************************************
* Function Name  : Sensor_Set_MaxDistance
* Description    : ��������������
* Input          : maxdistance
* Output         : None
* Return         : None
*******************************************************************************/
void Sensor_Set_MaxDistance(void)
{
	UserI2c_WritenByte((unsigned char *)&maxdistance, 0x0c,2);
}
/*******************************************************************************
* Function Name  : Sensor_Set_Offset
* Description    : ���õľ���ƫ��
* Input          : offset
* Output         : None
* Return         : None
*******************************************************************************/
void Sensor_Set_Offset(void)
{
	UserI2c_WritenByte((unsigned char *)&offset, 0x06,2);
}
/*******************************************************************************
* Function Name  : Sensor_Set_I2cAddr
* Description    : ����I2C��ַ
* Input          : i2caddr
* Output         : None
* Return         : None
*******************************************************************************/
void Sensor_Set_I2cAddr(void)
{
	UserI2c_WritenByte((unsigned char *)&i2caddr_set, 0x0f,1);
}
/************************END OF FILE*************************/
