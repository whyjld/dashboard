APPNAME		:= test
DESTDIR		?= ../../bin
SRCDIR		?= ../../src
OBJDIR		?= ../../obj

vpath %.c $(SRCDIR)
vpath %.cpp $(SRCDIR)
vpath %.o $(OBJDIR)

BIN = $(DESTDIR)/$(APPNAME)

CFLAGS = -D_GNU_SOURCE -fPIC -fno-strict-aliasing -fno-optimize-sibling-calls -Wall -g
CFLAGS += $(CFLAGS_EGL)
CFLAGS += -DLINUX

ifeq ($(DEBUG),1)
	CFLAGS += -DDEBUG -D_DEBUG 
else
	CFLAGS += -O3 
endif

CFLAGS += -I../vendor/include

OBJECTS += test.o

DEPS += -lstdc++ -lrt -lm -ldl

CPPFLAGS = -std=c++11
CPPFLAGS += $(CFLAGS)

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
