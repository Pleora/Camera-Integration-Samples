#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <vclib.h>
#include <vclinux.h>
#include <flib.h>
#include <vcgpio.h>
#include <vcnano3dz.h>

#define COUNT_CAPT_BUF (12)
// initializtion of variables for camera control
VCCamCfg cam = NULL_VCCamCfg;;
VCCaptCfg cpt[COUNT_CAPT_BUF];
VCBCCfg   cfg = NULL_VCBCCfg;
I32 cpt_index = 0;
I32 physicalSensorSize = 0;
// laser detection position X
F32 *pLaserPosX = NULL;
// laser detection position Z
F32 *pLaserPosZ = NULL;
// laser detection brightness value
U8  *pLaserVal = NULL;

I32 setGPIO(I32 gpioNr, I32 val)
{
	I32        rc;
	I32        fd = -1;
	char       acVal[20];
	VCGpioCfg  gpio;

	rc = vc_gpio_init(&gpio);
	if (rc < 0) {
		printf("vc_gpio_init is failed %d\n", rc);
		goto end;
	}

	//exports gpio and keeps it exported, since prsrvOldExpIff1 is not 1.
	rc = vc_gpio_dir_set(gpioNr, VCGPIODIR_OUT, &gpio, 0);
	if (rc < 0) {
		printf("vc_gpio_dir_set is failed %d\n", rc);
		goto end;
	}
	rc = vc_gpio_open(&fd, gpioNr, "value", 1, &gpio, NULL);
	if (rc < 0) {
		printf("vc_gpio_open is failed %d\n", rc);
		goto end;
	}

	sprintf(acVal, "%d", val);
	rc = write(fd, acVal, strlen(acVal));
	if (rc < 0) {
		printf("vwrit is failed %d\n", rc);
		goto end;
	}
	rc = vc_gpio_close(&fd, gpioNr, &gpio, 1);
	if (rc < 0) {
		printf("vc_gpio_close is failed %d\n", rc);
		goto end;
	}
end:
	vc_gpio_deinit(&gpio, 1);
	return(rc);
}

I32 SwitchLaser(I32 LaserOn)
{
	I32 rc = 0;
	static I32 LaserOnOld = -1;

	if (LaserOn != LaserOnOld)
	{
		LaserOnOld = LaserOn;

		if (LaserOn)
		{
			rc = setGPIO(4, 1); // vcgpio -w 1 -f 4
			usleep(5000);       // 5ms delay to establish laser power
		}
		else
		{
			rc = setGPIO(4, 0); // vcgpio -w 0 -f 4
		}
	}

	return rc;
}

void set_Laser_Mode(U32 nValue)
{
     SwitchLaser(nValue);
}
void set_Exposure(U32 nValue)
{
;
}
void set_Shutter_time(U32 nValue)
{
   U32 i;
   for (i = 0; i < COUNT_CAPT_BUF; i++)  vc_capt_sen_shutter_set_all(&cpt[i], nValue);
}
void set_Gain(U32 nValue)
{
   U32 i;
   for (i = 0; i < COUNT_CAPT_BUF; i++)  vc_capt_sen_gain_set_all(&cpt[i], nValue);
}
void set_camera_height(int aHeight) {
  int i;
  for (i=0; i < COUNT_CAPT_BUF; i++) {
    cpt[i].sen[0].img.dy = aHeight;
  }
}

void set_camera_width(int aWidth) {
  int i;
  for (i=0; i < COUNT_CAPT_BUF; i++) {
  	cpt[i].sen[0].img.dx = aWidth;

 }
}

I32 get_camera_laser_maxpoints() {
  return physicalSensorSize;
}

void camera_init() {
    I32   k, rc, ee=0, i;
    VCSenInit        senInit      = VCSenInit_Default;
    VCTrgInit        trgInit      = VCTrgInit_Default; // TRGMODE_EDGE (default) or TRGMODE_LEVEL
    VCNano3dZInit    nano3dInit   = VCNano3dZInit_Default;
    VCSenCustomCalls nano3dCustom = VCSenCustomCalls_Nano3dZ;

    // check the licese
    rc =  vc_license_init("vclib");
    if (0 != rc) {
      printf("vc_license_init is failed %d\n", rc);
    }

   // ini camera and sensor hardware
   
   // important at special cameras to write the right default values to sInit.
    senInit.custom = nano3dCustom;
  // disableImgIff1: To fasten processing: 1  NO IMAGE OUTPUT
   nano3dInit.disableImgIff1 = 0;
   senInit.cInit.nano3dz = &nano3dInit; // important at special cameras to write the right default values to sInit.
    //set default value
    senInit.maxDx=1408;
    senInit.maxDy=1080;

    // senStepDx = 64;
    // SenStepX  = 4;
    // SenStepY  = 4;
    // maxPitch  = (MaxDx / enStepDx) * SenStepDx;
    senInit.maxPitch= (senInit.maxDx / 64)* 64;

    // Initialize the first capture device (others will be cloned from it later)
    rc =  vc_cam_init(&cam, &cpt[0], NULL, &trgInit, &senInit);
    if (0 != rc) {
       printf("vc_cam_ini is failed %d\n", rc);
    }
    cpt[0].sen[0].img.type = IMAGE_GREY;
    cpt[0].sen[0].img.dx = 1408;
    cpt[0].sen[0].img.dy = 1080;

    physicalSensorSize = cpt[0].sen[0].d.senDx;
    // reset capture config to linear lut
    rc = vc_capt_sen_lut_reset_all(&cpt[0]);
  
    // clone capture
    for(k= 1; k< COUNT_CAPT_BUF; k++)
    {
        rc =  vc_capt_init_clone(&cpt[0], &cpt[k]);
    }

    //prepare buffer for X, Z brightness
    pLaserPosX = (F32 *)malloc(physicalSensorSize * sizeof(F32));
    memset (pLaserPosX, 0, physicalSensorSize * sizeof(F32));
    pLaserPosZ = (F32 *)malloc(physicalSensorSize * sizeof(F32));
    memset (pLaserPosX, 0, physicalSensorSize * sizeof(F32));
    pLaserVal = (U8 *)malloc(physicalSensorSize * sizeof(U8));
    memset (pLaserPosX, 0, physicalSensorSize * sizeof(U8));
    
    // initialize the device 
    set_Laser_Mode(0);
    set_Exposure(0);
    set_Shutter_time(1000);
    set_Gain(100);
    //printf("Camera Init of %d \n", rc);
	    
};

unsigned char *get_camera_image_3D_data(U32 *pDetectedLaserPoints, F32 *pLaserPosX_MM, F32 *pLaserPosZ_MM) {

    I32 i, cnt=0, rc=0;
    VCNano3dZResult *Result_0;
    cpt_index++; 
    cpt_index = cpt_index % COUNT_CAPT_BUF;
    rc = vc_capt_request(&cpt[cpt_index]);
    if (rc < 0) return NULL;	
    rc =  vc_capt_wait(&cpt[cpt_index],-1);  
    if (rc < 0) return NULL;	

    Result_0 = &cpt[cpt_index].sen[0].c.nano3dz->result;

  // pointer to laser positions for both images at double shutter
    F32 *restrict pX = pLaserPosX, *restrict px = Result_0->x;
    F32 *restrict pZ = pLaserPosZ, *restrict pz = Result_0->y;
    U8  *restrict pV = pLaserVal;
    I16 *restrict pv = Result_0->vMax, *restrict pdy = Result_0->dy;

    //Non TransferAllPos_MM
    for(i=0; i < Result_0->sliceCnt; i++)
    {
        if(*pdy++ > 0)
        {
          *pX++ = *px;
          *pZ++ = *pz;
          *pV++ = *pv;
        }
        px++;
        pz++;
        pv++;
    }
    // set number of valid laser detection points
    cnt = (I32)(pV - pLaserVal);
    // offer mapper for 'pix to mm' and 'mm to pix' conversion
    vc_nano3dz_external_mapper_toMM_(pLaserPosX, pLaserPosZ, pLaserPosX_MM, pLaserPosZ_MM, cnt, cpt[cpt_index].sen[0].c.nano3dz->cfg.d.e.mappers);
    *pDetectedLaserPoints = cnt;
    return cpt[cpt_index].sen[0].img.st;
}

void camera_delete() {
   I32 k;

   printf("camera delete \n");

// vc_capt_remove_all_requests(COUNT_CAPT_BUF, cpt);
  
    for(k= 0; k< COUNT_CAPT_BUF; k++) {
     vc_capt_deinit(&cpt[k]);
   }
   vc_cam_deinit(&cam);
   free(pLaserPosX);
   free(pLaserPosZ);
   free(pLaserVal);
}
