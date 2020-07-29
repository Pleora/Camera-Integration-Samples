# Parameters
# SRC_CS: The source C files to compie
# SRC_CPPS: The source CPP files to compile
# EXEC: The executable name
SRC_CPPS = \
           MyEventSink.cpp \
           MyRegisterEventSink.cpp \
           Region.cpp \
           Enums.cpp \
           Scan3dCoordinate.cpp \
           Scan3dExtraction.cpp \
           Scan3dPattern.cpp \
           Scan3dTransform.cpp \
           SoftDeviceGEV3d.cpp \
           Source3d.cpp   \
           Utilities.cpp 

SRC_CS = \
        Camera.c\

EXEC     = SoftDeviceGEV3d_VC-3D-Nano

ifeq ($(SRC_CS) $(SRC_CPPS),)
  $(error No source files specified)
endif

ifeq ($(EXEC),)
  $(error No executable file specified)
endif

CC                  = arm-linux-gnueabihf-gcc
CXX                 = arm-linux-gnueabihf-g++

PUREGEV_ROOT        ?= ../../..
PV_LIBRARY_PATH      =$(PUREGEV_ROOT)/lib

CFLAGS              += -I$(PUREGEV_ROOT)/include -I/usr/include/vc
CPPFLAGS            += -I$(PUREGEV_ROOT)/include -I/usr/include/vc


ifdef SCAN3D_FEATURE_ENABLE
SRC_CPPS += SourceLeft.cpp  SourceRight.cpp
CPPFLAGS += -DSCAN3D_FEATURE_ENABLE
endif

ifdef _DEBUG
    CFLAGS    += -g -D_DEBUG
    CPPFLAGS  += -D_UNIX_ -D_LINUX_ -fPIC -std=c++11
else
    CFLAGS    += -O3
    CPPFLAGS  += -O3
endif
CFLAGS    += -D_UNIX_ -D_LINUX_ -fPIC
CPPFLAGS  += -D_UNIX_ -D_LINUX_ -fPIC -std=c++11

LDFLAGS             += -L$(PUREGEV_ROOT)/lib         \
                        -lPvAppUtils                 \
                        -lPvBase                     \
                        -lPvBuffer                   \
                        -lPvSystem                   \
                        -lPvVirtualDevice            \
			-lvclinux                    \
			-lvclib                      \
			-lvcflib                     

LDFLAGS      += -Wl,-rpath=$(PUREGEV_ROOT)/lib

LD_LIBRARY_PATH       = $(PV_LIBRARY_PATH):$(QT_LIBRARY_PATH):$(GEN_LIB_PATH)
export LD_LIBRARY_PATH

OBJS      += $(SRC_CS:%.c=%.o)
OBJS      += $(SRC_CPPS:%.cpp=%.o)

all: $(EXEC)

clean:
	rm -rf $(OBJS) $(EXEC) $(SRC_MOC) $(SRC_QRC)

moc_%.cxx: %.h
	$(MOC) $< -o $@ 

qrc_%.cxx: %.qrc
	$(RCC) $< -o $@

%.o: %.cxx
	$(CXX) -c $(CPPFLAGS) -o $@ $<

%.o: %.cpp
	$(CXX) -c $(CPPFLAGS) -o $@ $<

%.o: %.c
	$(CC) -c $(CFLAGS) -o $@ $<

$(EXEC): $(OBJS)
	$(CXX) $(OBJS) -o $@ $(LDFLAGS) 

.PHONY: all clean
