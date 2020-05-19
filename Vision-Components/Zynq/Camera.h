#pragma once

extern "C" void camera_init();

extern "C" void camera_capture(unsigned char*  aDataPtr, int aSize);
extern "C" void camera_delete();
extern "C" unsigned char* camera_image_ptr();
extern "C" void set_camera_height(int aHeight);
extern "C" void set_camera_width(int aWidth);
extern "C" void set_barcode_read(int Action);

