//
// Created by 杨宇 on 2021/5/9.
//

#include "flash.h"
#include "main.h"

//第一次存入到的flash中的值
flash_storage_t  flash =
        {
            //改变header则可重新存入下列值
            .header     = 0xafff,
            .sepval     = 130,
            .vx_speed   = NORMAL_SPEED_VX,
            .vy_speed   = NORMAL_SPEED_VY,      //240
            .dir_kp     = NORMAL_DIR_KP,        //35
            .dir_kd     = NORMAL_DIR_KD,        //15
            .speed_kp   = MOTOR_SPEED_KP,
            .speed_ki   = MOTOR_SPEED_KI,
            .fork_kp    = FORK_DIR_KP,
            .fork_kd    = FORK_DIR_KD,
            .add_wz     = CHASSIS_WZ_SET_SACLE,
            .maxspeed   = MAX_WHEEL_SPEED,
            .out_speed  = 80,
            .out_time   = 40,
            .rotime[0]  = 25,    //第二次三叉
            .rotime[1]  = 19,    //第一次出三叉
            .rotime[2]  = 5,     //第二次出三叉
            .rospeed    = 200,
            .cirspeed   = NORMAL_SPEED_VY,
            .garspeed   = 170,
            .ramspeed   = 100,
            .forkspeed  = 80,
            .order[CIRCLE]   = 8,    //环岛优先级 (==0则关闭)
            .order[GARAGE]   = 20,    //入库优先级 (==0则关闭)
            .order[CROSS]    = 0,    //十字优先级 (==0则关闭)
            .order[FORK]     = 5,    //三叉优先级 (==0则关闭)
            .order[RAMP]     = 0,    //坡道优先级 (==0则关闭)
            .order[OUTROAD]  = 0,    //出界优先级 (==0则关闭)
            .inrotime   = INGAR_ROTIME,
            .ingotime   = INGAR_GOTIME,
            .one        = 16,       //20
            .two        = 0
        };


void Flash_Init()
{
    uint32 read_buff[FLASH_STORAGE_SIZE];
    //将第15个扇区的第3页 读取到read_buff中
    flash_page_read(FLASH_SECTION_15, FLASH_PAGE_3, read_buff, FLASH_STORAGE_SIZE);
    //如果第一个字节为header,则读取flash中数据
    if (flash.header==read_buff[HEADER])
    {
        flash.sepval    = read_buff[SEP_VAL];
        flash.vx_speed  = read_buff[SPEED]>>16;
        flash.vy_speed  = read_buff[SPEED]&0xffff;
        flash.dir_kp    = read_buff[DIR_PD]>>16;
        flash.dir_kd    = read_buff[DIR_PD]&0xffff;
        flash.fork_kp   = read_buff[FORK_PD]>>16;
        flash.fork_kd   = read_buff[FORK_PD]&0xffff;
        flash.speed_kp  = read_buff[SPEED_PI]>>16;
        flash.speed_ki  = read_buff[SPEED_PI]&0xffff;
        flash.out_speed = read_buff[OUT_GARAGE]>>16;
        flash.out_time  = read_buff[OUT_GARAGE]&0xffff;
        flash.one       = read_buff[ONETWO]>>16;
        flash.two       = read_buff[ONETWO]&0xffff;
        flash.rotime[0] = read_buff[ROTA_IN]>>16;
        flash.rotime[1] = read_buff[ROTA_IN]&0xffff;
        flash.rotime[2] = read_buff[ROTA_OUT]>>16;
        flash.rospeed   = read_buff[ROTA_OUT]&0xffff;
        flash.cirspeed  = read_buff[CIR_GARA_SPEED]>>16;
        flash.garspeed  = read_buff[CIR_GARA_SPEED]&0xffff;
        flash.ramspeed  = read_buff[RAM_FORK_SPEED]>>16;
        flash.forkspeed = read_buff[RAM_FORK_SPEED]&0xffff;
        flash.add_wz    = read_buff[ADD_WZ];
        flash.maxspeed  = read_buff[MAX_SPEED];
        flash.order[0]  = read_buff[PRIORITY_0]>>16;
        flash.order[1]  = read_buff[PRIORITY_0]&0xffff;
        flash.order[2]  = read_buff[PRIORITY_1]>>16;
        flash.order[3]  = read_buff[PRIORITY_1]&0xffff;
        flash.order[4]  = read_buff[PRIORITY_2]>>16;
        flash.order[5]  = read_buff[PRIORITY_2]&0xffff;
        flash.inrotime  = read_buff[INGAR_TIME]>>16;
        flash.ingotime  = read_buff[INGAR_TIME]&0xffff;
    } else
    {
        Flash_IN(&flash);
    }  
}

void Flash_IN(flash_storage_t  *fpoint)
{
    uint32 write_buff[FLASH_STORAGE_SIZE];
    write_buff[HEADER]    = fpoint->header;
    write_buff[SEP_VAL]   = fpoint->sepval;
    write_buff[SPEED]     = ((uint32)fpoint->vx_speed<<16)|fpoint->vy_speed;
    write_buff[DIR_PD]    = ((uint32)fpoint->dir_kp<<16)|fpoint->dir_kd;
    write_buff[FORK_PD]   = ((uint32)fpoint->fork_kp<<16)|fpoint->fork_kd;
    write_buff[SPEED_PI]  = ((uint32)fpoint->speed_kp<<16)|fpoint->speed_ki;
    write_buff[ROTA_IN]   = ((uint32)fpoint->rotime[0]<<16)|fpoint->rotime[1];
    write_buff[ROTA_OUT]  = ((uint32)fpoint->rotime[2]<<16)|fpoint->rospeed;
    write_buff[OUT_GARAGE]= ((uint32)fpoint->out_speed<<16)|fpoint->out_time;
    write_buff[ONETWO]    = ((uint32)fpoint->one<<16)|fpoint->two;
    write_buff[ADD_WZ]    = fpoint->add_wz;
    write_buff[MAX_SPEED] = fpoint->maxspeed;
    write_buff[PRIORITY_0]= ((uint32)fpoint->order[0]<<16)|fpoint->order[1];
    write_buff[PRIORITY_1]= ((uint32)fpoint->order[2]<<16)|fpoint->order[3];
    write_buff[PRIORITY_2]= ((uint32)fpoint->order[4]<<16)|fpoint->order[5];
    write_buff[RAM_FORK_SPEED] = ((uint32)fpoint->ramspeed<<16)|fpoint->forkspeed;
    write_buff[CIR_GARA_SPEED] = ((uint32)fpoint->cirspeed<<16)|fpoint->garspeed;
    write_buff[INGAR_TIME]= ((uint32)fpoint->inrotime<<16)|fpoint->ingotime;
    //将write_buff中的数据写入第15个扇区的第3页
    flash_page_program(FLASH_SECTION_15, FLASH_PAGE_3, write_buff, FLASH_STORAGE_SIZE);
}

