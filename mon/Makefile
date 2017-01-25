#CC	= $(CROSS_COMPILE)gcc
#AR	= $(CROSS_COMPILE)ar

CFLAGS	+= -DINI_PATH=\"/system/mds/system/bin/mon.ini\"
OBJS	= launcher.o mon.o
NAME	= mon
OUTDIR 	= ./
TARGET	= $(OUTDIR)$(NAME)
LIBS	= -liniparser -lmdsapi

PREFIX		:= /system
SBINDIR		:= $(PREFIX)/sbin
BINDIR		:= $(PREFIX)/$(VER)

###############################################################################
# Board Setting

BOARD	:=
CFLAGS	+= -DBOARD_$(BOARD)

###############################################################################

all:	$(TARGET)


$(TARGET):	$(OBJS)
	$(CC) -o $@ $^ $(LDFLAGS) $(LIBS)
	
%o:	%c
	$(CC) -c $< $(CFLAGS) $(LDFLAGS)

clean:
	rm -rf $(TARGET) $(OBJS) 
	
install:
	$(Q)$(call check_install_dir, $(DESTDIR)$(BINDIR))
	$(Q)fakeroot cp -v $(TARGET) $(DESTDIR)$(BINDIR)/$(TARGET)


#install:
#	$(Q)$(call check_install_dir, $(DESTDIR)$(WORK_PATH))
#	cp -v $(TARGET) $(DESTDIR)$(WORK_PATH)/$(NAME)
	
uninstall:
	rm -rf $(DESTDIR)$(WORK_PATH)/$(NAME)

###############################################################################
# Functions

define check_install_dir
	if [ ! -d "$1" ]; then mkdir -p $1; fi
endef
