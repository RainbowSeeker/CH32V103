/*
 * encoder.c
 *
 *  Created on: Jan 17, 2021
 *      Author: YY
 */

#include "move.h"
#include "com.h"

#define __nop()
#define error()     {while(1){gpio_set(BEEP_PIN,1);delay_ms(200);gpio_set(BEEP_PIN,0);delay_ms(200);}}

#define Stop_Move()             \
{                               \
    for(uint8 t=0;t<4;t++)      \
    {                           \
        Setduty(&motor[t],0);   \
    }                           \
}                               \


int16 deadband_limit(int16 input , int16 dealine)
{
    if (input<= dealine&&input>= -dealine)
    {
        return 0;
    }
    return input;
}



#define Range_Limit(input,bottom,top)   \
    {                                   \
        if((input)>=(top))              \
        {                               \
            (input)=(top);              \
        }                               \
        else if ((input)<=(bottom))     \
        {                               \
            (input)=(bottom);           \
        }                               \
    }                                   \

//--------------------���&������������----------------------------------------
Motor motor[4]={
        //!!!no����мǲ��ɸı䣡����
        {Lef_Up,PWM4_CH4_B9,C3,TIMER_3,TIMER3_CHA_B4 ,TIMER3_CHB_B5,2000,0},
        {Rig_Up,PWM4_CH3_B8,C2,TIMER_2,TIMER2_CHA_A15,TIMER2_CHB_B3,2000,0},
        {Lef_Down,PWM4_CH1_B6,C0,TIMER_2,TIMER2_CHA_A15,TIMER2_CHB_B3,2000,0},
        {Rig_Down,PWM4_CH2_B7,C1,TIMER_3,TIMER3_CHA_B4 ,TIMER3_CHB_B5,2000,0},

        };

void Encoder_Init()
{
    for (uint8 t = Lef_Up-1; t < Rig_Up; ++t)
    {
        timer_quad_init(motor[t].timern,motor[t].phaseA,motor[t].phaseB);
    }
}


void Motor_Init()
{
    for (uint8 t = 0; t < 4; ++t)
    {
        pwm_init(motor[t].channel,13*1000,motor[t].duty);
        gpio_init(motor[t].dirpin,GPO,0,GPIO_PIN_CONFIG);
        if (mychassis.funny)
        {
            Setduty(&motor[t],0);
        }
    }
}
/**
  * @brief          �����ض����ռ�ձ�
  * @param[in]      m: ���ָ��
  * @param[in]      duty: ռ�ձ�
  * @param[out]     NULL
  * @retval         none
  */
void Setduty(Motor *m,int32 duty)
{
    if (m->no==Lef_Down||m->no==Lef_Up)
    {
        if (duty >= 0) {
            if (duty > PWM_DUTY_MAX) duty = PWM_DUTY_MAX;
            gpio_set(m->dirpin, 1);
            pwm_duty(m->channel, duty);
        } else {
            if (duty < -PWM_DUTY_MAX) duty = -PWM_DUTY_MAX;
            gpio_set(m->dirpin, 0);
            pwm_duty(m->channel, -duty);
        }
    } else if (m->no==Rig_Up||m->no==Rig_Down)
    {
        if (duty >= 0) {
            if (duty > PWM_DUTY_MAX) duty = PWM_DUTY_MAX;
            gpio_set(m->dirpin, 0);
            pwm_duty(m->channel, duty);
        } else {
            if (duty < -PWM_DUTY_MAX) duty = -PWM_DUTY_MAX;
            gpio_set(m->dirpin, 1);
            pwm_duty(m->channel, -duty);
        }
    }
}
void GetWheelSpeed(Motor *mot)
{
    int16 temp;
    switch (mot->no)
    {
        case Lef_Up:
            temp =  timer_quad_get(TIMER_2);
            timer_quad_clear(TIMER_2);

            if(temp < 0) temp = -temp;              //ǿ��ȡ��
            if(gpio_get(B3) == 0) mot->nowspeed = -temp;       //��������������Ϊ�ߵ�ƽ����ȡ����ֵΪ��
            else   mot->nowspeed = temp;      //��������������Ϊ�͵�ƽ����ȡ����ֵΪ��
            break;
        case Rig_Up:
            temp =  timer_quad_get(TIMER_3);
            timer_quad_clear(TIMER_3);

            if(temp < 0) temp = -temp;              //ǿ��ȡ��
            if(gpio_get(B5) == 1) mot->nowspeed = -temp;       //��������������Ϊ�ߵ�ƽ����ȡ����ֵΪ��
            else   mot->nowspeed = temp;      //��������������Ϊ�͵�ƽ����ȡ����ֵΪ��
            break;
        case Lef_Down:
            mot->nowspeed =  slave_encoder_left;
            break;
        case Rig_Down:
            mot->nowspeed =  slave_encoder_right;
            break;

    }
}
//--------------------���̿�����----------------------------------------------
//�ⲿʹ��
chassis_info_t mychassis = {
        .wheel[0].speed_pid.result = 0,
        .wheel[1].speed_pid.result = 0,
        .wheel[2].speed_pid.result = 0,
        .wheel[3].speed_pid.result = 0,
                            };
Chassis_Move_Mode chassis_move_mode=STRAIGHT_RUN;
uint8 fork_keep = 0;
uint8 StopReg = 0;
uint8 into_gar= 0;
uint8 out_gar = 0;
//�ڲ�ʹ��
uint8 pos_swtich = 0;





//���õ��̿���ģʽ
static void Chassis_Set_Mode(chassis_info_t *mode_set);
//���̲������ݸ���
static void Chassis_Feedback_Update(chassis_info_t *data_to_update);
//���õ��̿�������ֵ
static void Chassis_Set_Control(chassis_info_t *control);
//ͨ�������������ݽ��п���
static void Chassis_PID_Control(chassis_info_t *chassis_control_loop);
//�����ٶȷֽ�
static void Chassis_Wheel_Speed_Measure(int32 wheelspeed[4],chassis_info_t *speed_measure);
//����״̬���ú���
static void Chassis_Behaviour_Control_Set(chassis_info_t *behaviour);
//���̲�����
static int16 Chassis_NO_Follow(chassis_info_t *follow);
//���̸���ADCģʽ
static int16 Chassis_Follow_ADC_Control(chassis_info_t *follow);
//���̸�������ͷģʽ
static int16 Chassis_Follow_CAM_Control(chassis_info_t *follow);
//���̸�������ͷģʽ
static int16 Chassis_Follow_ALL_Control(chassis_info_t *follow);
/**
  * @brief          ���̳�ʼ��
  * @param[out]     data_init:"chassis_info_t"����ָ��.
  * @retval         none
  */

void Chassis_Init(chassis_info_t *chassis_init)
{
    //���õ��̳�ʼģʽ
    chassis_init->follow_mode       =DEFAULT_FOLLOW_MODE;
    chassis_init->add_wz            =flash.add_wz;
    chassis_init->outGarage.speed   =flash.out_speed;
    chassis_init->outGarage.time    =flash.out_time;
    chassis_init->intoGarage.speed  = 200;
    chassis_init->intoGarage.time   = flash.inrotime+flash.ingotime;
    chassis_init->move_mode         =DEFAULT_MOVE_MODE;
    chassis_init->vx_set            = 0;
    chassis_init->vy_set            = 0;
    chassis_init->wz_set            = 0;
    chassis_init->rotate.rotime     = 0;
    chassis_init->rotate.rospeed    = flash.rospeed;

    switch (chassis_init->follow_mode)
    {
        case CHASSIS_NO_FOLLOW:
            chassis_init->follow_control = Chassis_NO_Follow;
            break;
        case CHASSIS_FOLLOW_ADC:
            chassis_init->follow_control = Chassis_Follow_ADC_Control;
            break;
        case CHASSIS_FOLLOW_CAM:
            chassis_init->follow_control = Chassis_Follow_CAM_Control;
            break;
        case CHASSIS_FOLLOW_ALL:
            chassis_init->follow_control = Chassis_Follow_ALL_Control;
            break;
    }

    for (uint8 t = 0; t < 4; ++t)
    {
        chassis_init->wheel[t].no = t;
        chassis_init->wheel[t].speed_set = 0;
        chassis_init->wheel[t].speed_pid.mode=PID_INCREMENT;
        chassis_init->wheel[t].speed_pid.Kp=flash.speed_kp;
        if (chassis_init->funny)
        {
            chassis_init->wheel[t].speed_pid.Ki=0;
        } else
        {
            chassis_init->wheel[t].speed_pid.Ki=flash.speed_ki;
        }
        chassis_init->wheel[t].speed_pid.Kd=0;
        chassis_init->wheel[t].speed_pid.integral=0;
        chassis_init->wheel[t].speed_pid.result=0;
        chassis_init->wheel[t].speed_pid.lasterror=0;
        chassis_init->wheel[t].speed_pid.preerror=0;
        chassis_init->wheel[t].speed_pid.epsilon=180;
    }

    //�����ʼ��
    gpio_init(C5,GPI,1,IN_PULLUP);
    gpio_init(B0,GPI,1,IN_PULLUP);

    //���������
    chassis_init->cyctime           =!gpio_get(C5);
    chassis_init->outGarage.dir     = gpio_get(B0);

    StopReg = out_gar;
}
/**
  * @brief          ����loop
  * @retval         none
  */
void Chassis_NormPIT(chassis_info_t *chassis_loop)
{
    if (chassis_move_mode==NO_MOVE)
    {
        Stop_Move();
        return;
    }
    //���ٶ�
    Chassis_Feedback_Update(chassis_loop);

    //pid����
    Chassis_PID_Control(chassis_loop);
}

static Filter_Info Error_Filter[3] = {{0,0,4},
                               {0,0,4},
                               {0,0,8}};
static PID  angle_pid =
        {
        .mode = PID_POSITION,
        .Kp = 55,
        .Kd = 8,
        .result = 0,
        .lasterror = 0
        };

void Chassis_FunnyPIT(chassis_info_t *chassis_loop)
{
    int16 input[3];
    if (pack[0])
    {
        int16 max = pack[4]*4;
        //���ٶȻ�
        PID_Realize(&angle_pid,-(slave_controlled_val/10),-max*(pack[3]-50)/30);
        input[0] = max*(pack[1]-50)/50;
        input[1] = max*(pack[2]-50)/50;
        input[2] = angle_pid.result;
    } else
    {
        input[0] = 0;
        input[1] = 0;
        input[2] = 0;
    }
    First_Order_Low_Pass_Filter(&Error_Filter[0],input[0]);
    First_Order_Low_Pass_Filter(&Error_Filter[1],input[1]);
    First_Order_Low_Pass_Filter(&Error_Filter[2],input[2]);

    chassis_loop->vx_set = deadband_limit(Error_Filter[0].output,30);
    chassis_loop->vy_set = deadband_limit(Error_Filter[1].output,30);
    chassis_loop->wz_set = deadband_limit(Error_Filter[2].output,30);

    int32 wheel_speed[4];
    //�����˶��ֽ�
    Chassis_Wheel_Speed_Measure(wheel_speed,chassis_loop);
    for (uint8_t t = 0; t < 4; t++)
    {
        chassis_loop->wheel[t].speed_set = wheel_speed[t];
    }
}
void Chassis_Control_Swtich()
{
    //ѡ��ģʽ
    Chassis_Set_Mode(&mychassis);
    //�����ٶ�
    Chassis_Set_Control(&mychassis);
}

/**
  * @brief          ���õ��̿���ģʽ
  * @param[out]     mode:"chassis_info_t"����ָ��.
  * @retval         none
  */
static void Chassis_Set_Mode(chassis_info_t *mode_set)
{
    if (mode_set->move_mode!=chassis_move_mode)
    {
        beep_time = 5;
        switch (chassis_move_mode)
        {
            case STRAIGHT_RUN:
                //������ת����
                pos_swtich = 1;
                if (mode_set->cyctime==FIRST)
                {
                    mode_set->rotate.rotime = flash.rotime[1];
                } else
                {
                    mode_set->rotate.rotime = flash.rotime[2];
                }
                break;
            case NO_MOVE:
                break;
            case ACROSS_THE_WALK:
                //������ת����
                pos_swtich = 1;
                if (mode_set->cyctime==SECOND)
                {
                    mode_set->rotate.rotime = flash.rotime[0];
                } else
                {
                    data_return = 0xaa;
                }
                StopReg = 1;
                break;
        }
        mode_set->move_mode = chassis_move_mode;
    }
}

/**
  * @brief          ���̲������ݸ���
  * @param[out]     data_to_update:"chassis_info_t"����ָ��.
  * @retval         none
  */
static void Chassis_Feedback_Update(chassis_info_t *data_to_update)
{
    for (uint8 t = 2; t < 4; t++) // ��ȡ�ٶ�ֵ
    {
        GetWheelSpeed(&motor[t]);
    }
}
/**
  * @brief          ���õ��̿�������ֵ, ���˶�����ֵ��ͨ��Chassis_Behaviour_Control_Set�������õ�
  * @param[out]     control:"chassis_info_t"����ָ��.
  * @retval         none
  */
void Chassis_Set_Control(chassis_info_t *control)
{
    //�ٶȷֽⷨ
    uint8 t;
    int16 temp = 0;
    int32 vector_rate = 0,max_vector = 0;
    int32 wheel_speed[4];
    //��ȡ������������ֵ
    Chassis_Behaviour_Control_Set(control);

    //�����˶��ֽ�
    Chassis_Wheel_Speed_Measure(wheel_speed,control);

    //�������ӿ�������ٶȣ�������������ٶ�
    for (t = 0; t < 4; t++)
    {
        temp = ABS(wheel_speed[t]);
        if (max_vector < temp)
        {
            max_vector = temp;
        }
    }
    if (STRAIGHT_RUN==control->move_mode&&max_vector > flash.maxspeed)
    {
        vector_rate = flash.maxspeed *100/ max_vector;
        for (t = 0; t < 4; t++)
        {
            wheel_speed[t] = wheel_speed[t] * vector_rate /100;
            control->wheel[t].speed_set = wheel_speed[t];
        }
    } else
    {
        for (t = 0; t < 4; t++)
        {
            control->wheel[t].speed_set = wheel_speed[t];
        }
    }

}

/**
  * @brief          ����ѭ�������ݿ����趨ֵ�����п���
  * @param[out]     chassis_control_loop:"chassis_info_t"����ָ��.
  * @retval         none
  */
static void Chassis_PID_Control(chassis_info_t *control)
{
    uint8 t=0;
    for (t = 0; t <4; t++)
    {
        PID_Realize(&control->wheel[t].speed_pid, motor[t].nowspeed, control->wheel[t].speed_set);
//            PID_Realize(&control->wheel[t].speed_pid, motor[t].nowspeed, 200);
        Setduty(&motor[t],control->wheel[t].speed_pid.result);
//            Setduty(&motor[t],-2000);
    }

}

/**
  * @brief          ���ÿ�����.
  * @param[in]      chassis_info_t,  ��������������Ϣ.
  * @retval         none
  */
static void Chassis_Behaviour_Control_Set(chassis_info_t *behaviour)
{
    //���⴦��
    if (out_gar)
    {
        if (behaviour->outGarage.time>0)
        {
            if (behaviour->outGarage.dir==Left) behaviour->vx_set = -behaviour->outGarage.speed;
            else behaviour->vx_set = behaviour->outGarage.speed;
            behaviour->vy_set = 0;
            behaviour->wz_set = 0;
            behaviour->outGarage.time--;
            if (behaviour->outGarage.time==0)
            {
                out_gar = 0;
                StopReg = 0;
            }
            return;
        }
    }
    if (into_gar&&garage_flag)
    {
        if (garage_flag==1)
        {
            StopReg = 1;
            static int inte = 0;
            for (uint8 t = 0; t < 4; ++t) {
                inte += motor[t].nowspeed;
            }
            if (inte>11000) garage_flag = 2;
            behaviour->vx_set = 0;
            behaviour->vy_set = flash.garspeed;
            behaviour->wz_set = 0;
            return;
        } else if (garage_flag==2)
        {
            if (behaviour->intoGarage.time>flash.ingotime)
            {
                behaviour->vx_set = 0;
                behaviour->vy_set = 0;
                if (behaviour->outGarage.dir==Right)    behaviour->wz_set = behaviour->intoGarage.speed;
                else behaviour->wz_set = -behaviour->intoGarage.speed;
                behaviour->intoGarage.time--;
                return;
            } else if (behaviour->intoGarage.time>0)
            {
                behaviour->intoGarage.time--;
                behaviour->vx_set = 0;
                behaviour->vy_set = flash.garspeed;
                behaviour->wz_set = 0;
                return;
            } else
            {
                behaviour->vx_set = 0;
                behaviour->vy_set = 0;
                behaviour->wz_set = 0;
                return;
            }
        }
    }
    //���ݲ�ͬ��̬���п���
    if (pos_swtich)    //��ѯ��ǰ�Ƿ�����ת����,������ִ��
    {
        static uint8_t stoptim = STOP_TIME;
        static uint8_t postim  = POS_TIME;
        if (stoptim>0)
        {
            stoptim--;
            behaviour->vx_set = 0;
            behaviour->vy_set = 0;
            behaviour->wz_set = 0;
            return;
        } else if (behaviour->rotate.rotime>0)
        {
            behaviour->rotate.rotime--;
            behaviour->vx_set = 0;
            behaviour->vy_set = 0;
            behaviour->wz_set = -behaviour->rotate.rospeed;
            return;
        } else if (postim>0)
        {
            postim--;
            behaviour->vx_set = 0;
            behaviour->vy_set = 0;
            behaviour->wz_set = behaviour->follow_control(behaviour);
            return;
        } else
        {
            pos_swtich = 0;
            if (!stoptim)   stoptim = STOP_TIME;
            if (!postim )   postim  = POS_TIME;
            if (behaviour->move_mode==STRAIGHT_RUN)
            {
                fork_keep = 20;
                StopReg = 0;
            } else
            {
                data_return = 0xaa;
            }
        }
    }

    //����ģʽ��ȡ���ٶ�
    int16 TransAngle = behaviour->follow_control(behaviour);
    switch (behaviour->move_mode)
    {
        case NO_MOVE:
            behaviour->vx_set = 0;
            behaviour->vy_set = 0;
            behaviour->wz_set = 0;
            break;
        case STRAIGHT_RUN:
            behaviour->vx_set = 0;
            //����·���ı��ٶ�
            switch (order.TrackType)
            {
                case CIRCLE :
                    behaviour->vy_set = flash.cirspeed;
                    break;
                case RAMP   :
                    if (ramp_flag==2)   behaviour->vy_set = flash.ramspeed;
                    break;
                case FORK   :
                    behaviour->vy_set = flash.forkspeed;
                    break;
                case GARAGE :
                    if (flash.order[GARAGE]<20&&garage_flag&&garage_flag<=2) behaviour->vy_set = flash.garspeed;
                    else behaviour->vy_set = flash.vy_speed;
                    break;
                case OUTROAD:
                    behaviour->vy_set = 0;
                    break;
                default     :behaviour->vy_set = flash.vy_speed;break;
            }
            behaviour->wz_set = TransAngle;
            if (2==flash.two)
            {
                if (GARAGE==order.TrackType)
                {
                    flash.two = 1;
                    flash.maxspeed -= ADD_MAXSP;
                } else
                {
                    behaviour->vy_set += ADD_SPEED;
                }
            }
            break;
        case ACROSS_THE_WALK:
            if (slave_controlled_val==0x0fff)
            {
                behaviour->vx_set = 140;
            } else
            {
                behaviour->vx_set = flash.vx_speed;
            }
            behaviour->vy_set = 0;
            behaviour->wz_set = TransAngle;
            break;
    }
}
/**
  * @brief          ���̲�����
  * @retval         ���ص�����
  */
static int16 Chassis_NO_Follow(chassis_info_t *follow)
{
    return 0;
}

/**
  * @brief          ���̸����п���
  * @retval         ���ص�����
  */
static int16 Chassis_Follow_ADC_Control(chassis_info_t *follow)
{
    switch (follow->move_mode)
    {
        case STRAIGHT_RUN:
            PID_Realize(&follow->pid_list[0],-slave_controlled_val,0);
            return follow->pid_list[0].result;
        case ACROSS_THE_WALK:
            //������
            if (slave_controlled_val==0x1fff)
            {
                chassis_move_mode = STRAIGHT_RUN;
                data_return = 0;
                return 0;
            }
            PID_Realize(&follow->pid_list[3],-slave_error_val,0);
            return follow->pid_list[3].result;
        case NO_MOVE:
            return 0;
    }
}
/**
  * @brief          ���̸�������ͷ����
  * @retval         ���ص�����
  */
static int16 Chassis_Follow_CAM_Control(chassis_info_t *follow)
{
    switch (follow->move_mode)
    {
        case STRAIGHT_RUN:
            if (RAMP==order.TrackType)
            {
//                //��������ֻ�õ��,������ֱ���õ��ƫ��ֵ
//                PID_Realize(&follow->pid_list[2],-slave_controlled_val,0);
//                return follow->pid_list[2].result;
                PID_Realize(&follow->pid_list[0],-Cam_Error,0);
                return follow->pid_list[0].result;
            }
            else
            {
                PID_Realize(&follow->pid_list[0],-Cam_Error,0);
                return follow->pid_list[0].result;
            }
        case ACROSS_THE_WALK:
            //������
            if (slave_controlled_val==0x1fff)
            {
                chassis_move_mode = STRAIGHT_RUN;
                data_return = 0;
                if (1==flash.two)
                {
                    flash.two = 2;
                    flash.maxspeed += ADD_MAXSP;
                }
                return 0;
            }
            PID_Realize(&follow->pid_list[3],-slave_error_val,0);
            return follow->pid_list[3].result;
        case NO_MOVE:
            return 0;
    }
}
/**
  * @brief          ���̸�������ͷ����
  * @retval         ���ص�����
  */
static int16 Chassis_Follow_ALL_Control(chassis_info_t *follow)
{
    return Chassis_Follow_CAM_Control(follow);
}
/**
  * @brief          �����˶��ֽ�
  * @param[in]      vx_set: x�����ٶ�
  * @param[in]      vy_set: y�����ٶ�
  * @param[in]      wz_set: ��ת���ٶ�
  * @param[out]     wheel_speed: �ĸ������ٶ�
  * @retval         none
  */
static void Chassis_Wheel_Speed_Measure(int32 wheelspeed[4],chassis_info_t *speed_measure)
{
    if (!speed_measure->funny&&STRAIGHT_RUN==speed_measure->move_mode&&ABS(speed_measure->wz_set)>=WZ_LIMIT)
    {
        if (speed_measure->wz_set>0)
        {
            wheelspeed[0] = +speed_measure->vx_set+speed_measure->vy_set -   (10+speed_measure->add_wz)  * speed_measure->wz_set/10;
            wheelspeed[1] = -speed_measure->vx_set+speed_measure->vy_set +    speed_measure->wz_set;
            wheelspeed[2] = -speed_measure->vx_set+speed_measure->vy_set -    speed_measure->wz_set;
            wheelspeed[3] = +speed_measure->vx_set+speed_measure->vy_set +   (10+speed_measure->add_wz)  * speed_measure->wz_set/10;
        } else
        {
            wheelspeed[0] = +speed_measure->vx_set+speed_measure->vy_set -    speed_measure->wz_set;
            wheelspeed[1] = -speed_measure->vx_set+speed_measure->vy_set +    (10+speed_measure->add_wz) * speed_measure->wz_set/10;
            wheelspeed[2] = -speed_measure->vx_set+speed_measure->vy_set -    (10+speed_measure->add_wz) * speed_measure->wz_set/10;
            wheelspeed[3] = +speed_measure->vx_set+speed_measure->vy_set +    speed_measure->wz_set;
        }
    } else
    {
        wheelspeed[0] = +speed_measure->vx_set+speed_measure->vy_set -    speed_measure->wz_set;
        wheelspeed[1] = -speed_measure->vx_set+speed_measure->vy_set +    speed_measure->wz_set;
        wheelspeed[2] = -speed_measure->vx_set+speed_measure->vy_set -    speed_measure->wz_set;
        wheelspeed[3] = +speed_measure->vx_set+speed_measure->vy_set +    speed_measure->wz_set;
    }
}
int16 ABS(int16 input)
{
    if (input<0) input=-input;
    return input;
}
//----------------------PID������----------------------------------------
void PID_Init()
{
    //����
    mychassis.pid_list[0].mode=PID_POSITION;
    mychassis.pid_list[0].Kp=flash.dir_kp;
    mychassis.pid_list[0].Kd=flash.dir_kd;
//    mychassis.pid_list[0].Kp=6;
//    mychassis.pid_list[0].Kd=1;
    mychassis.pid_list[0].lasterror=0;

    //Բ��
    mychassis.pid_list[1].mode=PID_POSITION;
    mychassis.pid_list[1].Kp=flash.dir_kp;
    mychassis.pid_list[1].Kd=flash.dir_kd;
    mychassis.pid_list[1].lasterror=0;

    mychassis.pid_list[2].mode=PID_POSITION;
    mychassis.pid_list[2].Kp=RAMP_DIR_KP;
    mychassis.pid_list[2].Kd=RAMP_DIR_KD;
    mychassis.pid_list[2].lasterror=0;

    mychassis.pid_list[3].mode=PID_POSITION;
    mychassis.pid_list[3].Kp=flash.fork_kp;
    mychassis.pid_list[3].Kd=flash.fork_kd;
    mychassis.pid_list[3].lasterror=0;
}
void Clear_PID_Data(PID *vpid)
{
    vpid->preerror  = 0;
    vpid->lasterror = 0;
    vpid->result    = 0;
    vpid->integral  = 0;
}
void PID_Realize(PID *vPID, int16 processValue,int16 setpoint)
{
    if (vPID->mode==PID_INCREMENT)
    {
        int16 thisError;
        int16 increment;
        int16 pError,iError;

        thisError=setpoint-processValue; //�õ�ƫ��ֵ
        pError=thisError-vPID->lasterror;
//        iError=thisError;
        iError=0;
//        dError=thisError-2*(vPID->lasterror)+vPID->preerror;

        if(thisError < -vPID->epsilon)
        {
            if(thisError>=0)    iError=thisError;
        }
        else if (thisError > vPID->epsilon)
        {
            if(thisError<=0)    iError=thisError;
        } else
        {
            iError=thisError;
        }
        increment=vPID->Kp*pError/5+vPID->Ki*iError/5;   //��������
//        if(thisError <= vPID->epsilon&&thisError >= -vPID->epsilon)
//            increment=vPID->Kp*pError/5+vPID->Ki*iError/5;   //��������
//        else
//            increment=vPID->Kp*pError/5;

        vPID->preerror=vPID->lasterror;  //���ƫ�������´�����
        vPID->lasterror=thisError;
        vPID->result+=increment;
    }
    else
    {
        int16 thisError;
        thisError=setpoint-processValue;
        //����ֻ�����򻷵�pd
//        if(ABS(thisError)<= vPID->epsilon)
//        {
//            vPID->integral+=thisError;  //��������
//        }
//
//        vPID->result=(int)vPID->Kp*thisError/5+(int)vPID->Ki*vPID->integral/5+(int)vPID->Kd*(thisError-vPID->lasterror)/5;

        vPID->result=vPID->Kp*thisError/50+vPID->Kd*(thisError-vPID->lasterror)/50;
        vPID->lasterror=thisError;
    }
}

/**
  * @brief          һ�׵�ͨ�˲�����
  * @param[in]      һ�׵�ͨ�˲��ṹ��
  * @param[in]      �����ʱ�䣬��λ s
  * @retval         ���ؿ�
  */
void First_Order_Low_Pass_Filter(Filter_Info *first_order_filter_type, int16 input)
{
    first_order_filter_type->input = input;
    first_order_filter_type->output=
            first_order_filter_type->ratio * first_order_filter_type->input/10 + (10 - first_order_filter_type->ratio) * first_order_filter_type->output /10;
}
