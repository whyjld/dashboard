APPNAME		:= dashboard
DESTDIR		?= ../bin
SRCDIR		?= ../src
OBJDIR		?= ../obj

vpath %.c $(SRCDIR)
vpath %.cpp $(SRCDIR)
vpath %.o $(OBJDIR)

# Make command to use for dependencies
#CC = $(CROSS_COMPILE)gcc
#AR = $(CROSS_COMPILE)ar

TARGET_PATH_LIB = $(SDKTARGETSYSROOT)/usr/lib
TARGET_PATH_INCLUDE = $(SDKTARGETSYSROOT)/usr/include
#COMMON_DIR=../common

BIN = $(DESTDIR)/$(APPNAME)

CFLAGS = -D_GNU_SOURCE -fPIC -fno-strict-aliasing -fno-optimize-sibling-calls -Wall -g
CFLAGS += $(CFLAGS_EGL)
CFLAGS += -DLINUX

ifeq ($(DEBUG),1)
	CFLAGS += -DDEBUG -D_DEBUG 
else
	CFLAGS += -O3 
endif

ifeq ($(ARCH),arm)
	CFLAGS += -DEGL_API_FB
else
	CFLAGS += -DEGL_USE_X11
endif

CFLAGS += \
	-I$(TARGET_PATH_INCLUDE) \
	-I../vendor/include

CPPFLAGS = -std=c++11
CPPFLAGS += $(CFLAGS)

OBJECTS += \
	main.o \
	fslegl.o \
	tga.o \
	texrect.o \
	dashboard.o \
	billboard.o

ifeq ($(ARCH),arm)
	DEPS = -lstdc++ -lm -lGLESv2 -lEGL  -ldl -Wl,--library-path=$(TARGET_PATH_LIB),-rpath-link=$(TARGET_PATH_LIB)
else
	DEPS = -lstdc++ -lm -lGLESv2 -lEGL -lX11 -ldl -Wl,--library-path=$(TARGET_PATH_LIB),-rpath-link=$(TARGET_PATH_LIB)
endif

$(BIN) : $(OBJECTS)
	@echo " LD " $@
	$(QUIET)$(CC) -o $(BIN) $(addprefix $(OBJDIR)/,$(OBJECTS)) $(DEPS)

%.o : %.c
	@echo " CC " $@
	$(QUIET)$(CC) $(CFLAGS) -MMD -c $< -o $(OBJDIR)/$@

%.o : %.cpp
	@echo " CXX " $@
	$(QUIET)$(CC) $(CPPFLAGS) -MMD -c $< -o $(OBJDIR)/$@

clean:
	rm -f $(addprefix $(OBJDIR)/,$(OBJECTS)) *~ $(addprefix $(OBJDIR)/,$(OBJECTS:.o=.d)) $(BIN)
	
install:
	cp -f $(APPNAME) $(DESTDIR)/.
	
	
-include $(OBJECTS_TUT:.o=.d)
