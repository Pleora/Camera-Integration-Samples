The following are the instructions to install the SoftDeviceGEV on VC pro Z Camera



1. Unzip the SoftDeviceGEV_VC.tar.gz on your device


2. Follow up the Readme.txt under SoftDeviceGEV to build the Application


3. Copy the SoftDeviceGEV at /root


4. Modify the configuration on device to make SoftDeviceGEV be launched automatically
   
   
	- Find the MAC Address of the NIC and the root path of ebus sdk
 for example:
              
		MAC_Addr: 00:06:1f:3d:37:75
              
		ROOT_PATH_EBUS_SDK=/opt/pleora/ebus_sdk/linux-gnueabihf-arm   

   
	- Create user_init.sh at /etc/vcint, if it doesn't exist.
   
   
	- Add the following line into user_init.sh:
 
             	#!/bin/bash

              	export GENICAM_ROOT_V3_1=ROOT_PATH_EBUS_SDK/lib/genicam
 
	      	/root/SoftDeviceGEV MAC_Addr
             
              	exit 0
8. Ready to go



