/*
 * cam.h
 *
 *  Created on: Jan 20, 2021
 *      Author: yy
 */

#ifndef USER_CAM_H_
#define USER_CAM_H_
#include "image_process.h"

//定义处理图像的大小
#define width                MT9V03X_W
#define height               MT9V03X_H



extern int16 Cam_Error;
extern int16 remote_radio;

void Camera_Init();
void Cam_Handle_Loop();
void CenterLineHandle();
void Check_Error();
#endif /* USER_CAM_H_ */
