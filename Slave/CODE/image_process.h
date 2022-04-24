//
// Created by 杨宇 on 2021/4/13.
//
#ifndef SEEKFREE_CH32V103R8T6_OPENSOURCE_LIBRARY_IMAGE_PROCESS_H
#define SEEKFREE_CH32V103R8T6_OPENSOURCE_LIBRARY_IMAGE_PROCESS_H
#include "main.h"

//定义摄像头所得误差范围（+-range）
#define CAM_ERROR_RANGE                 400
//赛道中线近似值
#ifdef USE_MASTER
#define ABS_MID                         middle    //增加则左移-40
#else
#define ABS_MID                         middle   //-15
#endif
//左出车库
#define DEAFULT_GO_DIR                  Left
//二值化增添阈值
#define PartThreshold           0
//-------------------------------------------------------
//内部宏定义值，确定后无需修改
//定义处理图像的大小
#define width                MT9V03X_W
#define height               MT9V03X_H
#define middle                  94
#define LefX_Start              1//1
#define RigX_End                187//width-1
#define excursion               8   //扫描边界偏移

#define WAY_OF_SEARCH           1
#define DEAFULT_TRAVERSELINE_MODE  5,width-5,0,3
#define DEAFULT_TRAVERSEROW_MODE   height-5,5,1,10
typedef enum
{
    NORMAL=0,
    CIRCLE=1,
    RAMP  =2,
    FORK  =3,
    CROSS,   //左右皆凸出则为十字
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
    int16 slope;  //斜率
    int16 intercept; //截距
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
    //最大白列的配置信息定义
    Side        StartSide;     //始边
    uint8       SkipLine;      //跳行
    uint8       SkipRow;      //跳列
    Jump_Type   Jump;
    uint8       BLWidth;

    uint8 line;//最大白列所处行
    uint8 row; //最大白列所处列
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
