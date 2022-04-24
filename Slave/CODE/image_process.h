//
// Created by ���� on 2021/4/13.
//
#ifndef SEEKFREE_CH32V103R8T6_OPENSOURCE_LIBRARY_IMAGE_PROCESS_H
#define SEEKFREE_CH32V103R8T6_OPENSOURCE_LIBRARY_IMAGE_PROCESS_H
#include "main.h"

//��������ͷ������Χ��+-range��
#define CAM_ERROR_RANGE                 400
//�������߽���ֵ
#ifdef USE_MASTER
#define ABS_MID                         middle    //����������-40
#else
#define ABS_MID                         middle   //-15
#endif
//�������
#define DEAFULT_GO_DIR                  Left
//��ֵ��������ֵ
#define PartThreshold           0
//-------------------------------------------------------
//�ڲ��궨��ֵ��ȷ���������޸�
//���崦��ͼ��Ĵ�С
#define width                MT9V03X_W
#define height               MT9V03X_H
#define middle                  94
#define LefX_Start              1//1
#define RigX_End                187//width-1
#define excursion               8   //ɨ��߽�ƫ��

#define WAY_OF_SEARCH           1
#define DEAFULT_TRAVERSELINE_MODE  5,width-5,0,3
#define DEAFULT_TRAVERSEROW_MODE   height-5,5,1,10
typedef enum
{
    NORMAL=0,
    CIRCLE=1,
    RAMP  =2,
    FORK  =3,
    CROSS,   //���ҽ�͹����Ϊʮ��
    GARAGE,
}Road_Type;
typedef struct
{
    uint8 line;
    uint8 row;
}Position;
typedef enum
{
    Left=0,
    Right=1,
    Mid=2,
}Side;
typedef struct
{
    int16 slope;  //б��
    int16 intercept; //�ؾ�
}FitLine;
typedef struct
{
    int16  Lefborder;
    int16  Rigborder;
    int16  Midpoint;
}LineInfo;
typedef struct
{
    int8 startline;
    int8 endline;
}LostInfo;
typedef enum
{
    UP  =0,
    DOWN=1,
}Way_Of_MakeUp;
typedef enum
{
    NON = 0,
    WTB = 1,
}Jump_Type;
typedef struct
{
    //�����е�������Ϣ����
    Side        StartSide;     //ʼ��
    uint8       SkipLine;      //����
    uint8       SkipRow;      //����
    Jump_Type   Jump;
    uint8       BLWidth;

    uint8 line;//������������
    uint8 row; //������������
}mwl_config_t;



extern Side Go_Dir;
extern uint8 Sepval;
extern uint8 SepLine;
extern int16 WaitTime;
extern int16 tem_error;

extern uint8  circle_flag;
extern uint8  fork_flag;
extern uint8  ramp_flag;
extern uint8  circle_flag;
extern uint8  garage_flag;

extern Road_Type Track_Type;
extern LostInfo leflost,riglost;
extern LineInfo Line[height];
extern mwl_config_t MaxWhite;


void  Para_Reset();
void Image_Process(uint8 image[height][width]);
void Threshold_Cal();
void TrackScan(uint8 image[height][width]);
void Track_Handle(uint8 image[height][width]);
void Speed_Change();
uint8 IsFork(uint8 image[height][width]);
void IsRamp();
uint8 IsGarage(uint8 image[height][width]);
void IsWind_Circle_Garage(uint8 image[height][width]);
void  GetMidLine();
uint8 GetSepLine(uint8 image[height][width],uint8 stopline);
ErrorStatus GetMaxWhiteRow(uint8 image[height][width],mwl_config_t *config);
uint16 Calculate_Dark(uint8 image[height][width]);
FitLine LinearRegression(int startline,int endline,Side dir);
void Findborder(uint8 image[height][width],uint8 lx,Side side,uint8 mode);
void BorderMakeUp(uint8 startline,uint8 endline,Side dir,Way_Of_MakeUp way);
uint8 TraverseLine(uint8 image[height][width],uint8 lx,int16 startpoint,int16 endpoint,uint8 mode,uint8 num);
uint8 TraverseRow(uint8 image[height][width],uint8 ly,int16 startpoint,int16 endpoint,uint8 mode,uint8 num);
#endif //SEEKFREE_CH32V103R8T6_OPENSOURCE_LIBRARY_IMAGE_PROCESS_H
