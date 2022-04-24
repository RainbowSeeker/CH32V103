//
// Created by 杨宇 on 2021/4/13.
//
#include "image_process.h"
#define Stop_Move()             \
{                               \
    for(uint8 t=0;t<4;t++)      \
    {                           \
        Setduty(&motor[t],0);   \
    }                           \
}

uint8 StandardLefArray[height]=
        {
                58, 57, 57, 56, 55, 54, 54, 53, 52, 52, 51, 50, 50, 49, 48, 48, 47, 46, 46, 45, 44, 43, 42, 42, 41, 40,
                39, 39, 38, 38, 37, 36, 36, 35, 34, 34, 33, 32, 32, 31, 30, 30, 29, 29, 28, 28, 27, 26, 26, 25, 25, 25,
                24, 24, 23, 23, 22, 21, 21, 20, 20, 19, 19, 18, 18, 17, 17, 16, 16, 15
        };
uint8 StandardRigArray[height]=
        {
                127, 128, 128, 129, 130, 130, 131, 132, 133, 133, 134, 134, 135, 136, 137, 137, 138, 139, 139, 140, 141,
                141, 142, 143, 143, 144, 144, 145, 146, 146, 147, 147, 148, 149, 149, 150, 151, 151, 152, 153, 153, 154,
                154, 155, 156, 156, 157, 157, 158, 159, 159, 160, 160, 161, 162, 162, 163, 163, 164, 164, 165, 165, 166,
                167, 167, 168, 168, 169, 169, 170
        };
uint8 StandardMidArray[height]=
        {
                71, 71, 71, 73, 75, 76, 77, 79, 81, 81, 83, 84, 85, 87, 89, 89, 91, 93, 93, 95, 97, 98, 100, 101, 102,
                104, 105, 106, 108, 108, 110, 111, 112, 114, 115, 116, 118, 119, 120, 122, 123, 124, 125, 126, 128, 128,
                130, 131, 132, 134, 134, 135, 136, 137, 139, 139, 141, 142, 143, 144, 145, 146, 147, 149, 149, 151, 151,
                153, 153, 155
        };
//-------------------------------------------------------------
//标志位
uint8   garage_flag=0;
uint8   ramp_flag=0;
uint8   fork_flag=0;
uint8   circle_flag=0;//圆环进出标志位
LostInfo leflost={.startline=0,.endline=0},riglost={.startline=0,.endline=0};
//外部引用变量
Side Go_Dir;                    //出发方向
Road_Type Track_Type=NORMAL;
uint8 Sepval=0;     //二值化阈值
uint8 SepLine=1;
int16 tem_error=0;
//-------------------------------------------------------------
//内部使用变量
LineInfo Line[height]={0};      //记录行
FitLine xline;                  //记录斜线信息
//uint8 open_cir = 0;
mwl_config_t MaxWhite =  //记录最大白列
        {
            .StartSide  = Left,
            .line       = 0,
            .row        = 0,
            .SkipLine   = 3,
            .SkipRow    = 12,
            .Jump       = WTB,
            .BLWidth    = 20
        };
#if WAY_OF_SEARCH == 0
uint8 LefSearchMode = 1;
uint8 RigSearchMode = 1;
#endif
//!!!图像处理and判断函数!!!
void Image_Process(uint8 image[height][width])
{
    tem_error = 0;
    TrackScan(image);

    uint16 dark = Calculate_Dark(image);
    if (dark>=1000&&0==fork_flag&&0xaa==data_return)
    {
        fork_flag = 1;
    }
    lcd_showuint16(50, 2, dark);
    if (fork_flag==1)
    {
        uint8_t lef=0,rig=0,t;
        for (t = height-1; t > 1; t=t-2)
        {
            if (!lef&&image[t][47]>Sepval&&image[t-2][47]<=Sepval)
            {
                lef = t - 1;
            }
            if (!rig&&image[t][141]>Sepval&&image[t-2][141]<=Sepval)
            {
                rig = t - 1;
            }
        }
        if (lef&&rig&&(lef+rig>36))   fork_flag = 2;
        
        lcd_showuint16(50, 3, lef+rig);
    }

    if (2==fork_flag)
    {
        static uint16 wtime = 100;
        if (wtime>0)
        {
            wtime--;
        }else
        {
            fork_flag = 0;
            wtime = 100;
        }
    }
}
void TrackScan(uint8 image[height][width])
{
    int16 i=height-1;
    GetMaxWhiteRow(image,&MaxWhite);
    //扫描最后一行(直接遍历)
    Findborder(image,height-1,Left,0);
    Findborder(image,height-1,Right,0);
    //扫描剩余行
    for(i=height-2;i>=SepLine;i--)
    {
        Findborder(image,i,Left ,1);
        Findborder(image,i,Right,1);
    }
}
ErrorStatus GetMaxWhiteRow(uint8 image[height][width],mwl_config_t *config)
{
    int16 i,j;
    config->row = 0;config->line = height;
    if (config->StartSide==Left)
    {
        if (config->Jump==WTB)
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
        } else if (config->Jump==NON)
        {
            for (i = 1; i < width; i=i+config->SkipRow)
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
            for (j = height-1; j >= 0 ; j=j-config->SkipLine)
            {
                if (image[j][RigX_End]<=Sepval)
                    break;
            }
            if (j + config->SkipLine < config->line)
            {
                config->line = j + config->SkipLine;
                config->row = RigX_End;
            }
        }
    } else if (config->StartSide==Right)
    {
        if (config->Jump==WTB)
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
        } else if (config->Jump==NON)
        {
            for (i = 1; i < width; i=i+config->SkipRow)
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
            for (j = height-1; j >= 0 ; j=j-config->SkipLine)
            {
                if (image[j][RigX_End]<=Sepval)
                    break;
            }
            if (j + config->SkipLine < config->line)
            {
                config->line = j + config->SkipLine;
                config->row = RigX_End;
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
                if (image[t][ly]<=Sepval&&image[t+1][ly]<=Sepval&&image[t+2][ly]<=Sepval)
                {
                    mode=0;
                    count++;
                }
            } else
            {
                if (image[t][ly]>Sepval&&image[t+1][ly]>Sepval&&image[t+2][ly]>Sepval)
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
#if WAY_OF_SEARCH == 0
            Lefmin=Line[lx+1].Lefborder-excursion;
            if (Lefmin<=0)  Lefmin=1;
            Lefmax=Line[lx+1].Lefborder+excursion;
            if (Lefmax>MaxWhite.row)  Lefmax=MaxWhite.row;
            for(row=Lefmax;row>Lefmin;row--)    //先向左
            {
                if(image[lx][row-1]<=Sepval&&image[lx][row]<=Sepval&&image[lx][row+1]>Sepval)
                {
                    Line[lx].Lefborder=row;
                    break;
                }
            }
            if(row==Lefmin)//丢线补线
            {
                Line[lx].Lefborder=row;
                LefSearchMode = 0;
            } else
            {
                LefSearchMode = 1;
            }
#else
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
#endif
        } else if (side==Right)
        {
#if WAY_OF_SEARCH == 0
            Rigmin=Line[lx+1].Rigborder-excursion;
            if (Rigmin<MaxWhite.row) Rigmin=MaxWhite.row;
            Rigmax=Line[lx+1].Rigborder+excursion;
            if (Rigmax>=width) Rigmax=width-1;
            for(row=Rigmin;row<Rigmax;row++)//再向右
            {
                if(image[lx][row-1]>Sepval&&image[lx][row]<=Sepval&&image[lx][row+1]<=Sepval)
                {
                    Line[lx].Rigborder=row;
                    break;
                }
            }
            if(row==Rigmax)//丢线补线
            {
                Line[lx].Rigborder=row;
                RigSearchMode = 0;
            } else
            {
                RigSearchMode = 1;
            }
#else
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
#endif
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

#ifdef USE_MASTER
//    Sepval=PartThreshold+OTSU();//计算阈值
    Sepval=flash.sepval;//计算阈值
#else
    Sepval=130;
#endif
//    Sepval=150;
//    Sepval=110;
}
