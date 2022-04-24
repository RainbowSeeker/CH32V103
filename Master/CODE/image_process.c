//
// Created by 杨宇 on 2021/4/13.
//
#include "image_process.h"
#include "move.h"
#include "com.h"

uint8 StandardLefArray[height]=
        {
                68, 67, 65, 64, 63, 62, 62, 61, 60, 60, 59, 58, 57, 57, 56, 55, 55, 54, 54, 53, 53, 52, 51, 50, 50, 49,
                48, 47, 47, 46, 46, 45, 44, 44, 43, 42, 42, 41, 41, 40, 39, 38, 37, 37, 36, 36, 35, 34, 34, 33, 32, 32,
                31, 31, 30, 29, 29, 28, 28, 27, 27, 26, 25, 25, 24, 24, 23, 22, 22, 21
        };
uint8 StandardRigArray[height]=
        {
                124, 124, 124, 125, 126, 127, 127, 128, 129, 130, 130, 131, 131, 132, 133, 133, 134, 135, 135, 136, 137,
                138, 138, 139, 140, 141, 141, 142, 143, 144, 145, 145, 146, 147, 147, 148, 149, 149, 150, 150, 151, 152,
                152, 153, 154, 154, 155, 155, 156, 156, 157, 158, 158, 159, 159, 160, 161, 161, 162, 162, 162, 162, 162,
                163, 163, 164, 165, 165, 166, 166
        };
uint8 StandardMidArray[height]=
        {
                56, 57, 59, 61, 63, 65, 65, 67, 69, 70, 71, 73, 74, 75, 77, 78, 79, 81, 81, 83, 84, 86, 87, 89, 90, 92,
                93, 95, 96, 98, 99, 100, 102, 103, 104, 106, 107, 108, 109, 110, 112, 114, 115, 116, 118, 118, 120, 121,
                122, 123, 125, 126, 127, 128, 129, 131, 132, 133, 134, 135, 135, 136, 137, 138, 139, 140, 142, 143, 144,
                145
        };
//-------------------------------------------------------------
//标志位
uint8   garage_flag=0;
uint8   ramp_flag=0;
uint8   cross_flag=0;
uint8   fork_flag=0;
uint8   circle_flag=0;
Side    circle_type=Mid;
LostInfo leflost={.startline=0,.endline=0},riglost={.startline=0,.endline=0};
//外部引用变量
uint8 Sepval=0;     //二值化阈值
uint8 SepLine=1;
int16 tem_error=0;  //内部改变,用于暂时强制修改偏差
//-------------------------------------------------------------
//内部使用变量
LineInfo Line[height]={0};      //记录行
FitLine xline;                  //记录斜线信息
img_test_schedule_t order =     //赛道检测调度器
        {
                .TrackType      = NORMAL,
                .FlagOccupy     = False,
                .WaitTime       = 0,
                .Schedule[0]    = IsCircle,
                .Schedule[1]    = IsGarage,
                .Schedule[2]    = IsFork,
                .Schedule[3]    = IsCross,
                .Schedule[4]    = IsRamp,
                .Schedule[5]    = IsOut,
                .TrackHandle    = Track_Handle,
                .Task_Schedule  = Task_Schedule,
        };
mwl_config_t MaxWhite =  //记录最大白列
        {
                .StartSide  = Left,
                .line       = 0,
                .row        = 0,
                .SkipLine   = 3,
                .SkipRow    = 12,
                .BLWidth    = 20
        };
void End_NowThread()
{
    Clear_All_Flag();
}
void Task_Schedule(uint8 image[height][width])
{
    if (order.FlagOccupy)
    {
        if (TO==order.TrackHandle(image))
        {
            End_NowThread();
        } else
        {
            order.WaitTime++;
        }
    } else
    {
        for (uint8_t cnt = 0; cnt < THREAD_NUM; cnt++)
        {
            if (TS==order.Schedule[cnt](image))
            {
                order.FlagOccupy = True;
                return;
            }
        }
        //正常赛道则直接补边界
        Fix_Border();
    }
}
void Clear_All_Flag()
{
    order.TrackType  = NORMAL;
    order.FlagOccupy = False;
    order.WaitTime   = 0;
    garage_flag=0;
    ramp_flag=0;
    fork_flag=0;
    circle_flag=0;
    cross_flag = 0;
    circle_type = Mid;
}
void Para_Reset()
{
    //Parameter Reset!
    leflost.startline=0;leflost.endline=0;
    riglost.startline=0;riglost.endline=0;
    tem_error = 0;
    SepLine = 1;
}
//!!!图像处理and判断函数!!!
void Image_Process(uint8 image[height][width])
{
    //求最大白列
    if (ERROR==GetMaxWhiteRow(image,&MaxWhite))
    {
        //扫描最后一行(直接遍历)
        Findborder(image,height-1,Left,0);
        Findborder(image,height-1,Right,0);
    }
    /* 开始扫线 */
    SepLine = GetSepLine(image,MaxWhite.line);

    TrackScan(image);

    if (!StopReg)
    {
        order.Task_Schedule(image);
    }
}
void TrackScan(uint8 image[height][width])
{
    int16 i=height-1;
    //扫描剩余行
    for(i=height-2;i>=SepLine;i--)
    {
        Findborder(image,i,Left ,1);
        Findborder(image,i,Right,1);
    }
    uint8 lx = (leflost.startline+leflost.endline)/2;
    if (Line[lx].Lefborder>=5&&!garage_flag)
    {
        leflost.startline = 0;
        leflost.endline = 0;
    }
    lx = (riglost.startline+riglost.endline)/2;
    if (Line[lx].Rigborder<=width-5&&!garage_flag)
    {
        riglost.startline = 0;
        riglost.endline = 0;
    }
}
void Fix_Border()
{
    for (int8 t = max(leflost.startline,riglost.startline); t >= min(leflost.endline,riglost.endline); t--)
    {
        if (t<=leflost.startline&&t>=leflost.endline)
        {
            if (t>riglost.startline||t<riglost.endline)
            {
                Line[t].Lefborder = Line[t].Rigborder - StandardMidArray[t];
            }
        } else
        {
            Line[t].Rigborder = Line[t].Lefborder + StandardMidArray[t];
        }
    }
}
uint8_t TillBorder(uint8 image[height][width],uint8_t line,Side dir,Jump_Type jump,uint8_t start,uint8_t end)
{
    uint8_t result=0;
    int16_t t;
    if (jump==WTB)
    {
        if (dir==Left)
        {
            for (t = start; t > 2&&t > end; t=t-2)
            {
                if (image[line][t-2]<=Sepval&&image[line][t]>Sepval)
                {
                    result = t;
                    break;
                }
            }
        } else
        {
            for (t = start; t < width-2&&t < end; t=t+2)
            {
                if (image[line][t]>Sepval&&image[line][t+2]<=Sepval)
                {
                    result = t;
                    break;
                }
            }
        }

    } else if (jump==BTW)
    {
        if (dir==Left)
        {
            for (t = start; t > 2&&t > end; t=t-2)
            {
                if (image[line][t-2]>Sepval&&image[line][t]<=Sepval)
                {
                    result = t;
                    break;
                }
            }
        } else
        {
            for (t = start; t < width-2&&t < end; t=t+2)
            {
                if (image[line][t]<=Sepval&&image[line][t+2]>Sepval)
                {
                    result = t;
                    break;
                }
            }
        }
    }
    return result;
}
//找凹点
Position FindPit(uint8 image[height][width],Side dir,uint8_t hline)
{
    uint8_t t = hline;
    Position pos = {.line = 0,.row = 0};
    uint8_t lef,rig,mid;
    if (dir==Left)
    {
        lef = TillBorder(image,hline,Right,WTB,1,width-1);
        rig = TillBorder(image,hline,Left ,WTB,width-1,1);
        mid = (lef + rig)/2;
    } else
    {
        lef = TillBorder(image,hline,Right,WTB,1,width-1);
        rig = TillBorder(image,hline,Left ,WTB,width-1,1);
        mid = (lef + rig)/2;
    }
    if (lef&&rig&&image[hline][mid]<=Sepval)
    {
        for (t = hline+1; t < height; t++)
        {
            if (image[t][mid]>Sepval)
                break;
            lef = TillBorder(image,t,Left ,BTW,mid,1);
            rig = TillBorder(image,t,Right,BTW,mid,width-1);
            mid = (lef + rig)/2;
        }
        pos.row = mid;
        pos.line = t;
    }
    return pos;
}
ThreadStatus Track_Handle(uint8 image[height][width])
{
    Position xpos = {.line = 0,.row = 0};
    switch (order.TrackType)
    {
        case OUTROAD:return TO;
        case CROSS:
            if (leflost.startline<=height-20)
            {
                BorderMakeUp(leflost.startline,leflost.endline,Left,UP);
            }
            if (riglost.startline<=height-20)
            {
                BorderMakeUp(riglost.startline,riglost.endline,Right,UP);
            }
            if (order.WaitTime>=80)
            {
                return TO;
            }
            break;
        case CIRCLE:
            if (Left==circle_type)
            {
                if (circle_flag==3)
                {
                    if (order.WaitTime<=12)
                    {
                        Position pos=FindPit(image,Left,leflost.endline+1);
                        if (pos.line)
                        {
                            xpos.line = height-1;xpos.row = Line[xpos.line].Rigborder-20;
                            int16_t factor = (int16_t)(pos.row-xpos.row)*10/(pos.line-xpos.line);
                            for (int8 t = height-1; t >= leflost.endline; t--)
                            {
                                Line[t].Rigborder = (int16) (pos.row + (t-pos.line) * factor/ 10);
                                Line[t].Lefborder = Line[t].Rigborder - StandardMidArray[t];
                            }
                            SepLine = leflost.endline;
                        }
                    }
                    if (order.WaitTime>=50&&Line[30].Lefborder<=5&&Line[30].Rigborder>=width-5)
                    {
                        circle_flag = 4;
                        order.WaitTime = 0;
                    }
                }
                if (circle_flag==4)
                {
                    if (!riglost.startline)
                    {
                        circle_flag = 5;
                        order.WaitTime = 0;
                    } else if (order.WaitTime<=60)
                    {
                        if (riglost.startline==height-1)
                        {
                            xpos.line = height-1;xpos.row = width-1;
                        } else
                        {
                            xpos.line = riglost.startline+1;xpos.row = Line[xpos.line].Rigborder;
                        }
                        for (int8 t = riglost.startline; t >= riglost.endline; t--)
                        {
                            Line[t].Rigborder = (int16) (xpos.row + (t-xpos.line) * 35/10);
                        }
                    }
                }
                if (circle_flag==5)
                {
                    if (leflost.startline)
                    {
                        for (int16 k = leflost.startline; k >= leflost.endline; k--)
                        {
                            Line[k].Lefborder = Line[k].Rigborder-StandardMidArray[k];
                        }
                    } else if (order.WaitTime>=40)
                    {
                        return TO;
                    }
                }
            }
            else
            {
                if (circle_flag==3)
                {
                    if (order.WaitTime<=12)
                    {
                        Position pos=FindPit(image,Right,riglost.endline+1);
                        if (pos.line)
                        {
                            xpos.line = height-1;xpos.row = Line[xpos.line].Lefborder+20;
                            int16_t factor = (int16_t)(pos.row-xpos.row)*10/(pos.line-xpos.line);
                            for (int8 t = height-1; t >= riglost.endline; t--)
                            {
                                Line[t].Lefborder = (int16) (pos.row + (t-pos.line) * factor/ 10);
                                Line[t].Rigborder = Line[t].Lefborder + StandardMidArray[t];
                            }
                            SepLine = riglost.endline;
                        }
                    }
                    if (order.WaitTime>=50&&Line[30].Lefborder<=5&&Line[30].Rigborder>=width-5)
                    {
                        circle_flag = 4;
                        order.WaitTime = 0;
                    }
                }
                if (circle_flag==4)
                {
                    if (!leflost.startline)
                    {
                        circle_flag = 5;
                        order.WaitTime = 0;
                    } else if (order.WaitTime<=60)
                    {
                        if (leflost.startline==height-1)
                        {
                            xpos.line = height-1;xpos.row = 1;
                        } else
                        {
                            xpos.line = leflost.startline+1;xpos.row = Line[xpos.line].Lefborder;
                        }
                        for (int8 t = leflost.startline; t >= leflost.endline; t--)
                        {
                            Line[t].Lefborder = (int16) (xpos.row + (xpos.line-t) * 35/10);
                        }
                    }
                }
                if (circle_flag==5)
                {
                    if (riglost.startline)
                    {
                        for (int16 k = riglost.startline; k >= riglost.endline; k--)
                        {
                            Line[k].Rigborder = Line[k].Lefborder+StandardMidArray[k];
                        }
                    } else if (order.WaitTime>=40)
                    {
                        return TO;
                    }
                }
            }
            break;
        case GARAGE:
            if (100==garage_flag)
            {
                if (order.WaitTime>=18)
                {
                    mychassis.cyctime = SECOND;
                    return TO;
                }
                else
                {
                    tem_error = 1;
                }
                return NO;
            }
            if (flash.order[GARAGE]>=20)
            {
                if (order.WaitTime>=18)
                {
                    mychassis.cyctime = FIRST;
                    return TO;
                }
                else
                {
                    tem_error = 1;
                 }
                 return NO;
            }
            if (mychassis.outGarage.dir==Right)
            {
                if (garage_flag==1)
                {
                    if (leflost.startline>=height-5&&leflost.endline>=30-flash.one) //7.15 best 28
                    {
                        garage_flag = 2;
                        order.WaitTime = 0;
                    } else
                    {
                        for (int8 k = leflost.startline; k > leflost.endline; k--)
                        {
                            Line[k].Lefborder = Line[k].Rigborder - StandardMidArray[k];
                        }
                    }
                }
                if (garage_flag==2)
                {
                    if (order.WaitTime<=20&&leflost.startline)
                    {
                        uint8 row;
                        Position pos=FindPit(image,Left,leflost.endline+1);
                        if (!pos.line)
                        {
                            pos.line = leflost.endline;pos.row = 10;
                        }
                        if (image[height-1][width-1]>Sepval)
                        {
                            Line[height-1].Rigborder = width-1;
                        } else
                        {
                            for (row = width-1; row > 2; row=row-2)
                            {
                                if (image[height-1][row-2]>Sepval&&image[height-1][row]<=Sepval)
                                {
                                    Line[height-1].Rigborder = row-1;
                                    break;
                                }
                            }
                        }
                        xpos.line = height-1;xpos.row = Line[xpos.line].Rigborder-40;
                        int16_t factor = (int16_t)(pos.row-xpos.row)*10/(pos.line-xpos.line);
                        for (int8 t = height-1; t >= leflost.endline; t--)
                        {
                            Line[t].Rigborder = (int16) (pos.row + (t-pos.line) * factor/ 10);
                            Line[t].Lefborder = Line[t].Rigborder - StandardMidArray[t];
                        }
                        SepLine = leflost.endline;
                    } else
                    {
                        chassis_move_mode = NO_MOVE;
                        beep_time = 20;
                        return TO;
                    }
                }
            } else
            {
                if (garage_flag==1)
                {
                    if (riglost.startline>=height-5&&riglost.endline>=30-flash.one) //7.14 best 27
                    {
                        garage_flag = 2;
                        order.WaitTime = 0;
                    } else
                    {
                        for (int8 k = riglost.startline; k > riglost.endline; k--)
                        {
                            Line[k].Rigborder = Line[k].Lefborder + StandardMidArray[k];
                        }
                    }
                }
                if (garage_flag==2)
                {
                    if (order.WaitTime<=20&&riglost.startline)
                    {
                        uint8 row;
                        Position pos=FindPit(image,Right,riglost.endline+1);
                        if (!pos.line)
                        {
                            pos.line = riglost.endline;pos.row = width-10;
                        }
                        if (image[height-1][1]>Sepval)
                        {
                            Line[height-1].Lefborder = 1;
                        } else
                        {
                            for (row = 1; row < width-3; row=row+2)
                            {
                                if (image[height-1][row+2]>Sepval&&image[height-1][row]<=Sepval)
                                {
                                    Line[height-1].Lefborder = row+1;
                                    break;
                                }
                            }
                        }
                        xpos.line = height-1;xpos.row = Line[xpos.line].Lefborder+40;
                        int16_t factor = (int16_t)(pos.row-xpos.row)*10/(pos.line-xpos.line);
                        for (int8 t = height-1; t >= riglost.endline; t--)
                        {
                            Line[t].Lefborder = (int16) (pos.row + (t-pos.line) * factor/ 10);
                            Line[t].Rigborder = Line[t].Lefborder + StandardMidArray[t];
                        }
                        SepLine = riglost.endline;
                    } else if (order.WaitTime>20)
                    {
                        if (order.WaitTime<=25)
                        {
                            tem_error = 1;
                        } else
                        {
                            chassis_move_mode = NO_MOVE;
                            beep_time = 20;
                            return TO;
                        }
                    }
                }
            }
            break;
        case FORK:
            if (order.WaitTime>=80)    return TO;
            uint8_t lef=0,rig=0,t;
            if (image[1][47]>Sepval)
            {
                 lef = 1;
            }
            if (image[1][141]>Sepval)
            {
                rig = 1;
            }
            for (t = 1; t < height-2; t=t+2)
            {
                if (!lef&&image[t][47]<=Sepval&&image[t+2][47]>Sepval)
                {
                    lef = t + 1;
                }
                if (!rig&&image[t][141]<=Sepval&&image[t+2][141]>Sepval)
                {
                    rig = t + 1;
                }
            }
            if (t>=height-2)
            {
                if (!lef)   lef = height;
                if (!rig)   rig = height;
            }
            if (order.WaitTime>=20&&lef&&rig&&lef+rig>120)
            {
                chassis_move_mode = ACROSS_THE_WALK;
                return TO;
            } else
            {
                if (lef==rig)   tem_error = 1;
                else tem_error = (lef - rig) * 8;
                lcd_showint16(0,2,tem_error);
//                if (mychassis.cyctime==0)  tem_error = -1;
//                else    tem_error = 1;
            }

            break;
        case RAMP:
            if (order.WaitTime>=50)
            {
                return TO;
            }
            if (ramp_flag==1)
            {
                if (order.WaitTime>=20&&Line[20].Rigborder>=Line[20].Lefborder+StandardMidArray[20]+22
                    &&Line[30].Rigborder>=Line[30].Lefborder+StandardMidArray[30]+20
                    &&Line[40].Rigborder>=Line[40].Lefborder+StandardMidArray[40]+18
                    &&Line[50].Rigborder>=Line[50].Lefborder+StandardMidArray[50]+16
                    &&slave_controlled_val==0x1fff)
                {
                    ramp_flag = 2;
                }
            }
            break;
        default:chassis_move_mode=NO_MOVE;break;
    }
    return NO;
}
//抽取某一行的誤差
int16 Attr_Error(uint8 image[height][width],uint8 line,int16 prop)
{
    uint8 Lef = 0,Rig = 0;
    int16 error,row=5;
    if (image[line][row]>Sepval)
    {
        for(;row<width-20;row=row+3)
        {
            if(image[line][row]>Sepval&&image[line][row+3]<=Sepval)
            {
                Lef = row;
                break;
            }
        }
    } else Lef = 5;
    row=width-5;
    if (image[line][row]>Sepval)
    {
        for(;row>20;row=row-2)
        {
            if(image[line][row-2]<=Sepval&&image[line][row]>Sepval)
            {
                Rig = row;
                break;
            }
        }
    } else Rig = width - 5;
    if (Lef&&Rig)
    {
        error = (int16)(Lef + Rig)/2 - middle;
        return (int16)(error*prop/5);
    }
    return 0;
}
ThreadStatus IsOut(uint8 image[height][width])
{
    if (fork_keep||flash.order[OUTROAD]==0)
    {
        if (fork_keep>0) fork_keep--;
        return NO;
    }
    if (!MaxWhite.line&&!MaxWhite.row&&100!=garage_flag)
    {
        chassis_move_mode = NO_MOVE;
        order.TrackType = OUTROAD;
        return TS;
    }
    return NO;
}
ThreadStatus IsGarage(uint8 image[height][width])
{
    if (flash.order[GARAGE]==0)    return NO;
    if ((mychassis.outGarage.dir==Right&&leflost.startline>=leflost.endline+10)
        ||(mychassis.outGarage.dir==Left&&riglost.startline>=riglost.endline+10))
    {
        uint8 count = TraverseLine(image,20,DEAFULT_TRAVERSELINE_MODE);
        count += TraverseLine(image,23,DEAFULT_TRAVERSELINE_MODE);
        count += TraverseLine(image,26,DEAFULT_TRAVERSELINE_MODE);
        if (count>=15)
        {
            if (mychassis.cyctime==FIRST)
            {
                garage_flag = 100;
                beep_time = 4;
            } else
            {
                garage_flag = 1;
                beep_time = 4;
            }
            order.TrackType = GARAGE;
            return TS;
        }
    }
    return NO;
}
ThreadStatus IsFork(uint8 image[height][width])
{
    if (flash.order[FORK]==0) return NO;
//    if (MaxWhite.line>=5&&
//        leflost.startline<=height-20&&riglost.startline<=height-20)
    if (MaxWhite.line>=2&&leflost.startline&&riglost.startline&&leflost.startline+riglost.startline>=leflost.endline+riglost.endline+10
    &&leflost.startline<=height-15&&riglost.startline<=height-15)
    {
        uint8_t t,lcount=0,rcount=0,time = 0;
        for (t = leflost.startline; t<height-3&&time<=10; t=t+3)
        {
            if (Line[t].Lefborder<Line[t+3].Lefborder)
            {
                lcount++;
            }
            time++;
        }
        for (t = riglost.startline,time = 0; t<height-3&&time<=10; t=t+3)
        {
            if (Line[t].Rigborder>Line[t+3].Rigborder)
            {
                rcount++;
            }
            time++;
        }
        if (lcount>=2&&rcount>=2&&(lcount+rcount>=5))
        {
            fork_flag = 1;
            beep_time = 5;
            order.TrackType = FORK;
            return TS;
        }
    }
    return NO;
}
ThreadStatus IsCross(uint8 image[height][width])
{
    if (flash.order[CROSS]==0)  return NO;
    if (leflost.startline>=leflost.endline+20&&riglost.startline>=riglost.endline+20)
    {
        cross_flag = 1;
        order.TrackType = CROSS;
        return TS;
    }
    return NO;
}
ThreadStatus IsRamp(uint8 image[height][width])
{
    if (flash.order[RAMP]==0)  return NO;
    if (slave_controlled_val==0x1fff
        &&Line[20].Rigborder>=Line[20].Lefborder+StandardMidArray[20]+20
        &&Line[30].Rigborder>=Line[30].Lefborder+StandardMidArray[30]+18
        &&Line[40].Rigborder>=Line[40].Lefborder+StandardMidArray[40]+16
        &&Line[50].Rigborder>=Line[50].Lefborder+StandardMidArray[50]+14)
    {
        ramp_flag = 1;
        beep_time = 5;
        order.TrackType = RAMP;
        return TS;
    }
    return NO;
}
ThreadStatus IsCircle(uint8 image[height][width])
{
    if (flash.order[CIRCLE]==0)    return NO;
    static uint8_t restime = 0;//限制检测时间
//    lcd_showuint8(0,5,circle_flag);
    if (circle_flag)
    {
        //检测成功
        if (Left==circle_type)
        {
            if (circle_flag==1&&image[height-1][5]>Sepval&&image[1][5]>Sepval&&image[40][5]<Sepval&&!riglost.startline)
            {
                xline = LinearRegression(leflost.startline-5,leflost.startline,Right);
                if (xline.slope>=44&&xline.slope<=77)
                {
                    circle_flag = 2;
                    restime = 0;
                }
            }
            if (circle_flag==2)
            {
                Position pos=FindPit(image,Left,leflost.endline+1);
                if (pos.line>=leflost.endline+2)
                {
                    circle_flag = 3;
                    beep_time = 5;
                    order.TrackType = CIRCLE;
                    return TS;
                }
            }
        } else
        {
            if (circle_flag==1&&image[height-1][width-5]>Sepval&&image[1][width-5]>Sepval&&image[40][width-5]<Sepval&&!leflost.startline)
            {
                xline = LinearRegression(riglost.startline-5,riglost.startline,Left);
                if (xline.slope<=-44&&xline.slope>=-77)
                {
                    circle_flag = 2;
                    restime = 0;
                }
            }
            if (circle_flag==2)
            {
                Position pos=FindPit(image,Right,riglost.endline+1);
                if (pos.line>=riglost.endline+2)
                {
                    circle_flag = 3;
                    beep_time = 5;
                    order.TrackType = CIRCLE;
                    return TS;
                }
            }
        }
        //检测限时
        restime++;
        if (restime>=100)
        {
            circle_flag = 0;
        }
    } else
    {
        restime = 0;
        circle_type = Mid;
        if (MaxWhite.line<=5)
        {   //左环判断
            if (leflost.startline>=leflost.endline+10&&leflost.startline<=height-10&&!riglost.startline)
            {
                xline = LinearRegression(leflost.startline-5,leflost.startline,Right);
                if (xline.slope>=44&&xline.slope<=87)
                {
                    int16 dec_point = (int16)(Line[leflost.startline].Rigborder-StandardMidArray[leflost.startline])/2;
                    if (dec_point>0&&(image[leflost.startline][dec_point]<=Sepval||image[leflost.startline][(dec_point/2)]<=Sepval))
                    {
                        circle_flag = 1;
                        circle_type = Left;
                        restime = 0;
                    }
                }
            }
            //右环判断
            if (riglost.startline>=riglost.endline+10&&riglost.startline<=height-10&&!leflost.startline)
            {
                xline = LinearRegression(riglost.startline-5,riglost.startline,Left);
                if (xline.slope<=-44&&xline.slope>=-87)
                {
                    int16 dec_point = (int16)(Line[riglost.startline].Lefborder+StandardMidArray[riglost.startline]+width)/2;
                    if (dec_point<width&&(image[riglost.startline][dec_point]<=Sepval||image[riglost.startline][(dec_point+width)/2]<=Sepval))
                    {
                        circle_flag = 1;
                        circle_type = Right;
                        restime = 0;
                    }
                }
            }
        }
    }
    return NO;
}
ErrorStatus GetMaxWhiteRow(uint8 image[height][width],mwl_config_t *config)
{
    int16 i,j;
    config->row = 0;config->line = height;
    if (config->StartSide==Left)
    {
        //Step1:确定搜索区间
        uint8 startrow=0,endrow=0;
        for (i = 1; i < width; i=i+1)
        {
            if (startrow&&image[height-1][i]<=Sepval)
            {
                if (i>=startrow+config->BLWidth)
                {
                    endrow = i;
                    break;
                } else startrow = 0;
            }
            if (!startrow&&image[height-1][i]>Sepval)
            {
                startrow = i;
            }
        }
        if (i>=width)
        {
            if (startrow&&i>=startrow+config->BLWidth)
            {
                endrow = i;
            } else
            {
                config->line = 0;
                config->row  = 0;
                return  ERROR;
            }
        }
        Line[height-1].Lefborder = startrow;
        Line[height-1].Rigborder = endrow-1;
        //Step2:区间内简单查找
        for (i = startrow; i < endrow; i=i+config->SkipRow)
        {
            for (j = height-1; j >= 0 ; j=j-config->SkipLine)
            {
                if (image[j][i]<=Sepval)
                    break;
            }
            if (j + config->SkipLine < config->line)
            {
                config->line = j + config->SkipLine;
                config->row = i;
                if (config->line==0)    break;
            }
        }
    } else if (config->StartSide==Right)
    {
        //Step1:确定搜索区间
        uint8 startrow=0,endrow=0;
        for (i = width-1; i > 0; i=i-1)
        {
            if (startrow&&image[height-1][i]<=Sepval)
            {
                if (i>=startrow+config->BLWidth)
                {
                    endrow = i;
                    break;
                } else startrow = 0;
            }
            if (!startrow&&image[height-1][i]>Sepval)
            {
                startrow = i;
            }
        }
        if (i>=width)
        {
            if (startrow&&i>=startrow+config->BLWidth)
            {
                endrow = i;
            } else
            {
                return  ERROR;
            }
        }
        Line[height-1].Lefborder = startrow;
        Line[height-1].Rigborder = endrow-1;
        //Step2:区间内简单查找
        for (i = startrow; i < endrow; i=i+config->SkipRow)
        {
            for (j = height-1; j >= 0 ; j=j-config->SkipLine)
            {
                if (image[j][i]<=Sepval)
                    break;
            }
            if (j + config->SkipLine < config->line)
            {
                config->line = j + config->SkipLine;
                config->row = i;
                if (config->line==0)    break;
            }
        }
    }
    return  SUCCESS;
}

uint8 GetSepLine(uint8 image[height][width],uint8 stopline)
{
    uint8 t;
    if (stopline<1) stopline = 1;
    for (t = height-1; t > stopline; t--)
    {
        if (image[t][1]>Sepval)
        {
            leflost.startline=t;
            break;
        }
    }
    for (; t > stopline; t--)
    {
        if (image[t][1]<=Sepval)
        {
            leflost.endline = t;
            break;
        }
    }
    if (leflost.startline)
    {
        if (t<=stopline) leflost.endline = stopline;
    }
    for (t = height-1; t > stopline; t--)
    {
        if (image[t][width-1]>Sepval)
        {
            riglost.startline = t;
            break;
        }
    }
    for (; t > stopline; t--)
    {
        if (image[t][width-1]<=Sepval)
        {
            riglost.endline = t;
            break;
        }
    }
    if (riglost.startline)
    {
        if (t<=stopline) riglost.endline = stopline;
    }

    //截止行判断
    if (!leflost.startline&&!riglost.startline)
    {
        return 1;
    }
//    if (leflost.endline>=10||riglost.endline>=10)
//    {
//        return max(leflost.endline,riglost.endline);
//    }
//    if ((leflost.startline&&leflost.startline<=leflost.endline+30&&leflost.endline<=10)
//            ||(riglost.startline&&riglost.startline<=riglost.endline+30&&riglost.endline<=10))
//    {
//        return max(leflost.startline,riglost.startline);
//    }
    return max(MaxWhite.line,1);
}
void GetMidLine()
{
    for (int8 i=height-1; i >= SepLine; i--)
    {
        Line[i].Midpoint = (int16) (Line[i].Lefborder + Line[i].Rigborder) / 2;
    }
}
//线性回归计算中线斜率
// y = Ax+B
FitLine LinearRegression(int startline,int endline,Side dir)
{
    uint8 t,SumLines=endline-startline+1;
    int x=0,y=0,xx=0,xy=0;
    int SumUp=0,SumDown=0;
    if (startline<0) startline=0;
    if (endline>height-1) endline=height-1;
    switch (dir)
    {
        case Mid:
            for(t=startline;t<=endline;t=t+1)
            {
                x+=t;
                y+=Line[t].Midpoint;
                xx+=t*t;
                xy+=t*Line[t].Midpoint;
            }
            break;
        case Left:
            for(t=startline;t<=endline;t=t+1)
            {
                x+=t;
                y+=Line[t].Lefborder;
                xx+=t*t;
                xy+=t*Line[t].Lefborder;
            }
            break;
        case Right:
            for(t=startline;t<=endline;t=t+1)
            {
                x+=t;
                y+=Line[t].Rigborder;
                xx+=t*t;
                xy+=t*Line[t].Rigborder;
            }
            break;
    }
    FitLine fline = {.slope = 0,.intercept = 0};
    SumUp = xy - x*y/SumLines;
    SumDown = xx - x*x/SumLines;
    if(SumUp==0)
        return fline;
    else
    {
        fline.slope = (int16)(SumUp*100/SumDown);
        if (fline.slope>=1000) fline.slope = 1000;
        if (fline.slope<=-1000) fline.slope = -1000;
        fline.intercept = y - fline.slope*x;
        return fline;
    }
}
uint16 Calculate_Dark(uint8 image[height][width])
{
    uint16 white_count=0;
    for (uint8 t = 0; t < height; t=t+2)
    {
        for (uint8 k = 1; k < width; k=k+5)
        {
            if (image[t][k]>Sepval)    white_count++;
        }
    }
    return white_count;
}

void BorderMakeUp(uint8 startline,uint8 endline,Side dir,Way_Of_MakeUp way)
{
    FitLine fitLine;
    int16 t;
    switch (dir)
    {
        case Mid:
            return;
            break;
        case Left:
            if (way==UP)
            {
                for (t = 1; t < 10; t++)
                {
                    if (endline-t<=0||Line[endline-t].Lefborder==LefX_Start)
                        break;
                }
                if  (t<=2) return;
                fitLine = LinearRegression(endline - t + 1, endline - 2, Left);
                if (fitLine.slope == 0) return;
            } else
            {
                for (t = 1; t < 10; t++)
                {
                    if (startline+t>height-1||Line[startline+t].Lefborder==LefX_Start)
                        break;
                }
                if  (t<=2) return;
                fitLine = LinearRegression(startline + 2, startline + t - 1, Left);
                if (fitLine.slope == 0) return;
            }
            for (t = startline; t >= endline; t--)
            {
                Line[t].Lefborder = (int16) (Line[startline+1].Lefborder + (t-startline-1) * fitLine.slope / 100);
//                Line[t].Lefborder = ((int16) (fitLine.intercept + t * fitLine.slope /100);
            }
            break;
        case Right:
            if (way==UP)
            {
                for (t = 1; t < 10; t++)
                {
                    if (endline-t<=0||Line[endline-t].Rigborder==RigX_End)
                        break;
                }
                if  (t<=2) return;
                fitLine = LinearRegression(endline - t + 1, endline - 2, Left);
                if (fitLine.slope == 0) return;
            } else
            {
                for (t = 1; t < 10; t++)
                {
                    if (startline+t>height-1||Line[startline+t].Rigborder==RigX_End)
                        break;
                }
                if  (t<=2) return;
                fitLine = LinearRegression(startline + 2, startline + t - 1, Left);
                if (fitLine.slope == 0) return;
            }
            for (t = startline; t >= endline; t--)
            {
                Line[t].Rigborder = (int16) (Line[startline+1].Rigborder + (t-startline-1) * fitLine.slope / 100);
//                Line[t].Rigborder = ((int16) (fitLine.intercept + t * fitLine.slope /100);
            }
            break;
    }
}

//mode: 0->以黑色为前景点 1->以白色为前景点
uint8 TraverseLine(uint8 image[height][width],uint8 lx,int16 startpoint,int16 endpoint,uint8 mode,uint8 num)
{
    int16 t;
    uint8 count=0;
    if (num<=0) num = 1;
    if (startpoint<0)   startpoint=0;   if (startpoint>width-1)   startpoint=width-1;
    if (endpoint<1)     endpoint=1;     if (endpoint>width-2)     endpoint=width-2;//-2防bug
    if (startpoint<=endpoint)
    {
        for (t = startpoint; t < endpoint; t=t+num)
        {
            if (mode)
            {
                if (image[lx][t]<=Sepval&&image[lx][t+1]<=Sepval&&image[lx][t+2]<=Sepval)
                {
                    mode=0;
                    count++;
                }
            } else
            {
                if (image[lx][t]>Sepval&&image[lx][t+1]>Sepval&&image[lx][t+2]>Sepval)
                {
                    mode=1;
                    count++;
                }
            }
        }
    } else
    {
        for (t = startpoint; t > endpoint; t=t-num)
        {
            if (mode)
            {
                if (image[lx][t]<=Sepval&&image[lx][t-1]<=Sepval&&image[lx][t-2]<=Sepval)
                {
                    mode=0;
                    count++;
                }
            } else
            {
                if (image[lx][t]>Sepval&&image[lx][t-1]>Sepval&&image[lx][t-2]>Sepval)
                {
                    mode=1;
                    count++;
                }
            }
        }
    }
    return count;
}
//mode: 0->以黑色为前景点 1->以白色为前景点
uint8 TraverseRow(uint8 image[height][width],uint8 ly,int16 startpoint,int16 endpoint,uint8 mode,uint8 num)
{
    int16 t;
    uint8 count=0;
    if (num<=0) num = 1;
    if (startpoint<0)   startpoint=0;   if (startpoint>height-1)   startpoint=height-1;
    if (endpoint<1)     endpoint=1;     if (endpoint>height-2)     endpoint=height-2;//-2防bug
    if (startpoint<=endpoint)
    {
        for (t = startpoint; t < endpoint; t=t+num)
        {
            if (mode)
            {
                if (image[t][ly]<=Sepval&&image[t+2][ly]<=Sepval)
                {
                    mode=0;
                    count++;
                }
            } else
            {
                if (image[t][ly]>Sepval&&image[t+2][ly]>Sepval)
                {
                    mode=1;
                    count++;
                }
            }
        }
    } else
    {
        for (t = startpoint; t > endpoint; t=t-num)
        {
            if (mode)
            {
                if (image[t][ly]<=Sepval&&image[t-1][ly]<=Sepval&&image[t-2][ly]<=Sepval)
                {
                    mode=0;
                    count++;
                }
            } else
            {
                if (image[t][ly]>Sepval&&image[t-1][ly]>Sepval&&image[t-2][ly]>Sepval)
                {
                    mode=1;
                    count++;
                }
            }
        }
    }
    return count;
}
//查找第i行的左边界
//mode:
//0:遍历查找;1:周围寻历;
int16 Lefmin,Lefmax,Rigmin,Rigmax;
int16 row=0;
void Findborder(uint8 image[height][width],uint8 lx,Side side,uint8 mode)
{
    if (mode)
    {
        if (side==Left)
        {
            if (image[lx][Line[lx+1].Lefborder]<=Sepval)
            {
                Lefmin = Line[lx+1].Lefborder;
                Lefmax = MaxWhite.row;
                for(row=Lefmin;row<Lefmax;row++)//再向右
                {
                    if(image[lx][row-1]<=Sepval&&image[lx][row]>Sepval&&image[lx][row+1]>Sepval)
                    {
                        Line[lx].Lefborder = row;
                        break;
                    }
                }
                if(row==Lefmax)//丢线补线
                {
                    Line[lx].Lefborder=row;
                }
            } else
            {
                Lefmin = LefX_Start;
                Lefmax = Line[lx+1].Lefborder;
                for(row=Lefmax;row>Lefmin;row--)
                {
                    if(image[lx][row-1]<=Sepval&&image[lx][row]>Sepval&&image[lx][row+1]>Sepval)
                    {
                        Line[lx].Lefborder=row;
                        break;
                    }
                }
                if(row==Lefmin)
                {
                    Line[lx].Lefborder=row;
                }
            }
        } else if (side==Right)
        {
            if (image[lx][Line[lx+1].Rigborder]<=Sepval)
            {
                Rigmin = MaxWhite.row;
                Rigmax = Line[lx+1].Rigborder;
                for(row=Rigmax;row>Rigmin;row--)
                {
                    if(image[lx][row-1]>Sepval&&image[lx][row]>Sepval&&image[lx][row+1]<=Sepval)
                    {
                        Line[lx].Rigborder=row;
                        break;
                    }
                }
                if(row==Rigmin)
                {
                    Line[lx].Rigborder=row;
                }
            } else
            {
                Rigmin = Line[lx+1].Rigborder;
                Rigmax = RigX_End;
                for(row=Rigmin;row<Rigmax;row++)
                {
                    if(image[lx][row-1]>Sepval&&image[lx][row]>Sepval&&image[lx][row+1]<=Sepval)
                    {
                        Line[lx].Rigborder = row;
                        break;
                    }
                }
                if(row==Rigmax)//丢线补线
                {
                    Line[lx].Rigborder=row;
                }
            }
        }
    } else
    {
        if (side==Left)
        {
            Lefmin = LefX_Start;
            Lefmax = MaxWhite.row;
            for(row=Lefmax;row>Lefmin;row--)    //先向左
            {
                if(image[lx][row-1]<=Sepval&&image[lx][row]>Sepval&&image[lx][row+1]>Sepval)
                {
                    Line[lx].Lefborder=row;
                    break;
                }
            }
            if(row==Lefmin)
            {
                Line[lx].Lefborder=row;
            }
        } else if (side==Right)
        {
            Rigmin = MaxWhite.row;
            Rigmax = RigX_End;
            for (row = Rigmin; row < Rigmax; row++)//再向右
            {
                if (image[lx][row-1]>Sepval && image[lx][row]>Sepval && image[lx][row+1] <=Sepval)
                {
                    Line[lx].Rigborder = row;
                    break;
                }
            }
            if (row == Rigmax)//丢线
            {
                Line[lx].Rigborder = row;
            }
        }
    }
}
//大津法求阈值
int OTSU()
{
    const uint8* image=mt9v03x_image[0];
    double w0 = 0.0;
    double w1 = 0.0;
    double u0_temp = 0.0;
    double u1_temp = 0.0;
    double u0 = 0.0;
    double u1 = 0.0;
    double delta_temp = 0.0;
    double delta_max = 0.0;

    //image灰度级
    uint32 pixel_count[256] = { 0 };
    float pixel_pro[256] = { 0 };
    int threshold = 0;
    //统计每个灰度级中像素的个数
    for (uint8 I = 0; I < MT9V03X_H; I++)
    {
        for (uint8 J = 1; J < MT9V03X_W; J++)
        {
            pixel_count[(int)image[I * MT9V03X_W + J]]++;//每个灰度级的像素数目
        }
    }
    //计算每个灰度级的像素数目占整幅图像的比例
    for (uint16 I = 0; I < 256; I++)
    {
        pixel_pro[I] = (float )pixel_count[I] / (MT9V03X_H * (MT9V03X_W-1));
    }
    //遍历灰度级[0,255],寻找合适的threshold
    for (uint16 I = 0; I < 256; I++)
    {
        w0 = w1 = u0_temp = u1_temp = u0 = u1 = delta_temp = 0;
        for (uint16 J = 0; J < 256; J++)
        {
            if (J <= I)   //背景部分
            {
                w0 += pixel_pro[J];//背景像素比例
                u0_temp += J * pixel_pro[J];
            }
            else   //前景部分
            {
                w1 += pixel_pro[J];//前景像素比例
                u1_temp += J * pixel_pro[J];
            }
        }
        u0 = u0_temp / w0;//背景像素点的平均灰度
        u1 = u1_temp / w1;//前景像素点的平均灰度
        delta_temp = (float )(w0 *w1* (u0 - u1)* (u0 - u1));//类间方差 g=w0*w1*(u0-u1)^2
        //当类间方差delta_temp最大时，对应的i就是阈值T
        if (delta_temp > delta_max)
        {
            delta_max = delta_temp;
            threshold = (int)I;
        }
    }
    return threshold;
}
void Threshold_Cal()
{

//    Sepval=PartThreshold+OTSU();//计算阈值
    Sepval=flash.sepval;//计算阈值
}
void Check_Error()
{
    if (chassis_move_mode==NO_MOVE)
    {
        //显示二值化图像
        lcd_displayimage032_zoom(mt9v03x_image[0],width,height,160-1,height-1,Sepval);
        //画线
        for(uint8 t=height-1;t>=SepLine;t--)
        {
            if (Line[t].Midpoint<0) Line[t].Midpoint=0;
            else if (Line[t].Midpoint>width-1) Line[t].Midpoint=width-1;
            lcd_drawpoint(Line[t].Midpoint*159/187,t,RED);
        }
        for(uint8 t=height-1;t>=SepLine;t--)
        {
            if (Line[t].Lefborder<0) Line[t].Lefborder=0;
            else if (Line[t].Lefborder>width-1) Line[t].Lefborder=width-1;
            lcd_drawpoint(Line[t].Lefborder*159/187, t, BLUE);
        }
        for(uint8 t=height-1;t>=SepLine;t--)
        {
            if (Line[t].Rigborder<0) Line[t].Rigborder=0;
            else if (Line[t].Rigborder>width-1) Line[t].Rigborder=width-1;
            lcd_drawpoint(Line[t].Rigborder*159/187, t, PURPLE);
        }
        for(uint8 t=0;t<160;t=t+5)           lcd_drawpoint(t,SepLine,RED);
        for (uint8 t = 0; t < height; ++t)      lcd_drawpoint((MaxWhite.row*159/187),t,GREEN);
        uint8 t;
        for (t = 0; t < 80; ++t)      lcd_drawpoint(t,leflost.startline,GREEN);
        for (; t < 160; ++t)    lcd_drawpoint(t,riglost.startline,BROWN);
        for (t = 0; t < 80; ++t)      lcd_drawpoint(t, leflost.endline,GREEN);
        for (; t < 160; ++t)    lcd_drawpoint(t, riglost.endline,BROWN);
        lcd_showuint8(0,0,(leflost.startline-leflost.endline));
        lcd_showuint8(136,0,(riglost.startline-riglost.endline));

        //清空
        switch (order.TrackType)
        {
            case NORMAL:lcd_showstr(0,7,"Normal  ");
                break;
            case CIRCLE:
                lcd_showstr(0,7,"Circle  ");
                lcd_showuint8(0,5,circle_flag);
                break;
            case CROSS:lcd_showstr(0,7,"Cross  ");
                break;
            case GARAGE:lcd_showstr(0,7,"Garage ");
                break;
            case FORK:lcd_showstr(0,7,"Fork   ");
                break;
            case RAMP:lcd_showstr(0,7,"Ramp   ");
                break;
            case OUTROAD:
                break;
        }
        lcd_showint16(32,4,Cam_Error);
        lcd_showint16(80,4,slave_error_val);
    }


}
void Schedule_Init()
{
    uint8 t;
    uint8 res = 0;
    uint8 no,max_val;
    for (uint8_t cnt = 0; cnt < THREAD_NUM; cnt++)
    {
        max_val = 0;
        no = 0;
        for (t = 0; t < 6; t++)
        {
            if (!(res&(1<<t))&&max_val<=flash.order[t])
            {
                no = t;
                max_val = flash.order[t];
            }
        }
        res |= (1<<no);
        switch (no)
        {
            case 0:order.Schedule[cnt] = IsCircle;break;
            case 1:order.Schedule[cnt] = IsGarage;break;
            case 2:order.Schedule[cnt] = IsCross;break;
            case 3:order.Schedule[cnt] = IsFork;break;
            case 4:order.Schedule[cnt] = IsRamp;break;
            case 5:order.Schedule[cnt] = IsOut;break;
        }
    }
}
