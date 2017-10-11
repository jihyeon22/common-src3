##############################################################################
# Path

SRCROOT := $(PWD)
DESTDIR := $(SRCROOT)/out
LIB_PATH   := /usr/lib
INC_PATH   := /usr/include

CFLAGS	:= $(EXTRA_CFLAGS)
LDFLAGS	:= $(EXTRA_LDFLAGS)

###############################################################################
# Board

BOARD	:= 
CFLAGS	+= -DBOARD_$(BOARD)

###############################################################################
# Compile

CC = $(CROSS_COMPILE)gcc
AR = $(CROSS_COMPILE)ar
RANLIB = $(CROSS_COMPILE)ranlib

TARGET	= libmdsapi.a
CFLAGS += -W -Wall
HEADERS = include/mdsapi/


LIB_SRCS  = src/crc.c src/debug.c src/file.c src/gpio.c src/nettools.c src/power.c 
LIB_SRCS  += src/proc.c src/process.c src/stackdump.c src/string.c src/time.c
LIB_SRCS  += src/uart.c src/validation.c 
LIB_SRCS  += src/udp_ipc_server.c src/udp_ipc_clnt.c 
LIB_SRCS  += src/gps_util.c
LIB_SRCS  += src/mds_shm.c 
LIB_SRCS  += src/led.c 
LIB_SRCS  += src/adc.c 
LIB_SRCS  += src/aes.c 
LIB_SRCS  += src/base64.c 

LIB_OBJS = $(LIB_SRCS:.c=.o)

LIB_LIBS += -lpthread
LIB_INCS = -Iinclude -Isrc

#####################################
TEST_SRCS1 = testapp/mds_api_test.c
TEST_OBJS1 = $(TEST_SRCS1:.c=.o)

TEST_SRCS2 = testapp/mds_shm_test.c
TEST_OBJS2 = $(TEST_SRCS2:.c=.o)

TEST_LIBS += -lpthread -lmdsapi


#####################################
TEST_IPC_SRCS_1 = testapp/udp_ipc_test/udp_ipc_test_svr.c
TEST_IPC_SRCS_2 = testapp/udp_ipc_test/udp_ipc_test_clnt.c
TEST_IPC_SRCS_3 = testapp/udp_ipc_test/udp_ipc_test_clnt_gps.c

TEST_IPC_OBJS_1 = $(TEST_IPC_SRCS_1:.c=.o)
TEST_IPC_OBJS_2 = $(TEST_IPC_SRCS_2:.c=.o)
TEST_IPC_OBJS_3 = $(TEST_IPC_SRCS_3:.c=.o)

TEST_IPC_LIBS += -lpthread -lmdsapi

#all: all-before libs $(BIN) samples all-after
all: $(TARGET) install api_sample shm_test test_udp_ipc_svr test_udp_ipc_clnt udp_ipc_test_clnt_gps

.c.o:
	$(CC) $(CFLAGS) $(LIB_LIBS) $(LIB_INCS) -c $< -o $@

install:
	fakeroot cp -va $(TARGET) $(DESTDIR)$(LIB_PATH)
	fakeroot cp -va $(HEADERS) $(DESTDIR)$(INC_PATH)

api_sample : $(TEST_OBJS1)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(TEST_LIBS) 
	
shm_test : $(TEST_OBJS2)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(TEST_LIBS) 
    
test_udp_ipc_svr : $(TEST_IPC_OBJS_1)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(TEST_IPC_LIBS) 

test_udp_ipc_clnt : $(TEST_IPC_OBJS_2)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(TEST_IPC_LIBS) 
    
udp_ipc_test_clnt_gps : $(TEST_IPC_OBJS_3)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS) $(TEST_IPC_LIBS) 
	
$(TARGET): $(LIB_OBJS)
	$(AR) rsc $@ $^
	$(RANLIB) $@

clean:
	rm -vrf $(TARGET) $(LIB_OBJS)
	rm -vrf $(DESTDIR)$(LIB_PATH)/$(TARGET)
	rm -vrf $(DESTDIR)$(INC_PATH)/mdsapi
	rm -vrf shm_test test_udp_ipc_svr test_udp_ipc_clnt $(TEST_OBJS1) $(TEST_OBJS2) $(TEST_IPC_OBJS_1) $(TEST_IPC_OBJS_2)
	
