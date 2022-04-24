/*
 * encoder.h
 *
 *  Created on: Jan 17, 2021
 *      Author: YY
 */

#ifndef USER_ENCODER_H_
#define USER_ENCODER_H_
#include "headfile.h"
#include "image_process.h"

#define STOP_TIME                   5
#define POS_TIME                    10

#define ADD_SPEED                   30
#define ADD_MAXSP                   10

#define NORMAL_SPEED_VX             80
#define NORMAL_SPEED_VY             160


#define MAX_WHEEL_SPEED             250 //最大轮子转速,非底盘速度！！！
#define DEFAULT_FOLLOW_MODE         CHASSIS_FOLLOW_CAM
#define DEFAULT_MOVE_MODE           STRAIGHT_RUN    //STRAIGHT_RUN  ACROSS_THE_WALK

#define MOTOR_SPEED_KP              160
#define MOTOR_SPEED_KI              18

#define NORMAL_DIR_KP                16  //36
#define NORMAL_DIR_KD                8  //15

#define RAMP_DIR_KP                  20
#define RAMP_DIR_KD                  5

#define FORK_DIR_KP                  12
#define FORK_DIR_KD                  8

#define WZ_LIMIT                     50 //偏差大于此限度时起作用(必须大于0)
#define CHASSIS_WZ_SET_SACLE          0 //速度快时用于减轻甩尾，慢速一般不用

#define INGAR_ROTIME                 13
#define INGAR_GOTIME                 18


//---------电机控制区-------------------------------------
typedef enum
{
    Lef_Up  =1,
    Rig_Up  =2,
    Lef_Down=3,
    Rig_Down=4,
}Wheel_No;
typedef struct
{
    Wheel_No        no;
    PWMCH_enum      channel; //电机控制通道
    PIN_enum        dirpin; //电机旋转方向控制角
    TIMERN_enum     timern;
    TIMER_PIN_enum  phaseA;
    TIMER_PIN_enum  phaseB;
    uint32          duty;
    int16           nowspeed;
}Motor;

extern Motor motor[4];


void Encoder_Init();
void Motor_Init();
void Setduty(Motor *m,int32 duty);
void GetWheelSpeed(Motor *wheel);
//---------低通滤波器区-------------------------------------
typedef struct
{
    int16 input;        //输入数据
    int16 output;          //滤波输出的数据
    int16 ratio;       //滤波参数
} Filter_Info;
void First_Order_Low_Pass_Filter(Filter_Info *first_order_filter_type, int16 input);
//--------- P I D 区 -------------------------------------
typedef enum
{
    PID_POSITION = 0x00,
    PID_INCREMENT,
}PID_MODE;
/*定义结构体和公用体*/
typedef struct
{
    PID_MODE mode;
    int16 Kp;             //比例系数
    int16 Ki;             //积分系数
    int16 Kd;             //微分系数
    int16 lasterror;      //前一拍偏差
    int16 preerror;       //前两拍偏差
    int16 epsilon;        //偏差检测阈值
//    int16 umax;           //最大值
//    int16 umin;           //最小值
    int32 integral;       //积分值
    int32 result;         //输出值
}PID;

void PID_Init();
void PID_Realize(PID *vPID, int16 processValue,int16 setpoint);
void Clear_PID_Data(PID *vpid);
//---------底盘控制区-----------------------------------
typedef struct
{
    Wheel_No    no;
    int16       speed_set;
    PID         speed_pid;
}wheel_info_t;

typedef enum
{
    CHASSIS_NO_FOLLOW=0,                      	//底盘不跟随
    CHASSIS_FOLLOW_CAM=1,					    //底盘跟随摄像头
    CHASSIS_FOLLOW_ADC=2,                       //底盘跟随电感
    CHASSIS_FOLLOW_ALL=3,                       //底盘共同跟随
} Chassis_Follow_Mode;

typedef enum
{
    STRAIGHT_RUN=0,
    NO_MOVE=1,
    ACROSS_THE_WALK=2,
}Chassis_Move_Mode;

typedef enum
{
    FIRST   = 0,
    SECOND  = 1
}Cycle_Time;
//旋转方向
typedef enum
{
    CLOCK_WISE      = 0,
    ANTI_CLOCK_WISZ = 1,
}Rotate_Dir;
//旋转策略
typedef enum
{
    USE_TIMER   = 0,
    USE_GYRO    = 1,
}Plan_Of_Rotate;
//定义旋转参数结构体
typedef struct
{
//    Rotate_Dir      dir;
//    Plan_Of_Rotate  cplan;
    uint16          rotime;
    int16           rospeed;
}rotate_t;

typedef struct
{
    Side   dir;
    uint16 time;
    int16  speed;
}garage_t;

typedef struct chassis_info
{
    uint8_t funny;
    void (*chassis_loop)(struct chassis_info *loop);

    Chassis_Follow_Mode   follow_mode;
    int16 (*follow_control)(struct chassis_info *control);

    Chassis_Move_Mode     move_mode;

    garage_t outGarage;
    garage_t intoGarage;

    int16 vx_set;//X轴平动速度
    int16 vy_set;//Y轴平动速度
    int16 wz_set;//yaw轴自转角速度
    int16 add_wz;


    Cycle_Time cyctime;     //第n圈 n=0/1

    rotate_t rotate;

    wheel_info_t wheel[4];
    PID pid_list[4];
}chassis_info_t;

extern chassis_info_t mychassis;
extern Chassis_Move_Mode chassis_move_mode;
extern uint8 out_gar;
extern uint8 into_gar;
extern uint8 StopReg;
extern uint8 fork_keep;

void Chassis_Init(chassis_info_t *chassis_init);
void Chassis_NormPIT(chassis_info_t *chassis_loop);
void Chassis_FunnyPIT(chassis_info_t *chassis_loop);
void Chassis_Control_Swtich();
//---------------------------------
int16 ABS(int16 input);
#endif /* USER_ENCODER_H_ */
