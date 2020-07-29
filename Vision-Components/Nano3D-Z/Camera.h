#pragma once

extern "C" void camera_init();

extern "C" void camera_delete();
extern "C" int get_camera_laser_maxpoints();
extern "C" unsigned char* get_camera_image_3D_data(int *pDetectedLaserPoints, float *pLaserPosX_MM, float *pLaserPosZ_MM);
extern "C" void set_camera_height(int aHeight);
extern "C" void set_camera_width(int aWidth);
extern "C" void set_Laser_Mode(int nValue);
extern "C" void set_Exposure(int nValne);
extern "C" void set_Shutter_time(int nValue);
extern "C" void set_Gain(int nValue);

