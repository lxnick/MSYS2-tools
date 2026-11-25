# try to do some autodetecting
UNAME := $(shell uname -s)
ARCH := $(shell uname -m)

ifeq "$(UNAME)" "Darwin"
	OS=macos
endif
ifeq "$(OS)" "Windows_NT"
	OS=windows
endif
ifeq "$(UNAME)" "Linux"
	OS=linux
endif

# deal with stupid Windows not having 'cc'
ifeq (default,$(origin CC))
  CC = gcc
endif
ifeq (default,$(origin CXX))
  CXX = g++
endif

#############  Mac
ifeq "$(OS)" "macos"

CFLAGS+=`pkg-config hidapi --cflags`
CXXFLAGS+=$(CFLAGS)
LIBS=-lhidapi -framework IOKit -framework CoreFoundation -framework AppKit
EXE=

endif

############# Windows
ifeq "$(OS)" "windows"

CFLAGS+=`pkg-config hidapi --cflags`
CXXFLAGS+=$(CFLAGS)
LIBS+= -lhidapi -lsetupapi -Wl,--enable-auto-import
EXE=.exe

endif

############ Linux (hidraw)
ifeq "$(OS)" "linux"

LIBS = `pkg-config libudev --libs`
CFLAGS+=`pkg-config hidapi-libusb --cflags`
CXXFLAGS+=$(CFLAGS)
LIBS+=`pkg-config hidapi-libusb --libs`
EXE=

endif


############# common

CFLAGS+=-Wall
CXXFLAGS+=-Wall

# === source files ===
C_SRCS   = data-center-csv.c
CPP_SRCS = data-center.cpp

OBJS = $(C_SRCS:.c=.o) $(CPP_SRCS:.cpp=.o)

TARGET = data-center-csv$(EXE)

all: $(TARGET)

# C compile
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# C++ compile
%.o: %.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

# link (IMPORTANT: use CXX)
$(TARGET): $(OBJS)
	$(CXX) $(OBJS) -o $(TARGET) $(LIBS)

clean:
	rm -f $(OBJS)
	rm -f $(TARGET)

package: $(TARGET)
	@echo "Packaging up data-center-csv for '$(OS)-$(ARCH)'"
	7z a data-center-csv-$(OS)-$(ARCH).zip $(TARGET)
