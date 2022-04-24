#include "menu.h"

uint8 Mode_Flag = 1;

PIN_enum KEY_PTxn[5] = {C7, B12, A8, C6, A11};  //定义五向开关引脚


//----------------按键初始化-----------------//
void key_init(uint8 key_num)
{
  while(key_num--)
    gpio_init(KEY_PTxn[key_num], GPI, 1, IN_PULLUP);
}


//----------------按键检测-----------------//
KEY_STATUS key_check(KEY key)
{
    if(gpio_get(KEY_PTxn[key]) == KEY_DOWN)
    {
        systick_delay_ms(100);   //延时消抖
        if( gpio_get(KEY_PTxn[key]) == KEY_DOWN)
        {
            return KEY_DOWN;
        }
    }
    return KEY_UP;
}

//菜单初始化
void Menu_Init()
{
    key_init(5);
    Menu();
}

//一级菜单
void Menu()
{
  uint8 j=0,jj=0;
  DisplayMenu();
  while(j!=10)
  {
    Move_Up_Down(&j,5);
    if(key_check(KEY_A)==KEY_DOWN)
      switch (j)
      {
          case 0:j=10;Go(&jj);Flash_IN(&flash);if(jj == 0)j = 0;break;
          case 1:Set_PID();Flash_IN(&flash);break;
          case 2:Set_Speed();Flash_IN(&flash);break;
          case 3:Set_Priority();Flash_IN(&flash);break;
          case 4:Read_Para();Flash_IN(&flash);break;
          case 5:Camera();Flash_IN(&flash);break;
      }
  }
    lcd_clear(WHITE);
}
void Set_Priority()
{
    lcd_clear(WHITE);
    uint8 j=0;
    lcd_showstr(5,0,"CIRC");
    lcd_showstr(5,1,"GARA");
    lcd_showstr(5,2,"CROS");
    lcd_showstr(5,3,"FORK");
    lcd_showstr(5,4,"RAMP");
    lcd_showstr(5,5,"OUT");
    lcd_showint16(55,0, flash.order[0]);
    lcd_showint16(55,1, flash.order[1]);
    lcd_showint16(55,2, flash.order[2]);
    lcd_showint16(55,3, flash.order[3]);
    lcd_showint16(55,4, flash.order[4]);
    lcd_showint16(55,5, flash.order[5]);
    while(1)
    {
        Move_Up_Down(&j,5);

        if (key_check(KEY_L) == KEY_DOWN)                                     //减小
        {
            switch (j) {
                case 0:
                    if (flash.order[0]>0) flash.order[0] --;
                    lcd_showint16( 55, 0, flash.order[0]);
                    break;
                case 1:
                    if (flash.order[1]>0) flash.order[1]--;
                    lcd_showint16(55, 1, flash.order[1]);
                    break;
                case 2:
                    if (flash.order[2]>0) flash.order[2]--;
                    lcd_showint16(55, 2, flash.order[2]);
                    break;
                case 3:
                    if (flash.order[3]>0) flash.order[3]--;
                    lcd_showint16( 55, 3, flash.order[3]);
                    break;
                case 4:
                    if (flash.order[4]>0) flash.order[4]--;
                    lcd_showint16( 55, 4, flash.order[4]);
                    break;
                case 5:
                    if (flash.order[5]>0) flash.order[5] --;
                    lcd_showint16(55, 5, flash.order[5]);
                    break;
            }
        } else if (key_check(KEY_R) == KEY_DOWN)                                     //增大
        {
            switch (j) {
                case 0:
                    if (flash.order[0]<100) flash.order[0] ++;
                    lcd_showint16( 55, 0, flash.order[0]);
                    break;
                case 1:
                    if (flash.order[1]<100) flash.order[1]++;
                    lcd_showint16(55, 1, flash.order[1]);
                    break;
                case 2:
                    if (flash.order[2]<100) flash.order[2]++;
                    lcd_showint16(55, 2, flash.order[2]);
                    break;
                case 3:
                    if (flash.order[3]<100) flash.order[3]++;
                    lcd_showint16( 55, 3, flash.order[3]);
                    break;
                case 4:
                    if (flash.order[4]<100) flash.order[4]++;
                    lcd_showint16( 55, 4, flash.order[4]);
                    break;
                case 5:
                    if (flash.order[5]<100) flash.order[5] ++;
                    lcd_showint16(55, 5, flash.order[5]);
                    break;
            }
        }
//        while(k--)
//        {
//            if(flash.order[k] < 0)
//                lcd_showint16(55,k,0);
//        }
        if (key_check(KEY_A) == KEY_DOWN)
        {
            lcd_clear(WHITE);
            DisplayMenu();
            break;
        }
    }

}
void Go(uint8 *jj)
{
    uint8 j=0,x=0,y=0;
    lcd_clear(WHITE);
    lcd_showstr(x,y,"1.Go Directly  ");
    lcd_showstr(x,++y,"2.Go With Garage");
    lcd_showstr(x,++y,"3.Back");
    lcd_showstr(130,j,"*");
    while(1)
    {
        Move_Up_Down(&j,2);
        if(key_check(KEY_A)==KEY_DOWN)
        {
            switch (j)
            {
                case 0:*jj = 10;out_gar = 0;break;
                case 1:*jj = 10;out_gar = 1;break;
                case 2:*jj = 0;lcd_clear(WHITE);DisplayMenu();break;
            }
            break;
        }
    }
}

//---------------------------修改PID-------------------------//
void Set_PID()
{
    uint8 j=0,x=0;
    lcd_clear(WHITE);

    while(1)
    {
        Move_Up_Down(&j,7);
        if(Mode_Flag)
        {
            lcd_showstr(x + 5, 0, "Straight");
            lcd_showstr(x + 5, 1,"Speed");
            lcd_showstr(x + 5, 2, "Sp");
            lcd_showstr(x + 5, 3, "Si");
            lcd_showstr(x + 5, 4, "Dp");
            lcd_showstr(x + 5, 5, "Dd");
            lcd_showstr(x + 5, 6, "One ");
            lcd_showstr(x + 5, 7, "Rot");
            lcd_showint16(x + 50, 1, flash.vy_speed);
            lcd_showint16(x + 50, 2, flash.speed_kp);
            lcd_showint16(x + 50, 3, flash.speed_ki);
            lcd_showint16(x + 50, 4, flash.dir_kp);
            lcd_showint16(x + 50, 5, flash.dir_kd);
            lcd_showint16(x + 50, 6, flash.one);
            lcd_showint16(x + 50, 7, flash.rotime[0]);
//-------------------------左右移动改大小--------------------------//
            if (key_check(KEY_L) == KEY_DOWN)                                     //减小
            {
                switch (j) {
                    case 0:Mode_Flag = 0;break;
                    case 1:
                        flash.vy_speed -= 10;
                        lcd_showint16(x + 50, 1, flash.vy_speed);
                        break;
                    case 2:
                        flash.speed_kp--;
                        lcd_showint16(x + 50, 2, flash.speed_kp);
                        break;
                    case 3:
                        flash.speed_ki--;
                        lcd_showint16(x + 50, 3, flash.speed_ki);
                        break;
                    case 4:
                        flash.dir_kp--;
                        lcd_showint16(x + 50, 4, flash.dir_kp);
                        break;
                    case 5:
                        flash.dir_kd--;
                        lcd_showint16(x + 50, 5, flash.dir_kd);
                        break;
                    case 6:
                        flash.one --;
                        lcd_showint16(x + 50, 6, flash.one);
                        break;
                    case 7:
                        flash.rotime[0] --;
                        lcd_showint16(x + 50, 7, flash.rotime[0]);
                        break;
                }
            } else if (key_check(KEY_R) == KEY_DOWN)                                     //增大
            {
                switch (j) {
                    case 0:Mode_Flag = 0;break;
                    case 1:
                        flash.vy_speed += 10;
                        lcd_showint16(x + 50, 1, flash.vy_speed);
                        break;
                    case 2:
                        flash.speed_kp++;
                        lcd_showint16(x + 50, 2, flash.speed_kp);
                        break;
                    case 3:
                        flash.speed_ki++;
                        lcd_showint16(x + 50, 3, flash.speed_ki);
                        break;
                    case 4:
                        flash.dir_kp++;
                        lcd_showint16(x + 50, 4, flash.dir_kp);
                        break;
                    case 5:
                        flash.dir_kd++;
                        lcd_showint16(x + 50, 5, flash.dir_kd);
                        break;
                    case 6:
                        flash.one ++ ;
                        lcd_showint16(x + 50, 6, flash.one);
                        break;
                    case 7:
                        flash.rotime[0] ++;
                        lcd_showint16(x + 50, 7, flash.rotime[0]);
                        break;
                }
            }

        }
        else
        {
            lcd_showstr(x + 5, 0, "Across  ");
            lcd_showstr(x + 5, 1,"Speed");
            lcd_showstr(x + 5, 2, "Ot");
            lcd_showstr(x + 5, 3, "Os");
            lcd_showstr(x + 5, 4, "Dp");
            lcd_showstr(x + 5, 5, "Dd");
            lcd_showstr(x + 5, 6, "Rot");
            lcd_showstr(x + 5, 7, "Rsp");

            lcd_showint16(x + 50, 1, flash.vx_speed);
            lcd_showint16(x + 50, 2, flash.out_time);
            lcd_showint16(x + 50, 3, flash.out_speed);
            lcd_showint16(x + 50, 4, flash.fork_kp);
            lcd_showint16(x + 50, 5, flash.fork_kd);
            lcd_showint16(x + 50, 6, flash.rotime[1]);
            lcd_showint16(x + 50, 7, flash.rospeed);
            if (key_check(KEY_L) == KEY_DOWN)                                     //减小
            {
                switch (j) {
                    case 0:Mode_Flag = 1;break;
                    case 1:
                        flash.vx_speed -= 10;
                        lcd_showint16(x + 50, 1, flash.vx_speed);
                        break;
                    case 2:
                        flash.out_time--;
                        lcd_showint16(x + 50, 2, flash.out_time);
                        break;
                    case 3:
                        flash.out_speed--;
                        lcd_showint16(x + 50, 3, flash.out_speed);
                        break;
                    case 4:
                        flash.fork_kp--;
                        lcd_showint16(x + 50, 4, flash.fork_kp);
                        break;
                    case 5:
                        flash.fork_kd--;
                        lcd_showint16(x + 50, 5, flash.fork_kd);
                        break;
                    case 6:
                        flash.rotime[1] --;
                        lcd_showint16(x + 50, 6, flash.rotime[1]);
                        break;
                    case 7:
                        flash.rospeed--;
                        lcd_showint16(x + 50, 7, flash.rospeed);
                        break;
                }
            } else if (key_check(KEY_R) == KEY_DOWN)                                     //增大
            {
                switch (j) {
                    case 0:Mode_Flag = 1;break;
                    case 1:
                        flash.vx_speed += 10;
                        lcd_showint16(x + 50, 1, flash.vx_speed);
                        break;
                    case 2:
                        flash.out_time++;
                        lcd_showint16(x + 50, 2, flash.out_time);
                        break;
                    case 3:
                        flash.out_speed++;
                        lcd_showint16(x + 50, 3, flash.out_speed);
                        break;
                    case 4:
                        flash.fork_kp++;
                        lcd_showint16(x + 50, 4, flash.fork_kp);
                        break;
                    case 5:
                        flash.fork_kd++;
                        lcd_showint16(x + 50, 5, flash.fork_kd);
                        break;
                    case 6:
                        flash.rotime[1] ++ ;
                        lcd_showint16(x + 50, 6, flash.rotime[1]);
                        break;
                    case 7:
                        flash.rospeed ++ ;
                        lcd_showint16(x + 50, 7, flash.rospeed);
                        break;
                }
            }
        }
        if (key_check(KEY_A) == KEY_DOWN)
        {
            DisplayMenu();
            break;
        }
    }
}



/*摄像头显示*/
void Camera()
{
    lcd_clear(WHITE);
    EnableGlobalIRQ(0);
    while(1)
    {
        mt9v03x_finish_flag=0;
        while (mt9v03x_finish_flag==0);
        //显示二值化图像
        lcd_displayimage032_zoom(mt9v03x_image[0],width,height,160-1,height-1,Sepval);
        lcd_showuint8(0,5,Sepval);
        if(key_check(KEY_A)==KEY_DOWN)
        {
            DisplayMenu();
            flash.sepval = Sepval;
            DisableGlobalIRQ();
            break;
        }
        if (key_check(KEY_L)==KEY_DOWN)
        {
            Sepval--;
        }
        if (key_check(KEY_R)==KEY_DOWN)
        {
            Sepval++;
        }
    }
}


//---------------------------修改设定速度-------------------------//
void Set_Speed()
{
    uint8 j=0;
    lcd_clear(WHITE);

    lcd_showstr(5,0,"Cir");
    lcd_showint16(55,0,flash.cirspeed);

    lcd_showstr(5,1,"Gar");
    lcd_showint16(55,1,flash.garspeed);

    lcd_showstr(5,2,"Ram     ");
    lcd_showint16(55,2,flash.ramspeed);

    lcd_showstr(5,3,"Fork     ");
    lcd_showint16(55,3,flash.forkspeed);

    lcd_showstr(5,4,"MAX      ");
    lcd_showint16(55,4,flash.maxspeed);

    lcd_showstr(5,5,"WZ       ");
    lcd_showint16(55,5,flash.add_wz);

    lcd_showstr(6,6,"IRT    ");
    lcd_showint16(55,6,flash.inrotime);

    lcd_showstr(7,7,"IGT    ");
    lcd_showint16(55,7,flash.ingotime);
    while(1)
    {
        Move_Up_Down(&j,7);
     //-------------------------左右移动改大小--------------------------//
        if(key_check(KEY_L)==KEY_DOWN)                                     //减小
        {
            switch(j){
                case 0: flash.cirspeed-=10;
                    lcd_showint16(55,0,flash.cirspeed);
                    break;
                case 1: flash.garspeed-=10;
                    lcd_showint16(55,1,flash.garspeed);
                    break;
                case 2:
                    flash.ramspeed-=10;
                    lcd_showint16(55,2,flash.ramspeed);
                    break;
                case 3:
                    flash.forkspeed-=10;
                    lcd_showint16(55,3,flash.forkspeed);
                    break;
                case 4:
                    flash.maxspeed-=10;
                    lcd_showint16(55,4,flash.maxspeed);
                    break;
                case 5:
                    flash.add_wz-=1;
                    lcd_showint16(55,5,flash.add_wz);
                    break;
                case 6:
                    flash.inrotime-=1;
                    lcd_showint16(55,6,flash.inrotime);
                    break;
                case 7:
                    flash.ingotime-=1;
                    lcd_showint16(55,7,flash.ingotime);
                    break;
            }

        }

        else if(key_check(KEY_R)==KEY_DOWN)                                     //增大
        {
            switch(j){
                case 0: flash.cirspeed+=10;
                    lcd_showint16(55,0,flash.cirspeed);
                    break;
                case 1: flash.garspeed+=10;
                    lcd_showint16(55,1,flash.garspeed);
                    break;
                case 2:
                    flash.ramspeed+=10;
                    lcd_showint16(55,2,flash.ramspeed);
                    break;
                case 3:
                    flash.forkspeed+=10;
                    lcd_showint16(55,3,flash.forkspeed);
                    break;
                case 4:
                    flash.maxspeed+=10;
                    lcd_showint16(55,4,flash.maxspeed);
                    break;
                case 5:
                    flash.add_wz+=1;
                    lcd_showint16(55,5,flash.add_wz);
                    break;
                case 6:
                    flash.inrotime+=1;
                    lcd_showint16(55,6,flash.inrotime);
                    break;
                case 7:
                    flash.ingotime+=1;
                    lcd_showint16(55,7,flash.ingotime);
                    break;
            }

        }
        if(key_check(KEY_A)==KEY_DOWN)                       //退出
        {
            DisplayMenu();
            break;
        }
    }

}

void Read_Para()
{
    uint8 j=0;
    lcd_clear(WHITE);
    lcd_showstr(0,0,"LOW SPEED");
    lcd_showstr(0,1,"MIDDLE SPEED");
    lcd_showstr(0,2,"FAST SPEED");
    lcd_showstr(0,3,"LAST SPEED");
    lcd_showstr(0,4,"BACK");
    while(1)
    {
        Move_Up_Down(&j, 4);
        if(key_check(KEY_A) == KEY_DOWN)
        {
            flash.vx_speed = memory[j].vx_speed;
            flash.vy_speed = memory[j].vy_speed;
            flash.speed_kp = memory[j].speed_kp;
            flash.speed_ki = memory[j].speed_ki;
            flash.dir_kp = memory[j].dir_kp;
            flash.dir_kd = memory[j].dir_kd;
            flash.fork_kp = memory[j].fork_kp;
            flash.fork_kd = memory[j].fork_kd;
            flash.add_wz = memory[j].add_wz;
            flash.cirspeed = memory[j].cirspeed;
            flash.forkspeed = memory[j].forkspeed;
            flash.ramspeed = memory[j].ramspeed;
            if (!into_gar)   flash.garspeed = memory[j].garspeed;
            flash.maxspeed  = memory[j].maxspeed;
            flash.out_speed  = memory[j].out_speed;
            flash.out_time  = memory[j].out_time;
            flash.maxspeed  = memory[j].maxspeed;
            flash.rotime[0]  = memory[j].rotime[0];
            flash.rotime[1]  = memory[j].rotime[1];
            flash.rotime[2]  = memory[j].rotime[2];
            flash.rospeed  = memory[j].rospeed;
            flash.two  = memory[j].two;

            lcd_clear(WHITE);
            DisplayMenu();
            break;
        }
    }
}

//-------------------------显示菜单列表--------------------------//
void DisplayMenu()
{
    uint8 x=0,y=0;
    lcd_clear(WHITE);
    lcd_showstr(x,y,"1.Go!");
    lcd_showstr(x,++y,"2.Set Para");
    lcd_showstr(x,++y,"3.Set Speed");
    lcd_showstr(x,++y,"4.Set Priority");
    lcd_showstr(x,++y,"5.Read Para");
    lcd_showstr(x,++y,"6.Camera");
}

//-------------------------上下移动--------------------------//
void Move_Up_Down(uint8 *j, uint8 k)
{
    if(key_check(KEY_U)==KEY_DOWN && *j!=0)
    {
        lcd_showstr(130,*j," ");
        (*j)--;
    }
    else if(key_check(KEY_U)==KEY_DOWN && *j==0)
    {
        lcd_showstr(130,*j," ");
        *j=k;
    }
    if(key_check(KEY_D)==KEY_DOWN && *j!=k)
    {
        lcd_showstr(130,*j," ");
        (*j)++;
    }
    else if(key_check(KEY_D)==KEY_DOWN && *j==k)
    {
        lcd_showstr(130,*j," ");
        *j=0;
    }
    lcd_showstr(130,*j,"*");
}

void Set_Mode()
{
    lcd_clear(WHITE);
    while(1)
    {
        if(key_check(KEY_A)==KEY_DOWN)                       //退出
        {
            DisplayMenu();
            break;
        }
    }
}

//-------------------------上下移动--------------------------/
