//
// Created by ���� on 2021/4/13.
//
#ifndef SEEKFREE_CH32V103R8T6_OPENSOURCE_LIBRARY_IMAGE_PROCESS_H
#define SEEKFREE_CH32V103R8T6_OPENSOURCE_LIBRARY_IMAGE_PROCESS_H
#include "headfile.h"

//��������ͷ������Χ��+-range��
#define CAM_ERROR_RANGE                 400
//�������߽���ֵ
#define ABS_MID                         middle   //-15
//-------------------------------------------------------
//�ڲ��궨��ֵ��ȷ���������޸�
//���崦��ͼ��Ĵ�С
#define width                MT9V03X_W
#define height               MT9V03X_H
#define middle                  94
#define LefX_Start              1//1
#define RigX_End                187//width-1
#define excursion               8   //ɨ��߽�ƫ��

#define DEAFULT_TRAVERSELINE_MODE  25,width-25,0,5
#define DEAFULT_TRAVERSEROW_MODE   height-5,5,1,10

#define THREAD_NUM          6

typedef enum
{
    CIRCLE  =0,
    GARAGE  =1,
    CROSS   =2,
    FORK    =3,
    RAMP    =4,
    OUTROAD =5,
    NORMAL  =6,
}Road_Type;
typedef enum
{
    NO     = 0,    //no operation
    TS     = 1,    //Thread Succeed
    TO     = 2,    //Thread Over
}ThreadStatus;
typedef enum
{
    False   =0,
    True    =1,
}bool;
typedef struct
{
    bool        FlagOccupy;
    Road_Type   TrackType;
    uint16      WaitTime;    //ͼ��֡���ȴ�

    //ͼ����ѭ��״̬��
    void (*Task_Schedule)(uint8 image[height][width]);

    //���������ʹ���
    ThreadStatus (*TrackHandle)(uint8 image[height][width]);
    //������ָ��
    ThreadStatus (*Schedule[THREAD_NUM])(uint8 image[height][width]);

}img_test_schedule_t;

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
typedef enum
{
    WTB=0,
    BTW=1,
}Jump_Type;
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
typedef struct
{
    //�����е�������Ϣ����
    Side        StartSide;     //ʼ��
    uint8       SkipLine;      //����
    uint8       SkipRow;      //����
    uint8       BLWidth;

    uint8 line;//������������
    uint8 row; //������������
}mwl_config_t;


extern uint8 Sepval;
extern uint8 SepLine;
extern int16 tem_error;

extern uint8  circle_flag;
extern uint8  fork_flag;
extern uint8  ramp_flag;
extern uint8  garage_flag;

extern LostInfo leflost,riglost;
extern LineInfo Line[height];
extern mwl_config_t MaxWhite;
extern img_test_schedule_t order;

void Schedule_Init();
void Task_Schedule(uint8 image[height][width]);
void Clear_All_Flag();
void Para_Reset();
void Image_Process(uint8 image[height][width]);
void Threshold_Cal();
void TrackScan(uint8 image[height][width]);
ThreadStatus IsFork(uint8 image[height][width]);
ThreadStatus IsOut(uint8 image[height][width]);
ThreadStatus IsRamp(uint8 image[height][width]);
ThreadStatus IsCross(uint8 image[height][width]);
ThreadStatus IsGarage(uint8 image[height][width]);
ThreadStatus IsCircle(uint8 image[height][width]);
ThreadStatus Track_Handle(uint8 image[height][width]);
void  GetMidLine();
uint8 GetSepLine(uint8 image[height][width],uint8 stopline);
ErrorStatus GetMaxWhiteRow(uint8 image[height][width],mwl_config_t *config);
uint16 Calculate_Dark(uint8 image[height][width]);
FitLine LinearRegression(int startline,int endline,Side dir);
void Findborder(uint8 image[height][width],uint8 lx,Side side,uint8 mode);
void BorderMakeUp(uint8 startline,uint8 endline,Side dir,Way_Of_MakeUp way);
uint8 TraverseLine(uint8 image[height][width],uint8 lx,int16 startpoint,int16 endpoint,uint8 mode,uint8 num);
uint8 TraverseRow(uint8 image[height][width],uint8 ly,int16 startpoint,int16 endpoint,uint8 mode,uint8 num);
int16 Attr_Error(uint8 image[height][width],uint8 line,int16 prop);
void Fix_Border();
#endif //SEEKFREE_CH32V103R8T6_OPENSOURCE_LIBRARY_IMAGE_PROCESS_H
