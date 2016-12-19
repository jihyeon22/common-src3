#CC	= $(CROSS_COMPILE)gcc
#AR	= $(CROSS_COMPILE)ar

TARGET		= logd
LOGD_LIB	= liblogd.a

INC_PATH	+= -I.

LOGD_SVC	= logd.c
SVC_OBJS	= $(LOGD_SVC:.c=.o)
OBJS		= $(SVC_OBJS)

LOGD_LIB_SVC	= logd_clnt.c
LOGD_LIB_OBJS	= $(LOGD_LIB_SVC:.c=.o)

LDLIBS 		+= -lnsl -lpthread

DEST_LIB_PATH       ?= /usr/lib
DEST_INC_PATH       ?= /usr/include

CFLAGS += $(INC_PATH)

all : $(TARGET) $(LOGD_LIB)

$(TARGET):	$(OBJS)
	$(CC) $(LDFLAGS) -o $(TARGET) $(OBJS) $(LDLIBS)
	
%o:	%c
	$(CC) -c $< $(CFLAGS) $(LDFLAGS)	

$(LOGD_LIB): $(LOGD_LIB_OBJS)
	$(AR) rsc $@ $^
	$(RANLIB) $@
	
clean:
	@rm -rf $(OBJS) $(TARGET) $(LOGD_LIB)

install: $(TARGET)
	@if [ ! -d $(DESTDIR)/system/sbin/ ]; then mkdir -p $(DESTDIR)/system/sbin/; fi
	fakeroot cp -va $(TARGET) $(DESTDIR)/system/sbin/$(TARGET)
	
	@if [ ! -d $(DESTDIR)$(DEST_INC_PATH) ]; then mkdir -p $(DESTDIR)$(DEST_INC_PATH); fi
	fakeroot cp -va logd_rpc.h $(DESTDIR)$(DEST_INC_PATH)
	
	@if [ ! -d $(DESTDIR)$(DEST_LIB_PATH)/ ]; then mkdir -p $(DESTDIR)$(DEST_LIB_PATH)/; fi
	fakeroot cp -va $(LOGD_LIB) $(DESTDIR)$(DEST_LIB_PATH)/$(LOGD_LIB)


uninstall: $(TARGET)
	@rm -rf $(TARGET) $(DESTDIR)/system/sbin/$(TARGET)
