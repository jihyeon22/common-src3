SRCROOT             := $(PWD)
DESTDIR	            := $(SRCROOT)/out
DEST_INC_PATH       := /usr/include
DEST_LIB_PATH       := /usr/lib
WORK_PATH           := /system


VER                 ?= 0
###############################################################################
# Board Setting

BOARD	:=
CFLAGS	+= -DBOARD_$(BOARD)

###############################################################################
# Compile

CC = $(CROSS_COMPILE)gcc
AR = $(CROSS_COMPILE)ar
RANLIB = $(CROSS_COMPILE)ranlib

CFLAGS	:= $(EXTRA_CFLAGS)
LDFLAGS	:= $(EXTRA_LDFLAGS)

SUBDIRS = logd mdsapi mon

export CROSS_COMPILE CC AR RANLIB BOARD CFLAGS LDFLAGS DESTDIR WORK_PATH DEST_INC_PATH DEST_LIB_PATH VER

all: subdirs

subdirs:
	$(Q)for dir in $(SUBDIRS) ; do \
		echo ""; \
		echo [Compile $$dir...]-------------------------------------------; \
		make -C $$dir || exit $?; \
	done

install: install-subdirs

install-subdirs:
	$(Q)$(call check_install_dir, $(DESTDIR)$(WORK_PATH))
	$(Q)for dir in $(SUBDIRS) ; do \
		echo ""; \
		echo [Install $$dir...]-------------------------------------------; \
		make -C $$dir install || exit $?; \
	done

clean: clean-subdirs

clean-subdirs:
	$(Q)for dir in $(SUBDIRS) ; do \
		echo ""; \
		echo [Clean $$dir...]-------------------------------------------; \
		make -C $$dir clean || exit $?; \
	done

uninstall: ;
target: ;


###############################################################################
# Function

define check_install_dir
	if [ ! -d "$1" ]; then mkdir -p $1; fi
endef
