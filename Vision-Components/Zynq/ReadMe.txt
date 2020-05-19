Copyright (c) 2020, Pleora Technologies Inc., All rights reserved.

===================================
SoftDeviceGEV - Vision Components
===================================

1. Description

This sample shows how to create and run a Software GigE Vision device for Vision Components (VC) PRO Z Camera. This sample encorporates barcode reading functionality 
from VC processing libraries and allows the user to enable/disable this functionality via a GenApi feature (BarcodeReader). 

2. Prerequisites

This sample assumes that:
 * You have a network adapter installed on your PC with a valid IP address.
 * You have a GigE Vision controller/receiver that can receive and display images (such as eBUSPlayer or any other GigE Vision receiver that supports the GVSP protocol). 
   The receiver should be reachable and on the same subnet as the interface from which it will be receiving.
 * You have an eBUS SDK Software GigE Vision Device license installed on your system.

3. Build Tool:

3.1 arm-linux-gnueabihf version 4.9 or higher
3.2 VC libaries as below or higher version
           * vclib.so.6.8.0
           * vcflib.so.5.1.0
           * vclinux.so.3.7.2 
3.3 Requires libstdc++.so.6.0.25 // you can dowload it from https://packages.debian.org/buster/all/libstdc++6-armhf-cross/download 
3.4 eBUS SDK for arm32 (hf) vesion 6.2.0 or higher

4. Description

4.1 SoftDeviceGEV.cpp

Main entry point of the sample.

4.1 MyEventSink.cpp

Implementation of the IPvSoftDeviceGEVEventSink interface. Used for event logging, creation of custom registers and creation of custom GenApi features.

4.2 MyRegisterEventSink.cpp

Implementation of the IPvRegisterEventSink interface. Used to demonstrate how to handle register events.

4.3 MySource.cpp

Implementation of the IPvStreamingChannelSource interface. Used to show how to properly implement and manage an image source in the context of a Software GigE Vision Device. 
Supports the Mono8, RGB16 and RGBa16 pixel formats. Data chunks are fully supported once enabled from the GenApi interface of the software Gige Vision device.

4.4 Utilities.cpp

Stand-alone functions used throughout the sample. DumpRegister shows how to browse the IPvRegisterMap of PvSoftDeviceGEV. 
FireTestEvents shows how to generate events on the messaging channel.


IMPORTANT: If you do not have a Pleora Developer Seat License for eBUS SDK or a eBUS GEV-Tx Runtime license installed on your system,
it will restrict your ability to customize the identity of the device and will disconnect all applications after 15 minutes.





