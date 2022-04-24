//
// Created by 杨宇 on 2021/5/25.
//

#include "main.h"

flash_storage_t memory[4]=
        {       //低速版本
                {
                        .vx_speed   = 80,
                        .vy_speed   = 160,
                        .dir_kp     = 16,
                        .dir_kd     = 5,
                        .speed_kp   = 170,
                        .speed_ki   = 20,
                        .fork_kp    = 12,
                        .fork_kd    = 8,
                        .add_wz     = 0,
                        .maxspeed   = 250,
                        .out_speed  = 80,
                        .out_time   = 40,
                        .rotime[0]  = 28,    //第二次三叉
                        .rotime[1]  = 18,    //第一次出三叉
                        .rotime[2]  = 0,     //第二次出三叉
                        .rospeed    = 200,
                        .cirspeed   = 160,
                        .garspeed   = 160,
                        .ramspeed   = 100,
                        .forkspeed  = 100,
                        .two        = 0
                },
                //中速版本
                {
                        .vx_speed   = 110,
                        .vy_speed   = 230,
                        .speed_kp   = 170,
                        .speed_ki   = 20,
                        .dir_kp     = 25,
                        .dir_kd     = 10,
                        .fork_kp    = 18,
                        .fork_kd    = 8,
                        .add_wz     = 2,
                        .maxspeed   = 320,
                        .out_speed  = 80,
                        .out_time   = 40,
                        .rotime[0]  = 28,    //第二次三叉
                        .rotime[1]  = 18,    //第一次出三叉
                        .rotime[2]  = 0,     //第二次出三叉
                        .rospeed    = 200,
                        .cirspeed   = 230,
                        .garspeed   = 200,
                        .ramspeed   = 120,
                        .forkspeed  = 150,
                        .two        = 0
                },
                //高速版本
                {
                        .vx_speed   = 140,
                        .vy_speed   = 270,
                        .speed_kp   = 170,
                        .speed_ki   = 20,
                        .dir_kp     = 40,
                        .dir_kd     = 20,
                        .fork_kp    = 20,
                        .fork_kd    = 10,
                        .add_wz     = 2,
                        .maxspeed   = 340,
                        .out_speed  = 80,
                        .out_time   = 40,
                        .rotime[0]  = 28,    //第二次三叉
                        .rotime[1]  = 18,    //第一次出三叉
                        .rotime[2]  = 0,     //第二次出三叉
                        .rospeed    = 200,
                        .cirspeed   = 270,
                        .garspeed   = 200,
                        .ramspeed   = 100,
                        .forkspeed  = 150,
                        .two        = 0
                },
                //决赛版本
                {
                        .vx_speed   = 180,
                        .vy_speed   = 300,
                        .speed_kp   = 190,
                        .speed_ki   = 30,
                        .dir_kp     = 35,
                        .dir_kd     = 20,
                        .fork_kp    = 40,
                        .fork_kd    = 20,
                        .add_wz     = 2,
                        .maxspeed   = 340,
                        .out_speed  = 80,
                        .out_time   = 40,
                        .rotime[0]  = 28,    //第二次三叉
                        .rotime[1]  = 18,    //第一次出三叉
                        .rotime[2]  = 0,     //第二次出三叉
                        .rospeed    = 200,
                        .cirspeed   = 300,
                        .garspeed   = 200,
                        .ramspeed   = 100,
                        .forkspeed  = 140,
                        .two        = 1
                }
        };
