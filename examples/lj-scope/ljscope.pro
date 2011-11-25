## Modify these variables:
TEMPLATE	= app
CONFIG		+= qt warn_on debug
HEADERS		= labjackclass.h u6.h renderarea.h  window.h gain.h
SOURCES		= labjackclass.cpp u6.c main.cpp renderarea.cpp window.cpp gain.cpp
TARGET		= ljscope
LIBS            += -llabjackusb
