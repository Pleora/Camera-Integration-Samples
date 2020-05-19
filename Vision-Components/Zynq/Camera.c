#include <vclib.h>
#include <vclinux.h>
#include <flib.h>
#include <stdio.h>
#include <string.h>

#define COUNT_CAPT_BUF (2)
static VCCamCfg cam = NULL_VCCamCfg;;
static VCCaptCfg cpt[COUNT_CAPT_BUF];

static VCBCCfg   cfg = NULL_VCBCCfg;
static VCBCInit   init = VCBCInit_Default;
static I32 cpt_index = 0;
static I32 barcode_read = 0;

void camera_init() {
 I32   k, rc, ee=0, i;

    VCSenInit senInit = VCSenInit_Default;
    senInit.maxDx=1920;
    senInit.maxDy=1080;
    senInit.maxPitch=960;
    rc =  vc_license_init("vclib");
    if (0 != rc) {
       printf("vc_license_init is failed %d\n", rc);
    }
    rc =  vc_cam_init(&cam, &cpt[0], NULL, NULL, &senInit);
    if (0 != rc) {
       printf("vc_cam_ini is failed %d\n", rc);
    }
    cpt[0].sen[0].shutter = 12345;
    cpt[0].sen[0].img.type = IMAGE_GREY;
    cpt[0].sen[0].img.dx = 960;
    cpt[0].sen[0].img.dy = 720;

    cfg.bcReadType[VCBCTYPE_EAN13] = 1;
    cfg.bcReadType[VCBCTYPE_128] = 1;
    rc = vc_bc_init(&cfg, &init);
    if (0 != rc) {
       printf("vc_bc_init is failed %d\n", rc);
    }
    for(k= 1; k< COUNT_CAPT_BUF; k++)
    {
        rc =  vc_capt_init_clone(&cpt[0], &cpt[k]);
    }
    printf("Camera Init of %d \n", rc);

    
};

void set_camera_height(int aHeight) {
  int i;

  for (i=0; i < COUNT_CAPT_BUF; i++) {
    cpt[i].sen[0].img.dy = aHeight;
  }
}

void set_camera_width(int aWidth) {
  cpt[cpt_index].sen[0].img.dx = aWidth;

}
void set_barcode_read(int bAction) {
  barcode_read = bAction;
}

unsigned char*  camera_image_ptr() {
 I32   rc, ee=0, i, nr;
 SPtrList bcLst = NULL_LIST;
 VCBCRegion *bc = NULL;

 cpt_index++; 
 cpt_index = cpt_index % COUNT_CAPT_BUF;
 rc = vc_capt_request(&cpt[cpt_index]); 
 rc=  vc_capt_wait(&cpt[cpt_index],-1);  

 if (barcode_read)
 { 
    rc = vc_bc_search(&cpt[cpt_index].sen[0].img, &cfg, &bcLst);
    if(0!=bcLst.size)
    {
        for(nr= 0; nr< bcLst.size; nr++)
        {
            I32 rc;
            bc = (VCBCRegion*) PtrList_getPtr(&bcLst, nr);
//          printf("  Barcode %2d/%-2d Content:  %s\n", nr+1, bcLst.size, bc->text);
            rc = chprint_centered_at_pos(
              &cpt[cpt_index].sen[0].img, 
              bc->text,  
              bc->centerX, 
              bc->centerY,
              4,
              4,
              255);
            if (0 == rc)
            {
                line(&cpt[cpt_index].sen[0].img, bc->cornerX[0], bc->cornerY[0], 
                   bc->cornerX[1], bc->cornerY[1], 255, FL_wpset_U8);
               line(&cpt[cpt_index].sen[0].img, bc->cornerX[1], bc->cornerY[1], 
                   bc->cornerX[2], bc->cornerY[2], 255, FL_wpset_U8);
               line(&cpt[cpt_index].sen[0].img, bc->cornerX[2], bc->cornerY[2], 
                   bc->cornerX[3], bc->cornerY[3], 255, FL_wpset_U8);
               line(&cpt[cpt_index].sen[0].img, bc->cornerX[3], bc->cornerY[3], 
                  bc->cornerX[0], bc->cornerY[0], 255, FL_wpset_U8);
            }
        }
    }
  }
  return cpt[cpt_index].sen[0].img.st;
}

void camera_delete() {
   I32 k;

   printf("camera delete \n");
   vc_bc_deinit(&cfg);
   for(k= 0; k< COUNT_CAPT_BUF; k++) {
     vc_capt_deinit(&cpt[k]);
   }
   vc_cam_deinit(&cam);
}
