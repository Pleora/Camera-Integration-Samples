# Parameters
# SRC_CS: The source C files to compie
# SRC_CPPS: The source CPP files to compile
# EXEC: The executable name
SRC_CPPS = \
           MyEventSink.cpp \
           MyRegisterEventSink.cpp \
           MySource.cpp \
           SoftDeviceGEV.cpp \
           Utilities.cpp \

SRC_CS = \
        Camera.c\

EXEC     = SoftDeviceGEV

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
ifdef _DEBUG
    CFLAGS    += -g -D_DEBUG
    CPPFLAGS  += -D_UNIX_ -D_LINUX_ -fPIC -std=c++11
else
    CFLAGS    += -O3
    CPPFLAGS  += -O3
endif
CFLAGS    += -D_UNIX_ -D_LINUX_ -fPIC
CPPFLAGS  += -D_UNIX_ -D_LINUX_ -fPIC

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
